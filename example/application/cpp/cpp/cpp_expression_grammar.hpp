/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_EXPRESSION_GRAMMAR_HPP__099CD1A4_A6C0_44BE_8F24_0B00F5BE5674__INCLUDED_)
#define _CPP_EXPRESSION_GRAMMAR_HPP__099CD1A4_A6C0_44BE_8F24_0B00F5BE5674__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/core/assert.hpp>
#include <boost/spirit/attribute/closure.hpp>

#include <boost/spirit/phoenix/functions.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/statements.hpp>
#include <boost/spirit/phoenix/casts.hpp>

#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"

#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_expression_grammar_gen.hpp"   
#include "cpp/cpp_literal_grammar_gen.hpp"  

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulation of the grammar for evaluation of constant preprocessor
//  expressions
//
///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace closures {

///////////////////////////////////////////////////////////////////////////////
//
//  define the closure type used throughout the C++ expression grammar
//
//      Throughout this grammar all literal tokens are stored into double
//      variables and casted appropriately, where required.
//
//      Quoting the C++ standard:
//
//          2.9.1 Preprocessing number tokens lexically include all integral 
//                literal tokens (decimal, octal and hexadecimal integer 
//                literal tokens) and all floating literal tokens.
//
//          2.9.2 A preprocessing number does not have a type or a value; it 
//                acquires both after a successful conversion (as part of 
//                translation phase 7, 2.1) to an integral literal token or a 
//                floating literal token.
//
///////////////////////////////////////////////////////////////////////////////
    struct cpp_expr_closure 
    :   boost::spirit::closure<cpp_expr_closure, double> 
    {
        member1 val;
    };

}   // namespace closures

namespace impl {

///////////////////////////////////////////////////////////////////////////////
//
//  convert the given tokenvalue (integer literal) to a unsigned long
//
///////////////////////////////////////////////////////////////////////////////
    struct convert_intlit {

        template <typename ArgT>
        struct result { typedef unsigned long type; };

        template <typename TokenT>
        unsigned long operator()(TokenT const &token) const
        { 
            return intlit_grammar_gen<TokenT>::evaluate(token);
        }
    };
    phoenix::function<convert_intlit> const as_intlit;

///////////////////////////////////////////////////////////////////////////////
//
//  convert the given tokenvalue (character literal) to a unsigned int
//
///////////////////////////////////////////////////////////////////////////////
    struct convert_chlit {

        template <typename ArgT>
        struct result { typedef unsigned int type; };

        template <typename TokenT>
        unsigned int operator()(TokenT const &token) const
        { 
            return chlit_grammar_gen<TokenT>::evaluate(token);
        }
    };
    phoenix::function<convert_chlit> const as_chlit;

///////////////////////////////////////////////////////////////////////////////
//
//  convert the given tokenvalue (floating literal) to a double
//
///////////////////////////////////////////////////////////////////////////////
    struct convert_floatlit {

        template <typename ArgT>
        struct result { typedef double type; };

        template <typename TokenT>
        double operator()(TokenT const &token) const
        { 
            return floatlit_grammar_gen<TokenT>::evaluate(token);
        }
    };
    phoenix::function<convert_floatlit> const as_floatlit;

}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CPP_EXPR_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CPP_EXPR_GRAMMAR) \
    /**/

struct expression_grammar :
    public boost::spirit::grammar<
        expression_grammar, 
        closures::cpp_expr_closure::context_t
    >
{
    expression_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "expression_grammar", 
            TRACE_CPP_EXPR_GRAMMAR);
    }
    
    template <typename ScannerT>
    struct definition
    {
        typedef closures::cpp_expr_closure closure_t;
        typedef boost::spirit::rule<ScannerT, closure_t::context_t> rule_t;
        typedef boost::spirit::rule<ScannerT> simple_rule_t;

        simple_rule_t pp_expression;
        rule_t const_exp;
        rule_t logical_or_exp, logical_and_exp;
        rule_t inclusive_or_exp, exclusive_or_exp, and_exp;
        rule_t cmp_equality, cmp_relational;
        rule_t shift_exp;
        rule_t add_exp, multiply_exp;
        rule_t unary_exp, primary_exp, constant;

        boost::spirit::subrule<0, closure_t::context_t> const_exp_subrule;
        boost::spirit::subrule<1, closure_t::context_t> shift_exp_clos;

        definition(expression_grammar const &self)
        {
            using namespace boost::spirit;
            using namespace phoenix;
            using namespace cpplexer;

            pp_expression
                =   const_exp[self.val = arg1]
                ;
                
            const_exp
                =   logical_or_exp[const_exp.val = arg1]
                    >> !(const_exp_subrule =
                            ch_p(T_QUESTION_MARK)
                            >>  logical_or_exp
                                [
                                    if_(const_exp.val)
                                    [
                                        const_exp_subrule.val = arg1
                                    ]
                                ] 
                            >>  ch_p(T_COLON)
                            >>  logical_or_exp
                                [
                                    if_(!const_exp.val)
                                    [
                                        const_exp_subrule.val = arg1
                                    ]
                                ]
                        )[const_exp.val = arg1]
                ;

            logical_or_exp 
                =   logical_and_exp[logical_or_exp.val = arg1]
                    >> *(   ch_p(T_OROR)
                            >>  logical_and_exp
                                [
                                    logical_or_exp.val = 
                                        logical_or_exp.val || arg1
                                ]
                        )
                ;

            logical_and_exp
                =   inclusive_or_exp[logical_and_exp.val = arg1]
                    >> *(   ch_p(T_ANDAND)
                            >>  inclusive_or_exp
                                [
                                    logical_and_exp.val = 
                                        logical_and_exp.val && arg1
                                ]
                        )
                ;

            inclusive_or_exp
                =   exclusive_or_exp[inclusive_or_exp.val = arg1]
                    >> *(   ch_p(T_OR)
                            >>  exclusive_or_exp
                                [
                                    inclusive_or_exp.val = 
                                        static_cast_<int>(inclusive_or_exp.val) 
                                      | static_cast_<int>(arg1)
                                ]
                        )
                ;

            exclusive_or_exp
                =   and_exp[exclusive_or_exp.val = arg1]
                    >> *(   ch_p(T_XOR)
                            >>  and_exp
                                [
                                    exclusive_or_exp.val = 
                                        static_cast_<int>(exclusive_or_exp.val)
                                      ^ static_cast_<int>(arg1)
                                ]
                        )
                ;

            and_exp
                =   cmp_equality[and_exp.val = arg1]
                    >> *(   ch_p(T_AND)
                            >>  cmp_equality
                                [
                                    and_exp.val = 
                                        static_cast_<int>(and_exp.val)
                                      & static_cast_<int>(arg1)
                                ]
                        )
                ;

            cmp_equality
                =   cmp_relational[cmp_equality.val = arg1]
                    >> *(   ch_p(T_EQUAL)
                            >>  cmp_relational
                                [
                                    cmp_equality.val = 
                                        cmp_equality.val == arg1
                                ]
                        |   ch_p(T_NOTEQUAL)
                            >>  cmp_relational
                                [
                                    cmp_equality.val = 
                                        cmp_equality.val != arg1
                                ]
                        )
                ;

            cmp_relational
                =   shift_exp[cmp_relational.val = arg1]
                    >> *(   ch_p(T_LESSEQUAL)
                            >>  shift_exp
                                [
                                    cmp_relational.val = 
                                        cmp_relational.val <= arg1
                                ]
                        |   ch_p(T_GREATEREQUAL)
                            >>  shift_exp
                                [
                                    cmp_relational.val = 
                                        cmp_relational.val >= arg1
                                ]
                        |   ch_p(T_LESS)
                            >>  shift_exp
                                [
                                    cmp_relational.val = 
                                        cmp_relational.val < arg1
                                ]
                        |   ch_p(T_GREATER)
                            >>  shift_exp
                                [
                                    cmp_relational.val = 
                                        cmp_relational.val > arg1
                                ]
                        )
                ;

            shift_exp
                =   add_exp[shift_exp.val = arg1]
                    >> *(shift_exp_clos 
                            =   ch_p(T_SHIFTLEFT)
                                >>  add_exp
                                    [
                                        shift_exp_clos.val = arg1,
                                        if_(shift_exp_clos.val < -64)
                                        [
                                            shift_exp_clos.val = -64
                                        ],
                                        if_(shift_exp_clos.val > 64)
                                        [
                                            shift_exp_clos.val = 64
                                        ],
                                        shift_exp.val = 
                                            static_cast_<int>(shift_exp.val)
                                         << static_cast_<int>(shift_exp_clos.val)
                                    ]
                            |   ch_p(T_SHIFTRIGHT)
                                >>  add_exp
                                    [
                                        shift_exp_clos.val = arg1,
                                        if_(shift_exp_clos.val < -64)
                                        [
                                            shift_exp_clos.val = -64
                                        ],
                                        if_(shift_exp_clos.val > 64)
                                        [
                                            shift_exp_clos.val = 64
                                        ],
                                        shift_exp.val =
                                            static_cast_<int>(shift_exp.val) 
                                         >> static_cast_<int>(shift_exp_clos.val)
                                    ]
                        )
                ;

            add_exp
                =   multiply_exp[add_exp.val = arg1]
                    >> *(   ch_p(T_PLUS)
                            >>  multiply_exp
                                [
                                    add_exp.val += arg1
                                ]
                        |   ch_p(T_MINUS)
                            >>  multiply_exp
                                [
                                    add_exp.val -= arg1
                                ]
                        )
                ;

            multiply_exp
                =   unary_exp[multiply_exp.val = arg1]
                    >> *(   ch_p(T_STAR)
                            >>  unary_exp
                                [
                                    multiply_exp.val *= arg1
                                ]
                        |   ch_p(T_DIVIDE)
                            >>  unary_exp
                                [
                                    multiply_exp.val /= arg1
                                ]
                        |   ch_p(T_PERCENT)
                            >>  unary_exp
                                [
                                    multiply_exp.val = 
                                        static_cast_<int>(multiply_exp.val)
                                      % static_cast_<int>(arg1)
                                ]
                        )
                ;

            unary_exp
                =   primary_exp[unary_exp.val = arg1]
                |   ch_p(T_PLUS) >> unary_exp[unary_exp.val = arg1]
                |   ch_p(T_MINUS) >> unary_exp[unary_exp.val = -arg1]
                |   ch_p(T_COMPL) >> unary_exp
                    [
                        unary_exp.val = ~static_cast_<int>(arg1)
                    ]
                |   ch_p(T_NOT) >> unary_exp
                    [
                        unary_exp.val = !static_cast_<int>(arg1)
                    ]
                ;

            primary_exp
                =   constant[primary_exp.val = arg1]
                |   ch_p(T_LEFTPAREN) 
                    >> const_exp[primary_exp.val = arg1]
                    >> ch_p(T_RIGHTPAREN)
                ;

            constant
                =   ch_p(T_INTLIT) 
                    [
                        constant.val = impl::as_intlit(arg1)
                    ]
                |   ch_p(T_CHARLIT) 
                    [
                        constant.val = impl::as_chlit(arg1)
                    ]
                |   ch_p(T_FLOATLIT) 
                    [
                        constant.val = impl::as_floatlit(arg1)
                    ]
                ;
              
            BOOST_SPIRIT_DEBUG_TRACE_RULE(pp_expression, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(const_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(logical_or_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(logical_and_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(inclusive_or_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(exclusive_or_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(and_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(cmp_equality, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(cmp_relational, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(shift_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(add_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(multiply_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(unary_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(primary_exp, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(constant, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(const_exp_subrule, TRACE_CPP_EXPR_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(shift_exp_clos, TRACE_CPP_EXPR_GRAMMAR);
        }

    // start rule of this grammar
        simple_rule_t const& start() const
        { return pp_expression; }
    };
};

///////////////////////////////////////////////////////////////////////////////
#undef TRACE_CPP_EXPR_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following function is defined here, to allow the separation of 
//  the compilation of the expression_grammar from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_EXPRGRAMMAR_GEN_INLINE
#else
#define CPP_EXPRGRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_EXPRGRAMMAR_GEN_INLINE 
bool 
expression_grammar_gen<TokenT>::evaluate(
    typename std::list<TokenT>::const_iterator const &first, 
    typename std::list<TokenT>::const_iterator const &last, 
    TokenT const &act_token)
{
    using namespace boost::spirit;
    using namespace cpplexer;
    
    typedef typename std::list<TokenT>::const_iterator iterator_t;
    
static expression_grammar g;  // expression grammar
double result = 0;                      // expression result
parse_info<iterator_t> hit =  parse (first, last, g[assign(result)], 
    ch_p(T_SPACE) | ch_p(T_CCOMMENT) | ch_p(T_CPPCOMMENT));

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            "", act_token);
    }
    return bool(result != 0.0);
}

#undef CPP_EXPRGRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_EXPRESSION_GRAMMAR_HPP__099CD1A4_A6C0_44BE_8F24_0B00F5BE5674__INCLUDED_)
