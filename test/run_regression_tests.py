#! /usr/bin/env python
#==============================================================================
# Copyright (c) 2003 Martin Wille
# http://spirit.sourceforge.net/
#
#   Use, modification and distribution is subject to the Boost Software
#   License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt)
#===============================================================================

import os
import string
import sys
import shutil
import re

boost_root = os.environ.get('BOOST_ROOT')

if not boost_root:
    if os.name == "posix":
        boost_root = "/home/boost-head-spirit/boost"
    else:
        boost_root = "X:\\boost"

assert(os.path.isdir(boost_root))

test_tools = os.environ.get('TEST_TOOLS')

if not test_tools:
    if os.name == "posix":
        test_tools = "intel-7.1 gcc-2.95.3 gcc-2.95.3-stlport-4.5.3 gcc-3.1 gcc-3.2.2 gcc-3.2.3 gcc-3.3.1 gcc-3.4-cvs como-4.3.3beta8-libcomobeta29-posix"
    else:
        test_tools = "msvc"

build_tool = os.environ.get('BUILD_TOOL')

if not build_tool:
    build_tool = "gcc"

home = os.environ.get('HOME')
if home:
    boost_build_path=os.environ.get('BOOST_BUILD_PATH')
    if boost_build_path:
        boost_build_path += os.pathsep + os.path.join(home,".boost-build")
    else:
        boost_build_path = os.path.join(home,".boost-build")

if boost_build_path:
    os.environ['BOOST_BUILD_PATH'] = boost_build_path


#===============================================================================

def dirname(name):
    return os.path.dirname(name)

def makedirectory(name):
    if not os.path.isdir(name):
        print "creating " + name
        os.mkdir(name)

def readfile(name):
    f = file(name, "r")
    contents = f.read()
    f.close()
    return contents

def writefile(name, contents):
    print "writing file " + name
    f = file(name, "w")
    f.write(contents)
    f.close()


def replacefile(name, newcontents):
    if os.path.isfile(name):
        oldcontents = readfile(name)
        if oldcontents != newcontents:
            writefile(name, newcontents)
    else:
        writefile(name, newcontents)

def tee(fd):
    line = fd.readline()
    file = ""
    while line:
        file += line
        sys.stdout.write(line)
        line = fd.readline()
    return file

def uname():
    if os.name == "posix":
        return os.uname()[0]
    else:
        return "Windows"

#===============================================================================

srcdir = os.getcwd()
bjamdir = os.path.join(boost_root, "tools/build/jam_src")
bjam = os.path.join(bjamdir, "spirit_bin/bjam")
buildscriptname = "build"

if os.name == "posix":
    buildscriptname = "build.sh"
else:
    buildscriptname = "build.bat"

bjambuild = os.path.join(bjamdir, buildscriptname)

tools_path = os.path.join(boost_root, "tools/regression/build")
process_jam_log = os.path.join(tools_path, "run/process_jam_log");
compiler_status = os.path.join(tools_path, "run/compiler_status");

testdir = os.path.join(boost_root, "libs/spirit_test")

spirit_header_include = os.path.abspath(dirname(dirname(dirname(srcdir))))
spirit_header_include = " -sSPIRIT_HEADER_INCLUDE=" + spirit_header_include
spirit_src_include = " -sSPIRIT_SRC_INCLUDE=" + os.path.abspath(srcdir)

#===============================================================================
print "preparing jamfile"

jamfile = readfile("Jamfile")
jamfile = string.replace(jamfile, "/spirit/", "/spirit_test/")

#===============================================================================
print "preparing test directory"

makedirectory(testdir)
makedirectory(os.path.join(testdir,"test"))
makedirectory(os.path.join(testdir,"test/impl"))
makedirectory(os.path.join(testdir,"test/status"))

replacefile(os.path.join(testdir,"test/Jamfile"), jamfile)

#===============================================================================
print "rebuilding jam"

os.chdir(bjamdir)
os.environ['LOCATE_TARGET'] = "spirit_bin"
os.spawnl(os.P_WAIT, os.path.abspath(bjambuild), bjambuild, build_tool)
os.environ['LOCATE_TARGET'] = "bin"
os.chdir(srcdir)

#===============================================================================
print "rebuilding test tools"

os.chdir(os.path.join(boost_root,"tools/regression/build"))
os.spawnl(os.P_WAIT, os.path.abspath(bjam), "bjam"
    "-sTOOLS=" + build_tool,
    "-sBUILD=release",
    "run")
os.chdir(srcdir)

#===============================================================================
print "running regression tests"

os.chdir(os.path.join(testdir,"test"))

for tool in string.split(test_tools):
    print "running regression tests for " + tool

    cmd = os.path.abspath(bjam) + " -sTOOLS=" + tool
    cmd += " " + spirit_header_include
    cmd += " " + spirit_src_include
    cmd += " --dump-tests test"

    (in_fd, out_fd) = os.popen4(cmd)

    regression_result = tee(out_fd)

    out_fd.close()
    in_fd.close()
    sys.stdout.flush()

    print "processing jam log for " + tool
    cmd = os.path.abspath(process_jam_log)
    cmd += " " + os.path.abspath(boost_root)

    (in_fd, out_fd) = os.popen4(cmd)
    in_fd.write(regression_result)
    in_fd.close()
    out_fd.close();

os.chdir(srcdir)

#===============================================================================
print "building tables"

os.chdir(os.path.join(testdir,"test"))

html_name = "cs-" + uname() + ".html"
html_links_name = "cs-" + uname() + "-links.html"

cmd = os.path.abspath(compiler_status)
cmd += " " + os.path.abspath(boost_root)
cmd += " " + html_name +  ".tmp"
cmd += " " + html_links_name +  ".tmp"

os.spawnl(os.P_WAIT, os.path.abspath(compiler_status), "compiler_status",
    os.path.abspath(boost_root),
    os.path.abspath("status/cs-" + uname() + ".html.tmp"),
    os.path.abspath("status/cs-" + uname() + "-links.html.tmp"))


os.chdir(srcdir)

def logo(x):
    return string.replace(x,
        "../c++boost.gif",
        "http://spirit.sourceforge.net/theme/site_spirit.gif")

def test_type(x):
    return string.replace(x,
        '<a href="compiler_status.html#test-type">Test Type</a>',
        'Test Type')

def re_substitute(pattern, s, x):
    expr = re.compile(pattern)
    m = expr.search(x)
    while m:
        x = x[:m.start()] + s + x[m.end():]
        m = expr.search(x)
    return x

def boost_to_spirit(x):
    return string.replace(x, "Boost", "Spirit")

def link_to_lib(x):
    x = re_substitute("<a href=\"\.\./\.\./\.\./[^>]*>","", x)
    return re_substitute("</a></td><td>","</td><td>", x)

def remove_tmp(x):
    return re_substitute("html\.tmp","html", x)


makedirectory("status")

x = readfile(os.path.join(testdir, "test/status/" + html_name + ".tmp"))
x = remove_tmp(link_to_lib(boost_to_spirit(test_type(logo(x)))))
writefile("status/"+html_name, x)

x = readfile(os.path.join(testdir, "test/status/" + html_links_name + ".tmp"))
x = remove_tmp(boost_to_spirit(logo(x)))
writefile("status/" + html_links_name, x)

print "done!"
