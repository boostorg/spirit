check_PROGRAMS = \
timestamp_compile_test \
rfcskipper_compile_test \
rfcdate_compile_test \
skipper_test \
rfcdate_test

timestamp_compile_test_SOURCES = timestamp-compile-test.cpp
rfcskipper_compile_test_SOURCES = rfcskipper-compile-test.cpp
rfcdate_compile_test_SOURCES = rfcdate-compile-test.cpp
skipper_test_SOURCES = skipper-test.cpp
rfcdate_test_SOURCES = rfcdate-test.cpp

EXTRA_DIST = \
rfc-skipper.hpp \
rfcdate-parser.hpp \
timestamp.hpp \
tutorial.html \
runtest.sh

INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

TESTS = runtest.sh

