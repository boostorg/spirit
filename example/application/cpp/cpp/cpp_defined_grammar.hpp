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

#if !defined(_CPP_DEFINED_GRAMMAR_HPP__F48287B2_DC67_40A8_B4A1_800EFBD67869__INCLUDED_)
#define _CPP_DEFINED_GRAMMAR_HPP__F48287B2_DC67_40A8_B4A1_800EFBD67869__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/core/assert.hpp>
#include <boost/spirit/attribute/closure.hpp>

#include "cpplexer/cpp_token_ids.hpp"

#include "cpp/cpp_macromap.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace closures {

    template <typename TokenT>
    struct cpp_defined_closure 
    :   boost::spirit::closure<cpp_defined_closure<TokenT>, TokenT> 
    {
        typedef typename cpp_defined_closure<TokenT>::member1 member1;
        member1 val;
    };
    
}

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CPP_DEFINED_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_DEFINED_GRAMMAR) \
    /**/

template <typename TokenT>
struct defined_grammar :
    public boost::spirit::grammar<
        defined_grammar<TokenT>, 
        typename closures::cpp_defined_closure<TokenT>::context_t
    >
{
    defined_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "defined_grammar", 
            TRACE_CPP_DEFINED_GRAMMAR);
    }

    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t defined_op;

        definition(defined_grammar const &self)
        {
            using namespace boost::spirit;
            using namespace phoenix;
            using namespace cpplexer;
            
            defined_op      // parens not required, see C++ standard 16.1.1
                =   ch_p(T_IDENTIFIER)      // token contains 'defined'
                    >>  (       ch_p(T_LEFTPAREN)
                            >>  ch_p(T_IDENTIFIER) 
                                [
                                    self.val = arg1
                                ]
                            >>  ch_p(T_RIGHTPAREN)
                        )
                        |   ch_p(T_IDENTIFIER) 
                            [
                                self.val = arg1
                            ]
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(defined_op, TRACE_CPP_DEFINED_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return defined_op; }
    };
};

///////////////////////////////////////////////////////////////////////////////
#undef TRACE_CPP_DEFINED_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_DEFINED_GRAMMAR_HPP__F48287B2_DC67_40A8_B4A1_800EFBD67869__INCLUDED_)
