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

#if !defined(_CPP_CHLIT_GRAMMAR_HPP__9527D349_6592_449A_A409_42A001E6C64C__INCLUDED_)
#define _CPP_CHLIT_GRAMMAR_HPP__9527D349_6592_449A_A409_42A001E6C64C__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute/closure.hpp>

#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/statements.hpp>

#include "cpp/cpp_exceptions.hpp"   
#include "cpp/cpp_literal_grammar_gen.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Reusable grammar to parse a C++ style character literal 
//
///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace closures {

    struct chlit_closure 
    :   boost::spirit::closure<chlit_closure, unsigned int> 
    {
        member1 val;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CHLIT_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CHLIT_GRAMMAR) \
    /**/

struct chlit_grammar :
    public boost::spirit::grammar<chlit_grammar, 
        closures::chlit_closure::context_t>
{
    chlit_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "chlit_grammar", 
            TRACE_CHLIT_GRAMMAR);
    }
    
    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t ch_lit;

        definition(chlit_grammar const &self)
        {
            using namespace boost::spirit;
            
            ch_lit
                =  !ch_p('L') 
                    >>  ch_p('\'')
                    >>  (   ch_p('\\') 
                        >>  (   ch_p('a')[self.val = 0x07]    // BEL
                            |   ch_p('b')[self.val = 0x08]    // BS
                            |   ch_p('t')[self.val = 0x09]    // HT
                            |   ch_p('n')[self.val = 0x0a]    // NL
                            |   ch_p('v')[self.val = 0x0b]    // VT
                            |   ch_p('f')[self.val = 0x0c]    // FF
                            |   ch_p('r')[self.val = 0x0d]    // CR
                            |   ch_p('?')[assign(self.val)]
                            |   ch_p('\'')[assign(self.val)]
                            |   ch_p('\"')[assign(self.val)]
                            |   ch_p('\\')[assign(self.val)]
                            |   ch_p('x') 
                                >>  hex_p[assign(self.val)]
                            |   uint_parser<unsigned int, 8, 1, 3>()
                                [
                                    assign(self.val)
                                ]
                            |   ch_p('u') 
                                >>  uint_parser<unsigned int, 16, 4, 4>()
                                    [
                                        assign(self.val)
                                    ]
                            |   ch_p('U')
                                >>  uint_parser<unsigned int, 16, 8, 8>()
                                    [
                                        assign(self.val)
                                    ]
                            )
                        |   anychar_p[assign(self.val)]
                        )
                    >>  ch_p('\'')
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(ch_lit, TRACE_CHLIT_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return ch_lit; }
    };
};

#undef TRACE_CHLIT_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following function is defined here, to allow the separation of 
//  the compilation of the intlit_grammap from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_CHLITGRAMMAR_GEN_INLINE
#else
#define CPP_CHLITGRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_CHLITGRAMMAR_GEN_INLINE 
unsigned int
chlit_grammar_gen<TokenT>::evaluate(TokenT const &token)
{
    using namespace boost::spirit;
    
static chlit_grammar g;
unsigned int result = 0;
typename TokenT::string_t const &token_val = token.get_value();
parse_info<typename TokenT::string_t::const_iterator> hit =
    parse(token_val.begin(), token_val.end(), g[assign(result)]);

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            token_val, token);
    }
    return result;
}

#undef CPP_CHLITGRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_CHLIT_GRAMMAR_HPP__9527D349_6592_449A_A409_42A001E6C64C__INCLUDED_)
