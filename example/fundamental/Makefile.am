SUBDIRS = calc position_iterator

EXTRA_PROGRAMS = comments complex_number dynamic_parser functor_parser \
  number_list sum matching_tags file_parser refactoring roman_numerals \
  list_parser polynomial rfc821 parameters

all: $(EXTRA_PROGRAMS)

comments_SOURCES = comments.cpp
complex_number_SOURCES = complex_number.cpp
dynamic_parser_SOURCES = dynamic_parser.cpp
functor_parser_SOURCES = functor_parser.cpp
number_list_SOURCES = number_list.cpp
sum_SOURCES = sum.cpp
matching_tags_SOURCES = matching_tags.cpp
file_parser_SOURCES = file_parser.cpp
refactoring_SOURCES = refactoring.cpp
roman_numerals_SOURCES = roman_numerals.cpp
list_parser_SOURCES = list_parser.cpp
polynomial_SOURCES = polynomial.cpp
rfc821_SOURCES = rfc821.cpp
parameters_SOURCES = parameters.cpp

EXTRA_DIST = \
boiler_plate.cpp \
regular_expression.cpp \
spirit_bind.cpp

INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

