SUBDIRS = testfiles
EXTRA_PROGRAMS = lexer lextest 

#callback

all: $(EXTRA_PROGRAMS)

lexer_SOURCES = \
lexer.cpp \
lexer.hpp

lextest_SOURCES = lextest.cpp
# callback requires boost::function to compile
#callback_SOURCES = callback.cpp

EXTRA_DIST = \
README \
callback.cpp

INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

