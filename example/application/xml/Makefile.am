EXTRA_PROGRAMS = xml simplexml ast_xml

all: $(EXTRA_PROGRAMS)

xml_SOURCES = xml.cpp xml_grammar.hpp
simplexml_SOURCES = simplexml.cpp
ast_xml_SOURCES = ast_xml.cpp

INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

EXTRA_DIST = \
simplexml.txt
