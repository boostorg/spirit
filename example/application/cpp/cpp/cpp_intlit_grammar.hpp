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

#if !defined(_CPP_INTLIT_GRAMMAR_HPP__2E1E70B1_F15C_4132_8554_10A231B0D91C__INCLUDED_)
#define _CPP_INTLIT_GRAMMAR_HPP__2E1E70B1_F15C_4132_8554_10A231B0D91C__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute/closure.hpp>

#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/statements.hpp>

#include "cpp/cpp_exceptions.hpp"   
#include "cpp/cpp_literal_grammar_gen.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Reusable grammar for parsing of C++ style integer literals
//
///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace closures {

    struct intlit_closure 
    :   boost::spirit::closure<intlit_closure, unsigned long> 
    {
        member1 val;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_INTLIT_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_INTLIT_GRAMMAR) \
    /**/

struct intlit_grammar :
    boost::spirit::grammar<intlit_grammar, closures::intlit_closure::context_t>
{
    intlit_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "intlit_grammar", 
            TRACE_INTLIT_GRAMMAR);
    }
    
    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t int_lit;
        boost::spirit::subrule<0> sub_int_lit;
        boost::spirit::subrule<1> oct_lit;
        boost::spirit::subrule<2> hex_lit;
        boost::spirit::subrule<3> dec_lit;

        definition(intlit_grammar const &self)
        {
            using namespace boost::spirit;
            using namespace phoenix;
            
            int_lit = (
                    sub_int_lit = 
                        (    ch_p('0')[self.val = 0] >> (hex_lit | oct_lit)
                        |   dec_lit
                        )
                        >> !as_lower_d[(ch_p('u') || 'l') | (ch_p('l') || 'u')]
                    ,

                    hex_lit =
                            (ch_p('X') | ch_p('x'))
                        >>  uint_parser<unsigned long, 16>()
                            [
                                self.val = arg1
                            ]
                    ,
                        
                    oct_lit =
                       !uint_parser<unsigned long, 8>()
                        [
                            self.val = arg1
                        ]
                    ,
                        
                    dec_lit =
                        uint_parser<unsigned long, 10>()
                        [
                            self.val = arg1
                        ]
                           
                    )
                ;
                
            BOOST_SPIRIT_DEBUG_TRACE_RULE(int_lit, TRACE_INTLIT_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(sub_int_lit, TRACE_INTLIT_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(hex_lit, TRACE_INTLIT_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(oct_lit, TRACE_INTLIT_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(dec_lit, TRACE_INTLIT_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return int_lit; }
    };
};

#undef TRACE_INTLIT_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following function is defined here, to allow the separation of 
//  the compilation of the intlit_grammap from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_INTLITGRAMMAR_GEN_INLINE
#else
#define CPP_INTLITGRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_INTLITGRAMMAR_GEN_INLINE 
unsigned long 
intlit_grammar_gen<TokenT>::evaluate(TokenT const &token)
{
    using namespace boost::spirit;
    
static intlit_grammar g;
unsigned long result = 0;
typename TokenT::string_t const &token_val = token.get_value();
parse_info<typename TokenT::string_t::const_iterator> hit =
    parse(token_val.begin(), token_val.end(), g[assign(result)]);

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            token_val, token);
    }
    return result;
}

#undef CPP_INTLITGRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_INTLIT_GRAMMAR_HPP__2E1E70B1_F15C_4132_8554_10A231B0D91C__INCLUDED_)
