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

#if !defined(_CPP_FLOATLIT_GRAMMAR_HPP__572D0597_9719_485A_AE71_6846FC18CFF1__INCLUDED_)
#define _CPP_FLOATLIT_GRAMMAR_HPP__572D0597_9719_485A_AE71_6846FC18CFF1__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute/closure.hpp>

#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/statements.hpp>

#include "cpp/cpp_exceptions.hpp"   
#include "cpp/cpp_literal_grammar_gen.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Reusable grammar for parsing of C++ style floting point literals
//
///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace closures {

    struct floatlit_closure 
    :   boost::spirit::closure<floatlit_closure, double> 
    {
        member1 val;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_FLOATLIT_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_FLOATLIT_GRAMMAR) \
    /**/

struct floatlit_grammar :
    boost::spirit::grammar<floatlit_grammar, 
        closures::floatlit_closure::context_t>
{
    floatlit_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "floatlit_grammar", 
            TRACE_FLOATLIT_GRAMMAR);
    }
    
    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t float_lit;

        definition(floatlit_grammar const &self)
        {
            using namespace boost::spirit;
            using namespace phoenix;
            
            float_lit
                =   real_p[self.val = arg1]
                    >> !as_lower_d
                        [
                            (ch_p('f') || ch_p('l')) | (ch_p('l') || ch_p('f'))
                        ]
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(float_lit, TRACE_FLOATLIT_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return float_lit; }
    };
};

#undef TRACE_FLOATLIT_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following function is defined here, to allow the separation of 
//  the compilation of the intlit_grammap from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_FLOATLITGRAMMAR_GEN_INLINE
#else
#define CPP_FLOATLITGRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_FLOATLITGRAMMAR_GEN_INLINE 
double
floatlit_grammar_gen<TokenT>::evaluate(TokenT const &token)
{
    using namespace boost::spirit;
    
static floatlit_grammar g;
double result = 0;
typename TokenT::string_t const &token_val = token.get_value();
parse_info<typename TokenT::string_t::const_iterator> hit =
    parse(token_val.begin(), token_val.end(), g[assign(result)]);

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            token_val, token);
    }
    return result;
}

#undef CPP_FLOATLITGRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_FLOATLIT_GRAMMAR_HPP__572D0597_9719_485A_AE71_6846FC18CFF1__INCLUDED_)
