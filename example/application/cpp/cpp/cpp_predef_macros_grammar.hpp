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

#if !defined(_CPP_PREDEF_MACROS_GRAMMAR_HPP__53858C9A_C202_4D60_AD92_DC9CAE4DBB43__INCLUDED_)
#define _CPP_PREDEF_MACROS_GRAMMAR_HPP__53858C9A_C202_4D60_AD92_DC9CAE4DBB43__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/parse_tree.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/utility/lists.hpp>

#include "cpp/cpp_grammar_gen.hpp"
#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_PREDEF_MACROS_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_PREDEF_MACROS_GRAMMAR) \
    /**/

///////////////////////////////////////////////////////////////////////////////
// Encapsulation of the grammar for command line driven predefined macros.
struct cpp_predefined_macros_grammar : 
    public boost::spirit::grammar<cpp_predefined_macros_grammar>
{
    template <typename ScannerT>
    struct definition
    {
    // 'normal' (parse_tree generating) rule type
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t plain_define, macro_definition, macro_parameters;

        definition(cpp_predefined_macros_grammar const &self) 
        {
        // import the spirit and cpplexer namespaces here
            using namespace boost::spirit;
            using namespace cpplexer;
            
        // save the rule id's for later use
            self.rule_ids.plain_define_id = plain_define.id().to_long();
            self.rule_ids.macro_parameters_id = macro_parameters.id().to_long();
            self.rule_ids.macro_definition_id = macro_definition.id().to_long();

        // recognizes command line defined macro syntax, i.e.
        //  -DMACRO
        //  -DMACRO=
        //  -DMACRO=value
        //  -DMACRO(x)
        //  -DMACRO(x)=
        //  -DMACRO(x)=value

        // This grammar resembles the overall structure of the cpp_grammar to
        // make it possible to reuse the parse tree traversal code
            plain_define
                =   ch_p(T_IDENTIFIER) 
                    >>  !macro_parameters
                    >>  !macro_definition
                ;

        // parameter list
            macro_parameters
                =   confix_p(
                        no_node_d[ch_p(T_LEFTPAREN) >> *ch_p(T_SPACE)],
                       !list_p(
                            ch_p(T_IDENTIFIER), 
                            no_node_d
                            [
                                *ch_p(T_SPACE) >> ch_p(T_COMMA) >> *ch_p(T_SPACE)
                            ]
                        ),
                        no_node_d[*ch_p(T_SPACE) >> ch_p(T_RIGHTPAREN)]
                    )
                ;

        // macro body (anything left until eol)
            macro_definition
                =   no_node_d[ch_p(T_ASSIGN)]
                    >> *anychar_p
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(plain_define, TRACE_PREDEF_MACROS_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(macro_definition, TRACE_PREDEF_MACROS_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(macro_parameters, TRACE_PREDEF_MACROS_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return plain_define; }
    };

    cpp_predefined_macros_grammar_rule_ids &rule_ids;
    
    cpp_predefined_macros_grammar(cpp_predefined_macros_grammar_rule_ids &rule_ids_) 
    :   rule_ids(rule_ids_)
    { 
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, 
            "cpp_predefined_macros_grammar", TRACE_PREDEF_MACROS_GRAMMAR); 
    }

};

///////////////////////////////////////////////////////////////////////////////
#undef TRACE_PREDEF_MACROS_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following parse function is defined here, to allow the separation of 
//  the compilation of the cpp_grammar from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_PREDEF_MACROS_GRAMMAR_GEN_INLINE
#else
#define CPP_PREDEF_MACROS_GRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_PREDEF_MACROS_GRAMMAR_GEN_INLINE 
boost::spirit::tree_parse_info<cpplexer::lex_iterator<TokenT> >
cpp_predefined_macros_grammar_gen<TokenT>::parse_predefined_macro (
    cpplexer::lex_iterator<TokenT> const &first,
    cpplexer::lex_iterator<TokenT> const &last)
{
    static cpp_predefined_macros_grammar g(rule_ids);
    return boost::spirit::pt_parse (first, last, g);
}

#undef CPP_PREDEF_MACROS_GRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_PREDEF_MACROS_GRAMMAR_HPP__53858C9A_C202_4D60_AD92_DC9CAE4DBB43__INCLUDED_)
