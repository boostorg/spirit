
# CC = C:/cygwin/usr/local/bin/g++ -O2 -ftemplate-depth-500
# LD = C:/cygwin/usr/local/bin/g++ -O2 -ftemplate-depth-500

CC = g++ -O2 -ftemplate-depth-500
LD = g++ -O2 -ftemplate-depth-500

default: cpp_lexer_test.exe

cpp_lexer_test.exe: cpp_lexer_test.o cpp_lexer_token.o cpp_lexer.o
	$(LD) -o $@ $^

cpp_lexer.o			: cpp_lexer.cpp cpp_lexer.hpp
cpp_lexer_token.o	: cpp_lexer_token.cpp cpp_lexer_token.hpp
cpp_lexer_test.o	: cpp_lexer_test.cpp cpp_lexer.hpp cpp_lexer_token.hpp


%.o: %.cpp
	$(CC) -o $@ -c $<
