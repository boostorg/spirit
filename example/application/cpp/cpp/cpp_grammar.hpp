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

#if !defined(_CPP_GRAMMAR_HPP__FEAEBC2E_2734_428B_A7CA_85E5A415E23E__INCLUDED_)
#define _CPP_GRAMMAR_HPP__FEAEBC2E_2734_428B_A7CA_85E5A415E23E__INCLUDED_

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/parse_tree.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/utility/lists.hpp>

#if defined(CPP_DUMP_PARSE_TREE)
#include <map>
#include <boost/spirit/tree/tree_to_xml.hpp>
#endif // defined(CPP_DUMP_PARSE_TREE)

#include "cpp/cpp_grammar_gen.hpp"
#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_iterator.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp { 
namespace grammars {

namespace impl {

///////////////////////////////////////////////////////////////////////////////
//
//  store_position 
//
//      The store_position functor extracts the actual file position from the 
//      supplied token.
//
///////////////////////////////////////////////////////////////////////////////

    template <typename PositionT>
    struct store_position {

        store_position(PositionT &pos_) : pos(pos_) {}
        
        template <typename TokenT>
        void operator()(TokenT const &token) const
        {
            pos = token.get_position();
        }
        
        PositionT &pos;
    };

///////////////////////////////////////////////////////////////////////////////
//
//  flush_underlying_parser
//
//      The flush_underlying_parser flushes the underlying
//      multi_pass_iterator during the normal parsing process. This is
//      used at certain points during the parsing process, when it is
//      clear, that no backtracking is needed anymore and the input
//      gathered so far may be discarded.
//
///////////////////////////////////////////////////////////////////////////////
    struct flush_underlying_parser
    :   public boost::spirit::parser<flush_underlying_parser>
    {
        typedef flush_underlying_parser this_t;

        template <typename ScannerT>
        typename boost::spirit::parser_result<this_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            scan.first.clear_queue();
            return scan.empty_match();  
        }
    };

    flush_underlying_parser const 
        flush_underlying_parser_p = flush_underlying_parser();

}   // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CPP_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR) \
    /**/

///////////////////////////////////////////////////////////////////////////////
// Encapsulation of the C++ preprocessor grammar.
template <typename PositionT>
struct cpp_grammar : 
    public boost::spirit::grammar<cpp_grammar<PositionT> >
{
    typedef cpp_grammar<PositionT>          grammar_t;
    typedef impl::store_position<PositionT> store_pos_t;
    
    template <typename ScannerT>
    struct definition
    {
    // non-parse_tree generating rule type
        typedef typename ScannerT::iteration_policy_t iteration_policy_t;
        typedef boost::spirit::match_policy match_policy_t;
        typedef typename ScannerT::action_policy_t action_policy_t;
        typedef 
            boost::spirit::scanner_policies<
                iteration_policy_t, match_policy_t, action_policy_t> 
            policies_t;
        typedef 
            boost::spirit::scanner<typename ScannerT::iterator_t, policies_t> 
            non_tree_scanner_t;
        typedef boost::spirit::rule<non_tree_scanner_t> no_tree_rule_t;

    // 'normal' (parse_tree generating) rule type
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t pp_statement;
        rule_t include_file, system_include_file, macro_include_file;
        rule_t plain_define, macro_definition, macro_parameters;
        rule_t undefine;
        rule_t ppifdef, ppifndef, ppif, ppelse, ppelif, ppendif;
        rule_t ppline; 
        rule_t pperror;
        rule_t ppwarning;
        rule_t ppnull;
        rule_t pppragma;
        rule_t illformed;
        no_tree_rule_t ppspace;

        definition(cpp_grammar const &self) 
        {
        // import the spirit and cpplexer namespaces here
            using namespace boost::spirit;
            using namespace cpplexer;
            
        // save the rule id's for later use
            self.rule_ids.pp_statement_id = pp_statement.id().to_long();
            self.rule_ids.include_file_id = include_file.id().to_long();
            self.rule_ids.sysinclude_file_id = system_include_file.id().to_long();
            self.rule_ids.macroinclude_file_id = macro_include_file.id().to_long();
            self.rule_ids.plain_define_id = plain_define.id().to_long();
            self.rule_ids.macro_parameters_id = macro_parameters.id().to_long();
            self.rule_ids.macro_definition_id = macro_definition.id().to_long();
            self.rule_ids.undefine_id = undefine.id().to_long();
            self.rule_ids.ifdef_id = ppifdef.id().to_long();
            self.rule_ids.ifndef_id = ppifndef.id().to_long();
            self.rule_ids.if_id = ppif.id().to_long();
            self.rule_ids.elif_id = ppelif.id().to_long();
            self.rule_ids.else_id = ppelse.id().to_long();
            self.rule_ids.endif_id = ppendif.id().to_long();
            self.rule_ids.line_id = ppline.id().to_long();
            self.rule_ids.error_id = pperror.id().to_long();
            self.rule_ids.warning_id = ppwarning.id().to_long();
            self.rule_ids.null_id = ppnull.id().to_long();
            self.rule_ids.pragma_id = pppragma.id().to_long();
            self.rule_ids.illformed_id = illformed.id().to_long();
            self.rule_ids.ppspace_id = ppspace.id().to_long();

#if defined(CPP_DUMP_PARSE_TREE)
            self.map_rule_id_to_name.init_rule_id_to_name_map(self);
#endif // defined(CPP_DUMP_PARSE_TREE)
            
        // recognizes preprocessor directives only

        // C++ standard 16.1: A preprocessing directive consists of a sequence 
        // of preprocessing tokens. The first token in the sequence is # 
        // preprocessing token that is either the first character in the source 
        // file (optionally after white space containing no new-line 
        // characters) or that follows white space containing at least one 
        // new-line character. The last token in the sequence is the first 
        // new-line character that follows the first token in the sequence.

            pp_statement
                =   no_node_d[*ppspace]
                    >>  (
                            (   include_file
                            |   system_include_file
                            |   macro_include_file
                            |   plain_define
                            |   undefine
                            |   ppifdef
                            |   ppifndef
                            |   ppif
                            |   ppelse
                            |   ppelif
                            |   ppendif
                            |   ppline
                            |   pperror
                            |   ppwarning
                            |   pppragma
                            |   illformed
                            |   ppnull
                            )
                            >>  no_node_d
                                [
                                   *ppspace 
                                    >>  (   ch_p(T_NEWLINE)
                                            [
                                                store_pos_t(self.pos_of_newline)
                                            ]
                                        |   ch_p(T_CPPCOMMENT)
                                            [
                                                store_pos_t(self.pos_of_newline)
                                            ]
                                        |   ch_p(T_EOF)
                                            [
                                                store_pos_t(self.pos_of_newline)
                                            ]
                                        )
                                ]
#if defined(BOOST_SPIRIT_DEBUG) && !(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR)
                            >>  impl::flush_underlying_parser_p
#endif // defined(BOOST_SPIRIT_DEBUG && !(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR)
                        )
                ;

        // #include ...
            include_file            // include "..."
                =   ch_p(T_PP_QHEADER) 
                ;

            system_include_file     // include <...>
                =   ch_p(T_PP_HHEADER) 
                ;

            macro_include_file      // include ...anything else...
                =   no_node_d[ch_p(T_PP_INCLUDE)]
                    >> *(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // #define FOO foo (with optional parameters)
            plain_define
                =   no_node_d[ch_p(T_PP_DEFINE) >> +ppspace]
                    >>  ch_p(T_IDENTIFIER) 
                    >>  !macro_parameters
                    >>  !macro_definition
                ;

        // parameter list
            macro_parameters
                =   confix_p(
                        no_node_d[ch_p(T_LEFTPAREN) >> *ppspace],
                       !list_p(
                            ch_p(T_IDENTIFIER), 
                            no_node_d
                            [
                                *ppspace >> ch_p(T_COMMA) >> *ppspace
                            ]
                        ),
                        no_node_d[*ppspace >> ch_p(T_RIGHTPAREN)]
                    )
                ;

        // macro body (anything left until eol)
            macro_definition
                =   no_node_d[+ppspace]
                    >> *(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // #undef FOO 
            undefine
                =   no_node_d[ch_p(T_PP_UNDEF) >> +ppspace]
                    >>  ch_p(T_IDENTIFIER)
                ;

        // #ifdef et.al.
            ppifdef
                =   no_node_d[ch_p(T_PP_IFDEF) >> +ppspace]
                    >>  ch_p(T_IDENTIFIER)
                ;

            ppifndef
                =   no_node_d[ch_p(T_PP_IFNDEF) >> +ppspace]
                    >>  ch_p(T_IDENTIFIER)
                ;

            ppif
                =   no_node_d[ch_p(T_PP_IF) >> +ppspace]
                    >> +(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

            ppelse
                =   no_node_d[ch_p(T_PP_ELSE)]
                ;

            ppelif
                =   no_node_d[ch_p(T_PP_ELIF) >> +ppspace]
                    >> +(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

            ppendif
                =   no_node_d[ch_p(T_PP_ENDIF)]
                ;

        // #line ...
            ppline 
                =   no_node_d[ch_p(T_PP_LINE) >> +ppspace]
                    >> +(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // # (empty preprocessor directive)
            ppnull
                =   no_node_d[ch_p(T_POUND)]    // real null directive
                ;

        // # something else (ill formed preprocessor directive)
            illformed           // for error reporting
                =   no_node_d[ch_p(T_POUND) >> *ppspace]
                    >>  (   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                    >> no_node_d
                        [
                           *(   anychar_p -
                                (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                            )
                        ]
                ;

        // #error
            pperror
                =   no_node_d[ch_p(T_PP_ERROR) >> +ppspace]
                    >> *(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // #warning
            ppwarning
                =   no_node_d[ch_p(T_PP_WARNING) >> +ppspace]
                    >> *(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // #pragma ...
            pppragma
                =   no_node_d[ch_p(T_PP_PRAGMA)] 
                    >> *(   anychar_p -
                            (ch_p(T_NEWLINE) | ch_p(T_CPPCOMMENT) | ch_p(T_EOF)) 
                        )
                ;

        // auxiliary helper rules
            ppspace     // valid space in a line with a preprocessor directive
                =   ch_p(T_SPACE) | ch_p(T_CCOMMENT)
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(pp_statement, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(include_file, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(system_include_file, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(macro_include_file, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(plain_define, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(macro_definition, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(macro_parameters, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(undefine, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppifdef, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppifndef, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppif, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppelse, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppelif, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppendif, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppline, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(pperror, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppwarning, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppnull, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(illformed, TRACE_CPP_GRAMMAR);
            BOOST_SPIRIT_DEBUG_TRACE_RULE(ppspace, TRACE_CPP_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return pp_statement; }
    };

    cpp_grammar_rule_ids &rule_ids;
    PositionT &pos_of_newline;
    
    cpp_grammar(cpp_grammar_rule_ids &rule_ids_, PositionT &pos_of_newline_) 
    :   rule_ids(rule_ids_), pos_of_newline(pos_of_newline_)
    { 
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "cpp_grammar", 
            TRACE_CPP_GRAMMAR); 
    }

#if defined(CPP_DUMP_PARSE_TREE)
// helper function and data to get readable names of the rules known to us
    struct map_ruleid_to_name :
        public std::map<boost::spirit::parser_id, std::string> 
    {
        typedef std::map<boost::spirit::parser_id, std::string> base_t;

        void init_rule_id_to_name_map(cpp_grammar const &self)
        {
            struct {
                int parser_id;
                char const *rule_name;
            } 
            init_ruleid_name_map[] = {
                { self.rule_ids.pp_statement_id, "pp_statement" },
                { self.rule_ids.include_file_id, "include_file" },
                { self.rule_ids.sysinclude_file_id, "system_include_file" },
                { self.rule_ids.macroinclude_file_id, "macro_include_file" },
                { self.rule_ids.plain_define_id, "plain_define" },
                { self.rule_ids.macro_parameters_id, "macro_parameters" },
                { self.rule_ids.macro_definition_id, "macro_definition" },
                { self.rule_ids.undefine_id, "undefine" },
                { self.rule_ids.ifdef_id, "ppifdef" },
                { self.rule_ids.ifndef_id, "ppifndef" },
                { self.rule_ids.if_id, "ppif" },
                { self.rule_ids.elif_id, "ppelif" },
                { self.rule_ids.else_id, "ppelse" },
                { self.rule_ids.endif_id, "ppendif" },
                { self.rule_ids.line_id, "ppline" },
                { self.rule_ids.error_id, "pperror" },
                { self.rule_ids.warning_id, "ppwarning" },
                { self.rule_ids.null_id, "ppnull" },
                { self.rule_ids.pragma_id, "pppragma" },
                { self.rule_ids.illformed_id, "illformed" },
                { self.rule_ids.ppspace_id, "ppspace" },
                { 0 }
            };

        // initialize parser_id to rule_name map
            for (int i = 0; 0 != init_ruleid_name_map[i].parser_id; ++i)
                base_t::insert(base_t::value_type(
                    boost::spirit::parser_id(init_ruleid_name_map[i].parser_id), 
                    std::string(init_ruleid_name_map[i].rule_name))
                );
        }
    };
    mutable map_ruleid_to_name map_rule_id_to_name;
#endif // defined(CPP_DUMP_PARSE_TREE)
};

///////////////////////////////////////////////////////////////////////////////
#undef TRACE_CPP_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following parse function is defined here, to allow the separation of 
//  the compilation of the cpp_grammar from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_GRAMMAR_INSTANTIATION)
#define CPP_GRAMMAR_GEN_INLINE
#else
#define CPP_GRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
CPP_GRAMMAR_GEN_INLINE 
boost::spirit::tree_parse_info<cpplexer::lex_iterator<TokenT> >
cpp_grammar_gen<TokenT>::parse_cpp_grammar (
    cpplexer::lex_iterator<TokenT> const &first,
    cpplexer::lex_iterator<TokenT> const &last)
{
    static cpp_grammar<typename TokenT::position_t> g(rule_ids, pos_of_newline);
    
    using namespace boost::spirit;
    tree_parse_info<cpplexer::lex_iterator<TokenT> > hit = pt_parse (first, last, g);
    
#if defined(CPP_DUMP_PARSE_TREE)
    if (hit.match) {
        tree_to_xml (CPP_DUMP_PARSE_TREE_OUT, hit.trees, "", 
            g.map_rule_id_to_name, &TokenT::get_token_id, 
            &TokenT::get_token_value);
    }
#endif // defined(CPP_DUMP_PARSE_TREE)

    return hit;
}

#undef CPP_GRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace cpp

#endif // !defined(_CPP_GRAMMAR_HPP__FEAEBC2E_2734_428B_A7CA_85E5A415E23E__INCLUDED_)
