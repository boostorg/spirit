#! /bin/sh

#
# Shell script for running the Spirit regression test suite and generating
# a html table of results.

# 
# This script requires a Boost source tree to run. During the run some
# of Spirit's files are copied to $(BOOST_ROOT)/libs/spirit_test.

#
# Operation:
#
# - this script has to be started with PWD==$(SPIRIT_ROOT)/libs/spirit/test
# - bjam and other tools are rebuilt from their sources in $(BOOST_ROOT)/tools
# - files are copied to $(BOOST_ROOT)/libs/spirit_test.
# - the tests are run in $(BOOST_ROOT)/libs/spirit_test
# - the results are copied back to $(SPIRIT_ROOT)/libs/spirit/test/status
# - results: status/cs-<system>.html and status/cs-<system>-links.html

#
# Environment variables:
#
# BOOST_ROOT defines where Boost can be found. If not set /home/boost
#            is assumed.
#
#

# Set the following variables to configure the operation. Variables you
# should set, i.e. usually required are listed first. Optional variables
# have reasonable defaults for most situations.


### THESE SHOULD BE CHANGED!

#
# "boost_root" points to the root of you boost installation:
# This can be either a non-exitent directory or an already complete Boost
# source tree.
# if BOOST_ROOT is defined then BOOST_ROOT will be used.
#
boost_root=/home/boost

if test "x$BOOST_ROOT" != "x"; then
    boost_root="$BOOST_ROOT"
fi

#
# Wether to fetch the most current Boost code from CVS (yes/no):
# There are two contexts to use this script in: on an active Boost CVS
# tree, and on a fresh Boost CVS tree. If "yes" is specified here an attempt
# to fetch the latest CVS Boost files is made. For an active Boost CVS
# the CVS connection information is used. If an empty tree is detected
# the code is fetched with the anonymous read only information.
#
cvs_update=no

#
# "test_tools" are the Boost.Build toolsets to use for building and running the
# regression tests. Specify a space separated list, of the Boost.Build toolsets.
# Each will be built and tested in sequence.
#
test_tools="gcc-2.95.3 gcc-3.0.4 gcc-3.1 gcc-3.2 gcc-3.2.2"

#
# "toolset" is the Boost.Build toolset to use for building the helper programs.
# This is usually different than the toolsets one is testing. And this is
# normally a toolset that corresponds to the compiler built into your platform.
#
toolset=gcc;



### DEFAULTS ARE OK FOR THESE.

#
# "exe_suffix" the suffix used by exectable files:
# In case your platform requires use of a special suffix for executables specify
# it here, including the "." if needed. This should not be needed even in Windows
# like platforms as they will execute without the suffix anyway.
#
exe_suffix=

#
# "bjam" points to your built bjam executable:
# The location of the binary for running bjam. The default should work
# under most circumstances.
#
bjam=$boost_root/tools/build/jam_src/bin/bjam$exe_suffix

#
# "process_jam_log", and "compiler_status" paths to built helper programs:
# The location of the executables of the regression help programs. These
# are built locally so the default should work in most situations.
#
process_jam_log=$boost_root/tools/regression/build/run/process_jam_log$exe_suffix
compiler_status=$boost_root/tools/regression/build/run/compiler_status$exe_suffix

#
# "boost_build_path" can point to additional locations to find toolset files.
#
boost_build_path=$HOME/.boost-build


### NO MORE CONFIGURABLE PARTS.

#
# Some setup.
#

split_path=':'
#if test `uname -s | grep -i WIN | grep -iv darwin | wc -l` != 0 ; then
#    split_path=';'
#fi

boost_dir=`basename $boost_root`
export BOOST_BUILD_PATH="$boost_build_path$split_path$BOOST_BUILD_PATH"

spirit_tests_src_dir="`pwd`"
spirit_tests_dir="$boost_root/libs/spirit_test"

cd ../../..
spirit_hdr_include="-sSPIRIT_HEADER_INCLUDE=\"`pwd`\""
spirit_src_include="-sSPIRIT_SRC_INCLUDE=\"$spirit_tests_src_dir\""

cd "$spirit_tests_src_dir"

#
# STEP 0:
#
# Get the source code:
#
if test ! -d $boost_root ; then
    mkdir -p $boost_root
    if test $? -ne 0 ; then
        echo "creation of $boost_root directory failed."
        exit 256
    fi
fi
if test $cvs_update = yes ; then
    echo fetching Boost:
    echo "/1 :pserver:anonymous@cvs.sourceforge.net:2401/cvsroot/boost A" >> $HOME/.cvspass
    cat $HOME/.cvspass | sort | uniq > $HOME/.cvspass
    cd `dirname $boost_root`
    if test -f boost/CVS/Root ; then
        cvs -z3 -d `cat $boost_dir/CVS/Root` co -d $boost_dir boost
    else
        cvs -z3 -d :pserver:anonymous@cvs.sourceforge.net:2401/cvsroot/boost co -d $boost_dir boost
    fi
fi

#
# STEP 1:
# rebuild bjam if required:
#
echo building bjam:
cd $boost_root/tools/build/jam_src && \
LOCATE_TARGET=bin sh ./build.sh
if test $? != 0 ; then
    echo "bjam build failed."
    exit 256
fi

#
# STEP 2:
# rebuild the regression test helper programs if required:
#
#echo building regression test helper programs:
cd $boost_root/tools/regression/build && \
$bjam -sTOOLS=$toolset -sBUILD=release run
if test $? != 0 ; then
    echo "helper program build failed."
    exit 256
fi

#
# STEP 3:
# copy the spirit tests into a new subdirectory of
# libs/boost

if ! test -d "$spirit_tests_dir"; then
    mkdir "$spirit_tests_dir"
    mkdir "$spirit_tests_dir/test"
    mkdir "$spirit_tests_dir/test/impl"
    mkdir "$spirit_tests_dir/test/status"
fi

cd "$spirit_tests_src_dir"

sed 's%/spirit/%/spirit_test/%' < Jamfile > "$spirit_tests_dir/test/Jamfile"

cd "$spirit_tests_dir/test"

#
# STEP 6:
# repeat steps 4 and 5 for each additional toolset:
#

for tool in $test_tools ; do

#
# STEP 4:
# run the regression tests:
#
echo running the $tool regression tests:
#echo $bjam -sTOOLS=$tool $spirit_hdr_include $spirit_src_include --dump-tests test 
$bjam -sTOOLS=$tool $spirit_hdr_include $spirit_src_include --dump-tests test 2>&1 | tee regress.log

#
# STEP 5:
# post process the results:
#

echo processing the regression test results for $tool:
cat regress.log | $process_jam_log "$boost_root"
if test $? != 0 ; then
    echo "Failed regression log post processing."
    exit 256
fi

done

#
# STEP 7:
# create the html table:
##
uname=`uname`
echo generating html tables:
$compiler_status $boost_root cs-$uname.html.tmp cs-$uname-links.html.tmp
if test $? != 0 ; then
    echo "Failed HTML result table generation."
    exit 256
fi

#
# STEP 8:
# patch the html tables
#

logo='s%\.\./c++boost.gif%http://spirit.sourceforge.net/theme/site_spirit.gif%'
test_type='s%<a href="compiler_status.html#test-type">Test Type</a>%Test Type%'
link_to_lib='s%<a href="\.\./\.\./\.\./[^>]*>%%g;s%</a></td><td>%</td><td>%g'

cat cs-$uname.html.tmp |\
    sed "$logo" |\
    sed "$link_to_lib" |\
    sed "$test_type" |\
    sed 's%Boost%Spirit%' |\
    sed 's%html\.tmp%html%g' > cs-$uname.html

cat cs-$uname-links.html.tmp |\
    sed "$logo" |\
    sed 's%Boost%Spirit%' |\
    sed 's%html\.tmp%html%g' > cs-$uname-links.html

mv regress.log cs-$uname.html.tmp cs-$uname.html cs-$uname-links.html.tmp cs-$uname-links.html status

#
# STEP 9
# copy the html tables
#

cd "$spirit_tests_src_dir"

if ! test -d status ; then
    mkdir status
fi

cp "$spirit_tests_dir/test/status/"/*.html status
cp "$spirit_tests_dir/test/status/regress.log" status

echo "done!"

