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

keep=1

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

counter=0
errors=0

expect_re()
{
    counter=`expr $counter + 1`
    read line
    count=`echo "$line" | egrep -e "^$2\$" | wc -l`
    if test $count == 1; then
        echo "$1/$counter: PASSED"
    else
        echo "$1/$counter: FAILED"
        errors=`expr $errors + 1`
        echo " expected RE: ^$2\$"
        echo " found      : $line"
    fi
    test $count == 1
}

#
# Special case for test25
#
test25()
{
    counter=0
    errors=0

    # version is %02d%1d%1d
    # full version %02d%1d%1d%04d
    version=`$cpp -v`
    x=`echo $version | sed 's/^/v1=/;s/\./ v2=/;s/\./ v3=/;s/\./ v4=/'`
    eval "$x"

    test "$v1" -lt "10" && v1="0$v1"
    test "$v4" -lt "10" && v4="0$v4"
    test "$v4" -lt "100" && v4="0$v4"
    test "$v4" -lt "1000" && v4="0$v4"

    spirit_pp="0x$v1$v2$v3"
    spirit_pp_version="0x$v1$v2$v3$v4"
    spirit_pp_version_str="$version"

    expect_re $1 '"__STDC__": *1'
    expect_re $1 '"__cplusplus": *199711L'
    expect_re $1 '"__DATE__": *"[A-Z][a-z][a-z] [0-3]?[0-9] 20[0-9][0-9]"'
    expect_re $1 '"__TIME__": *"[0-2]?[0-9]:[0-5][0-9]:[0-5][0-9]"'
    expect_re $1 '"__LINE__": *21'
    expect_re $1 '"__FILE__": *.*test25.cpp'
    expect_re $1 '"__SPIRIT_PP__": *'"$spirit_pp"
    expect_re $1 '"__SPIRIT_PP_VERSION__": *'"$spirit_pp_version"
    expect_re $1 '"__SPIRIT_PP_VERSION_STR__": *"'"$spirit_pp_version_str"'"'
    expect_re $1 50
    expect_re $1 test.cpp
    expect_re $1 54
    expect_re $1 test.cpp
}

test25 test_files/test25.cpp < $dir/test25.out

