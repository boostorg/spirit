check_PROGRAMS = \
match_tests \
scanner_tests \
primitives_tests numerics_tests \
fundamental_tests parser_traits_tests traverse_tests \
composite_tests epsilon_tests actions_tests operators_tests directives_tests \
rule_tests grammar_tests subrule_tests owi_st_tests owi_mt_tests \
closure_tests parametric_tests \
exception_tests \
if_tests for_tests while_tests \
chset_tests loops_tests symbols_tests escape_char_parser_tests scoped_lock_tests\
multi_pass_tests fixed_size_queue_tests

# Spirit.Core.Kernel
match_tests_SOURCES = match_tests.cpp

# Spirit.Core.Scanner
scanner_tests_SOURCES = scanner_tests.cpp

# Spirit.Core.Primitive
primitives_tests_SOURCES = primitives_tests.cpp
numerics_tests_SOURCES = numerics_tests.cpp

# Spirit.Core.Meta
fundamental_tests_SOURCES = fundamental_tests.cpp
parser_traits_tests_SOURCES = parser_traits_tests.cpp
traverse_tests_SOURCES = traverse_tests.cpp

# Spirit.Core.Composite
composite_tests_SOURCES = composite_tests.cpp
epsilon_tests_SOURCES = epsilon_tests.cpp
actions_tests_SOURCES = actions_tests.cpp
operators_tests_SOURCES = operators_tests.cpp
directives_tests_SOURCES = directives_tests.cpp

# Spirit.Core.Non_Terminal
rule_tests_SOURCES = rule_tests.cpp
grammar_tests_SOURCES = grammar_tests.cpp
subrule_tests_SOURCES = subrule_tests.cpp
owi_st_tests_SOURCES = owi_st_tests.cpp
owi_mt_tests_SOURCES = owi_mt_tests.cpp

# Spirit.Attribute
closure_tests_SOURCES = closure_tests.cpp
parametric_tests_SOURCES = parametric_tests.cpp

# Spirit.Error_Handling
exception_tests_SOURCES = exception_tests.cpp

# Spirit.Dynamic
if_tests_SOURCES = if_tests.cpp
for_tests_SOURCES = for_tests.cpp
while_tests_SOURCES = while_tests.cpp

# Spirit.Utility
chset_tests_SOURCES = chset_tests.cpp
loops_tests_SOURCES = loops_tests.cpp
symbols_tests_SOURCES = symbols_tests.cpp
escape_char_parser_tests_SOURCES = escape_char_parser_tests.cpp
scoped_lock_tests_SOURCES = scoped_lock_tests.cpp

# Spirit.Iterator
multi_pass_tests_SOURCES = multi_pass_tests.cpp
fixed_size_queue_tests_SOURCES = fixed_size_queue_tests.cpp


EXTRA_DIST = \
runtests.sh \
Jamfile

INCLUDES = -I$(top_srcdir)

TESTS = runtests.sh

