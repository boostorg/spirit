EXTRA_PROGRAMS = calc_plain full_calc primitive_calc subrule_calc \
  calc_with_variables phoenix_calc vmachine_calc rpn_calc ast_calc \
  parse_tree_calc1 ast_calc2 phoenix_subrule_calc

parse_tree_calc1_SOURCES = parse_tree_calc1.cpp
ast_calc2_SOURCES = ast_calc2.cpp
ast_calc_SOURCES = ast_calc.cpp tree_calc_grammar.hpp
calc_plain_SOURCES = calc_plain.cpp
full_calc_SOURCES = full_calc.cpp
primitive_calc_SOURCES = primitive_calc.cpp
subrule_calc_SOURCES = subrule_calc.cpp
calc_with_variables_SOURCES = calc_with_variables.cpp
phoenix_calc_SOURCES = phoenix_calc.cpp
vmachine_calc_SOURCES = vmachine_calc.cpp
rpn_calc_SOURCES = rpn_calc.cpp
phoenix_subrule_calc_SOURCES = phoenix_subrule_calc.cpp
#grouping_calc_SOURCES = grouping_calc.cpp
EXTRA_DIST = grouping_calc.cpp

all: $(EXTRA_PROGRAMS)


INCLUDES = -I$(top_srcdir)

CLEANFILES = $(EXTRA_PROGRAMS)

