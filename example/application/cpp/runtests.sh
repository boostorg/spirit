#! /bin/sh
#==============================================================================
#  Spirit V1.5.2
#  Copyright (c) 2003 Martin Wille
#  http://spirit.sourceforge.net/
#
#  Permission to copy, use, modify, sell and distribute this software
#  is granted provided this copyright notice appears in all copies.
#  This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.
#===============================================================================

cpp=./bin/cpp/gcc/debug/runtime-link-dynamic/cpp
test "X$CPP" != "X" && cpp="$CPP"

dir="tmp_test"

if test "$keep" = 0; then
    trap "rm -rf $dir" EXIT
fi

test -d $dir && rm -rf $dir
mkdir $dir

tests1=`find test_files -type f -name "test?.cpp" | sort`
tests2=`find test_files -type f -name "test??.cpp" | sort`

tests="$tests1 $tests2"

differs()
{
    diff $1 $2 >/dev/null 2>&1
    test $? != 0
}

logdiffers()
{
    sed 's/^[^(]*//' < $1 | diff - $2 > /dev/null 2>&1
    test $? != 0
}

for t in $tests
do
    bn=`basename $t`
    bn=`basename $bn .cpp`
    ($cpp $t 2>$dir/$bn.log; echo $? > $dir/$bn.return) | \
        grep -v '^$' | grep -v '^# [0-9]' > $dir/$bn.out
    grep '^//E ' $t | sed 's%^//E %%' > $dir/${bn}_expected.out
    grep '^//Eignore' $t >/dev/null 2>&1 && echo yes > $dir/${bn}_ignore.out
    grep '^//L ' $t | sed 's%^//L %%' > $dir/${bn}_expected.log
    grep '^//R ' $t | sed 's%^//R %%' > $dir/${bn}_expected.return

    passed=1
    reasons=""
    # Check the output
    if test -f $dir/${bn}_ignore.out; then
        reasons="$reasons output ignored."
    else
        if differs $dir/${bn}.out $dir/${bn}_expected.out ; then
            passed=0;
            reasons="$reasons output differs."
        fi
    fi

    # Check the diagnostic
    if logdiffers $dir/${bn}.log $dir/${bn}_expected.log ; then
        passed=0;
        reasons="$reasons diagnostic differs."
    fi

    # Check the return code
    if differs $dir/${bn}.return $dir/${bn}_expected.return ; then
        passed=0;
        reasons="$reasons return code differs."
    fi
    
    if test $passed = 1 ; then
        echo "$t:PASSED: $reasons"
    else
        echo "$t:FAILED: $reasons"
    fi
done


