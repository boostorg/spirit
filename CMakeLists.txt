#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
#----------------------------------------------------------------------------
# This file was automatically generated from the original CMakeLists.txt file
# Add a variable to hold the headers for the library
set (lib_headers
    spirit.hpp
    spirit
)

# Add a library target to the build system
boost_library_project(
  spirit
  #  SRCDIRS 
  TESTDIRS  test
  HEADERS ${lib_headers}
  #  DOCDIRS 
  DESCRIPTION  "LL parser framework represents parsers directly as EBNF grammars in inlined C++."
  MODULARIZED
  AUTHORS  "Joel de Guzman <joel -at- boost-consulting.com>"
           "Hartmut Kaiser <hartmut.kaiser -at- gmail.com>"
           "Dan Nuffer"
  #  MAINTAINERS 
)


