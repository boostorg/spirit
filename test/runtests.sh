#! /bin/sh

VERBOSE_RUNTESTS=1
export VERBOSE_RUNTESTS

# Spirit.Core.Kernel
./match_tests || exit 1
./bug_000002 || exit 1

# Spirit.Core.Scanner
./scanner_tests || exit 1

# Spirit.Core.Primitive
./primitives_tests || exit 1
./numerics_tests || exit 1

# Spirit.Core.Meta
./fundamental_tests || exit 1
./parser_traits_tests || exit 1
./traverse_tests || exit 1
# bug_000005 is a compile time test

# Spirit.Core.Composite
./composite_tests || exit 1
./epsilon_tests || exit 1
./actions_tests || exit 1
./operators_tests || exit 1
./directives_tests || exit 1
# sf_bug_718903 is a compile time test

# Spirit.Core.Non_Terminal
./rule_tests || exit 1
./owi_st_tests || exit 1
./grammar_tests || exit 1
./subrule_tests || exit 1
./owi_mt_tests || exit 1
./grammar_mt_tests || exit 1

# Spirit.Attribute
./closure_tests || exit 1
./parametric_tests || exit 1
./bug_000008 || exit 1

# Spirit.Error_Handling
./exception_tests || exit 1

# Spirit.Tree
./bug_000001 || exit 1
./ast_calc_tests || exit 1

# Spirit.Dynamic
./if_tests || exit 1
./for_tests || exit 1
./while_tests || exit 1
# if_p_as_parser_tests is a compile time test
# while_p_as_parser_tests is a compile time test
# for_p_as_parser_tests is a compile time test

# Spirit.Utility
./chset_tests || exit 1
./loops_tests || exit 1
./symbols_tests || exit 1
./escape_char_parser_tests || exit 1
./scoped_lock_tests || exit 1
./sf_bug_719322 || exit 1
# bug_000003 is a compile time test
./bug_000004 || exit 1
./bug_000006 || exit 1
./bug_000007 || exit 1

# Spirit.Iterator
./file_iterator_tests || exit 1
./fixed_size_queue_tests || exit 1
./multi_pass_tests || exit 1
./position_iterator_tests || exit 1
./sf_bug_720917 || exit 1
