SUBDIRS = doc
EXTRA_PROGRAMS = quickdoc

all: $(EXTRA_PROGRAMS)

quickdoc_SOURCES = quickdoc.cpp quickdoc.hpp

INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

EXTRA_DIST = \
index.html \
quickdoc.txt

