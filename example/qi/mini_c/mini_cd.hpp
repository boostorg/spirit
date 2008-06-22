/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_MINI_CD)
#define BOOST_SPIRIT_MINI_CD

#include "mini_c.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Our program grammar and compiler
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
program<Iterator>::program(std::vector<int>& code)
  : code(code)
  , statement_def(code, functions)
  , statement(statement_def, statement_def.statement_list)
  , add_function(function_adder(functions))
  , state_reset(function_state_reset(code, statement_def.vars, statement_def.nvars))
  , op(code)
{
    bool& has_return = statement_def.has_return;
    int& nvars = statement_def.nvars;
    boost::phoenix::function<var_adder>& add_var = statement_def.add_var;

    identifier %=
        raw[lexeme[alpha >> *(alnum | '_')]]
        ;

    function =
            (
                lit("void")                     [ref(has_return) = false]
            |   lit("int")                      [ref(has_return) = true]
            )
        >>  !functions                          // no duplicate functions!
        >>  identifier                          [_a = _1]
        >>  '('
        >  -(
                identifier                      [add_var(_1)]
                >> *(',' > identifier           [add_var(_1)])
            )
        >   ')'
        >   char_('{')                          [
                                                    _b = size(ref(code)),
                                                    add_function(
                                                        _a     // function name
                                                      , ref(nvars)      // arity
                                                      , size(ref(code)) // address
                                                    ),
                                                    op(op_stk_adj, 0)   // adjust this later
                                                ]
        >   statement
        >   char_('}')                          [state_reset(_b)]
        ;

    start =
        +function
        ;

    identifier.name("identifier");
    function.name("function");
    start.name("program");

    on_error<fail>(start, error_handler(_4, _3, _2));
}

#endif


