boost_additional_test_dependencies(spirit BOOST_DEPENDS test variant function_types intrusive lambda assign iostreams math random dynamic_bitset)


boost_test_run(qi_actions qi/actions.cpp BOOST_LIB spirit)
boost_test_run(qi_char qi/char.cpp)
boost_test_run(qi_char_class qi/char_class.cpp)
boost_test_run(qi_lit qi/lit.cpp)
boost_test_run(qi_int qi/int.cpp)
boost_test_run(qi_uint qi/uint.cpp)
boost_test_run(qi_real qi/real.cpp)
boost_test_run(qi_eps qi/eps.cpp)
boost_test_run(qi_none qi/none.cpp)
boost_test_run(qi_lazy qi/lazy.cpp)
boost_test_run(qi_tst qi/tst.cpp)
boost_test_run(qi_symbols qi/symbols.cpp)
boost_test_run(qi_range_run qi/range_run.cpp)
boost_test_run(qi_no_case qi/no_case.cpp)
boost_test_run(qi_lexeme qi/lexeme.cpp)
boost_test_run(qi_raw qi/raw.cpp)
boost_test_run(qi_sequence qi/sequence.cpp)
boost_test_run(qi_alternative qi/alternative.cpp)
boost_test_run(qi_sequential_or qi/sequential_or.cpp)
boost_test_run(qi_permutation qi/permutation.cpp)
boost_test_run(qi_difference qi/difference.cpp)
boost_test_run(qi_list qi/list.cpp)
boost_test_run(qi_optional qi/optional.cpp)
boost_test_run(qi_kleene qi/kleene.cpp)
boost_test_run(qi_plus qi/plus.cpp)
boost_test_run(qi_and_predicate qi/and_predicate.cpp)
boost_test_run(qi_not_predicate qi/not_predicate.cpp)
boost_test_run(qi_expect qi/expect.cpp)
boost_test_run(qi_rule qi/rule.cpp)
boost_test_run(qi_grammar qi/grammar.cpp)
boost_test_run(qi_functor qi/functor.cpp)
boost_test_run(qi_match_manip qi/match_manip.cpp)
boost_test_run(qi_binary qi/binary.cpp)
boost_test_run(qi_debug qi/debug.cpp)
boost_test_run(qi_end qi/end.cpp)

boost_test_compile_fail(qi_grammar_fail qi/grammar_fail.cpp)
boost_test_compile_fail(qi_rule_fail qi/rule_fail.cpp)

    # run Karma tests
boost_test_run(karma_actions karma/actions.cpp) #              : : : : karma_actions ]
boost_test_run(karma_alternative karma/alternative.cpp) #              : : : : karma_alternative ]
boost_test_run(karma_binary karma/binary.cpp) #                   : : : : karma_binary ]
boost_test_run(karma_case_handling karma/case_handling.cpp)
boost_test_run(karma_center_alignment karma/center_alignment.cpp)
boost_test_run(karma_char karma/char.cpp) #                     : : : : karma_char ]
boost_test_run(karma_delimieter karma/delimiter.cpp)
boost_test_run(karma_eol karma/eol.cpp) #                      : : : : karma_eol ]
boost_test_run(karma_eps karma/eps.cpp) #                      : : : : karma_eps ]
boost_test_run(karma_format_manip karma/format_manip.cpp)
boost_test_run(karma_functor karma/functor.cpp) #                  : : : : karma_functor ]
boost_test_run(karma_grammar karma/grammar.cpp) #                  : : : : karma_grammar ]
boost_test_run(karma_int_numerics karma/int_numerics.cpp)
boost_test_run(karma_kleene karma/kleene.cpp) #                   : : : : karma_kleene ]
boost_test_run(karma_lazy karma/lazy.cpp) #                     : : : : karma_lazy ]
boost_test_run(karma_left_alignment karma/left_alignment.cpp)
boost_test_run(karma_list karma/list.cpp) #                     : : : : karma_list ]
boost_test_run(karma_lit karma/lit.cpp) #                      : : : : karma_lit ]
boost_test_run(karma_none karma/none.cpp) #                     : : : : karma_none ]
boost_test_run(karma_optional karma/optional.cpp) #                 : : : : karma_optional ]
boost_test_run(karma_pattern karma/pattern.cpp)
boost_test_run(karma_real_numerics karma/real_numerics.cpp)
boost_test_run(karma_right_alignment karma/right_alignment.cpp)
boost_test_run(karma_sequence karma/sequence.cpp) #                 : : : : karma_sequence ]

boost_test_compile_fail(karma_grammar_fail karma/grammar_fail.cpp) #   : : karma_grammar_fail ]
boost_test_compile_fail(karma_rule_fail karma/rule_fail.cpp) #      : : karma_rule_fail ]

    # run support tests
boost_test_run(support_hold_any support/hold_any.cpp)
# the multi_pass tests are not completed yet
#    [ run support/multi_pass_compile.cpp    : : : : ]
#    [ run support/multi_pass.cpp            : : : : ]

    # run lexer tests
boost_test_run(lex_lexertl1 lex/lexertl1.cpp)
boost_test_run(lex_lexertl2 lex/lexertl2.cpp)
boost_test_run(lex_lexertl3 lex/lexertl3.cpp)
boost_test_run(lex_lexertl4 lex/lexertl4.cpp)
boost_test_run(lex_lexertl5 lex/lexertl5.cpp)
boost_test_run(lex_state_switcher_test lex/state_switcher_test.cpp)




