check_PROGRAMS = test_lexer

test_lexer_SOURCES = \
aq.c \
aq.h \
cpp.re \
cpp.c \
scanner.h \
test_lexer.c \
../tokennames.cpp \

BUILT_SOURCES = cpp.c

$(srcdir)/cpp.c: $(srcdir)/cpp.re
	re2c -s $< > $@

TESTS = run_tests.sh

EXTRA_DIST = \
lextest.in \
lextest.output \
test_lexer.in \
test_lexer.output
