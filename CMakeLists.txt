#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
# Add a library target to the build system
boost_library_project(
  spirit
  SRCDIRS     example/karma example/qi example/lex 
  TESTDIRS    test phoenix/example
  HEADERS     spirit.hpp spirit
  #  DOCDIRS 
  DESCRIPTION "A object oriented, recursive-descent parser and output generation library using the syntax of Parsing Expression Grammars (PEGs) directly in C++"
  MODULARIZED
  AUTHORS     "Joel de Guzman <joel -at- boost-consulting.com>"
              "Hartmut Kaiser <hartmut.kaiser -at- gmail.com>"
              "Dan Nuffer"
  #  MAINTAINERS 
)


