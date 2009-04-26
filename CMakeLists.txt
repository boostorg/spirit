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
  DESCRIPTION  "A object oriented, recursive-descent parser and output generation library using the yyntax of Parsing Expression Grammars (PEG’s) directly in C++"
  MODULARIZED
  AUTHORS  "Joel de Guzman <joel -at- boost-consulting.com>"
           "Hartmut Kaiser <hartmut.kaiser -at- gmail.com>"
           "Dan Nuffer"
  #  MAINTAINERS 
)


