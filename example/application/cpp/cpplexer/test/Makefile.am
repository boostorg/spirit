check_PROGRAMS = test_re2c_lexer test_slex_lexer

test_re2c_lexer_SOURCES = \
test_re2c_lexer.cpp \
test_re2c_lexer.hpp \
instantiate_re2c_lexer.cpp \
../re2clex/aq.cpp \
../re2clex/aq.h \
../re2clex/cpp.re \
../re2clex/cpp.re.cpp \
../re2clex/scanner.h \
../re2clex/cpp_re2c_lexer.hpp \
../re2clex/cpp_re2c_token.hpp \
../cpp_token_ids.hpp \
../cpp_lex_interface.hpp \
../cpp_lex_iterator.hpp

test_slex_lexer_SOURCES = \
test_slex_lexer.cpp \
test_slex_lexer.hpp \
instantiate_slex_lexer.cpp \ 
../slex/cpp_slex_lexer.hpp \
../slex/cpp_slex_token.hpp \
../cpp_token_ids.hpp \
../cpp_lex_interface.hpp \
../cpp_lex_iterator.hpp

TESTS = run_tests.sh

EXTRA_DIST = \
lextest.in \
lextest.re2c.output \
lextest.slex.output
