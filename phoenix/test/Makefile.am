check_PROGRAMS = binary_tests binders_tests functors_tests iostream_tests \
  mixed_binary_tests more_expressions_tests primitives_tests statements_tests \
  stl_tests tuples_tests unary_tests

binary_tests_SOURCES = binary_tests.cpp
binders_tests_SOURCES = binders_tests.cpp
functors_tests_SOURCES = functors_tests.cpp
iostream_tests_SOURCES = iostream_tests.cpp
mixed_binary_tests_SOURCES = mixed_binary_tests.cpp
more_expressions_tests_SOURCES = more_expressions_tests.cpp
primitives_tests_SOURCES = primitives_tests.cpp
statements_tests_SOURCES = statements_tests.cpp
stl_tests_SOURCES = stl_tests.cpp
tuples_tests_SOURCES = tuples_tests.cpp
unary_tests_SOURCES = unary_tests.cpp

EXTRA_DIST = \
runtest.sh \
Jamfile

INCLUDES = -I$(top_srcdir)

TESTS = runtest.sh
