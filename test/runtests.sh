#! /bin/sh

VERBOSE_RUNTESTS=1
export VERBOSE_RUNTESTS

# Spirit.Core.Kernel
./match_tests || exit 1

# Spirit.Core.Scanner
./scanner_tests || exit 1

# Spirit.Core.Primitive
./primitives_tests || exit 1
./numerics_tests || exit 1

# Spirit.Core.Meta
./fundamental_tests || exit 1
./parser_traits_tests || exit 1
./traverse_tests || exit 1

# Spirit.Core.Composite
./composite_tests || exit 1
./epsilon_tests || exit 1
./operators_tests || exit 1
./directives_tests || exit 1

# Spirit.Core.Non_Terminal
./rule_tests || exit 1
./owi_st_tests || exit 1
./grammar_tests || exit 1
./subrule_tests || exit 1
./owi_mt_tests || exit 1
./grammar_mt_tests || exit 1
./parser_context_test || exit 1

# Spirit.Attribute
./closure_tests || exit 1
./bug_000008 || exit 1
./parametric_tests || exit 1

# Spirit.Error_Handling
./exception_tests || exit 1

# Spirit.Tree
./ast_calc_tests || exit 1

# Spirit.Dynamic
# if_p_as_parser_tests is a compile time test
# while_p_as_parser_tests is a compile time test
# for_p_as_parser_tests is a compile time test
./if_tests || exit 1
./for_tests || exit 1
./while_tests || exit 1
./lazy_tests || exit 1
./distinct_tests || exit 1

# Spirit.Utility.Parsers
./chset_tests || exit 1
./confix_tests || exit 1
./loops_tests || exit 1
./symbols_tests || exit 1
./escape_char_parser_tests || exit 1

# Spirit.Utility.Actors
./actor/action_tests || exit 1

# Spirit.Utility.Support
./scoped_lock_tests || exit 1

# Spirit.Iterator
./fixed_size_queue_tests || exit 1
./file_iterator_tests || exit 1
./multi_pass_tests || exit 1
./sf_bug_720917 || exit 1
./position_iterator_tests || exit 1

# Spirit.Small_Bug_Fixes
./bug_fixes || exit 1
