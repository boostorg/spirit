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

#if !defined(_CPP_GRAMMAR_GEN_HPP__80CB8A59_5411_4E45_B406_62531A12FB99__INCLUDED_)
#define _CPP_GRAMMAR_GEN_HPP__80CB8A59_5411_4E45_B406_62531A12FB99__INCLUDED_

#include <boost/spirit/tree/parse_tree.hpp>

#include "cpplexer/cpp_lex_iterator.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

namespace grammars {

///////////////////////////////////////////////////////////////////////////////
//  
//  store parser_id's of all rules of the cpp_grammar here for later access
//
///////////////////////////////////////////////////////////////////////////////
struct cpp_grammar_rule_ids {
    long pp_statement_id;
    long include_file_id;       // #include "..."
    long sysinclude_file_id;    // #include <...>
    long macroinclude_file_id;  // #include ...
    long plain_define_id;       // #define
    long macro_parameters_id;
    long macro_definition_id;
    long undefine_id;           // #undef
    long ifdef_id;              // #ifdef
    long ifndef_id;             // #ifndef
    long if_id;                 // #if
    long elif_id;               // #elif
    long else_id;               // #else
    long endif_id;              // #endif
    long line_id;               // #line
    long error_id;              // #error
    long warning_id;            // #warning
    long null_id;               // #
    long pragma_id;             // #pragma
    long illformed_id;
    long ppspace_id;
};

///////////////////////////////////////////////////////////////////////////////
//  
//  cpp_grammar_gen template class
//
//      This template helps separating the compilation of the cpp_grammar 
//      class from the compilation of the main pp_iterator. This is done to
//      safe compilation time.
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
struct cpp_grammar_gen
{
    typedef cpplexer::lex_iterator<TokenT>  iterator_t;

//  the parser_id's of all rules of the cpp_grammar are stored here
//  note: these are valid only after the first call to parse_cpp_grammar
    static cpp_grammar_rule_ids rule_ids;

//  the actual position of the last matched T_NEWLINE is stored here into the
//  member 'pos_of_newline'
    static typename TokenT::position_t pos_of_newline;
    
//  parse the cpp_grammar and return the resulting parse tree    
    static boost::spirit::tree_parse_info<iterator_t> 
    parse_cpp_grammar (iterator_t const &first, iterator_t const &last);
};

///////////////////////////////////////////////////////////////////////////////
//  definitions of the static members
template <typename TokenT>
cpp_grammar_rule_ids cpp_grammar_gen<TokenT>::rule_ids;

template <typename TokenT>
typename TokenT::position_t cpp_grammar_gen<TokenT>::pos_of_newline;

///////////////////////////////////////////////////////////////////////////////
//  
//  store parser_id's of all rules of the cpp_predefined_macros_grammar here 
//  for later access
//
///////////////////////////////////////////////////////////////////////////////
struct cpp_predefined_macros_grammar_rule_ids {
    long plain_define_id;       // #define
    long macro_parameters_id;
    long macro_definition_id;
};

///////////////////////////////////////////////////////////////////////////////
//  
//  cpp_predefined_macros_grammar_gen template class
//
//      This template helps separating the compilation of the 
//      cpp_predefined_macros_grammar class from the compilation of the 
//      main pp_iterator. This is done to safe compilation time.
//
//      This class helps parsing command line given macro definitions in a
//      similar way, as macros are parsed by the cpp_grammar class.
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
struct cpp_predefined_macros_grammar_gen
{
    typedef cpplexer::lex_iterator<TokenT>  iterator_t;

//  the parser_id's of all rules of the cpp_grammar are stored here
//  note: these are valid only after the first call to parse_cpp_grammar
    static cpp_predefined_macros_grammar_rule_ids rule_ids;

//  parse the cpp_grammar and return the resulting parse tree    
    static boost::spirit::tree_parse_info<iterator_t> 
    parse_predefined_macro (iterator_t const &first, iterator_t const &last);
};

///////////////////////////////////////////////////////////////////////////////
//  definitions of the static members
template <typename TokenT>
cpp_predefined_macros_grammar_rule_ids 
    cpp_predefined_macros_grammar_gen<TokenT>::rule_ids;

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_GRAMMAR_GEN_HPP__80CB8A59_5411_4E45_B406_62531A12FB99__INCLUDED_)
