/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the preprocessor iterator
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_ITERATOR_HPP__175CA88F_7273_43FA_9039_BCF7459E1F29__INCLUDED_)
#define _CPP_ITERATOR_HPP__175CA88F_7273_43FA_9039_BCF7459E1F29__INCLUDED_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>
#include <boost/spirit/tree/parse_tree_utils.hpp>

#include "cpplexer/cpp_lex_iterator.hpp"

#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_grammar_gen.hpp"
#include "cpp/cpp_iteration_context.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace impl {

///////////////////////////////////////////////////////////////////////////////
//  
//  pp_iterator_functor
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT> 
class pp_iterator_functor {

    typedef typename ContextT::lex_t                lex_t;
    typedef typename lex_t::token_t                 result_type;
    typedef typename result_type::string_t          string_t;
    typedef cpp::cpp_grammar_gen<result_type>       cpp_grammar_t;

    typedef base_iteration_context<lex_t>           base_iteration_context_t;
    typedef 
        iteration_context<lex_t, typename ContextT::input_policy_t>
        iteration_context_t;
    
public:
    template <typename IteratorT>
    pp_iterator_functor(ContextT &ctx_, IteratorT const &first_, 
            IteratorT const &last_, std::string const &filename_)
    :   ctx(ctx_), 
        iter_ctx(new base_iteration_context_t(
            lex_t(first_, last_, filename_), lex_t(), filename_)), 
        seen_newline(true)
    {}
    
// get the next preprocessed token
    result_type get();

// get the last recognized token (for error processing etc.)
    result_type const &current_token() const { return act_token; }
    
protected:
    result_type pp_token();
    bool pp_directive();
    void dispatch_directive(boost::spirit::tree_parse_info<lex_t> const &hit);

    void on_include (result_type const &t, bool is_system);
                    
private:
    ContextT &ctx;              // context, this iterator is assicciated with
    boost::shared_ptr<base_iteration_context_t> iter_ctx;
    
    result_type act_token;      // current token 
    bool seen_newline;          // needed for recognizing begin of line
};

///////////////////////////////////////////////////////////////////////////////
//
//  get(): get the next preprocessed token
//
//      throws a pp_exception, if appropriate
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline typename pp_iterator_functor<ContextT>::result_type
pp_iterator_functor<ContextT>::get()
{
    using namespace cpplexer;

// test for EOI, if there is a pending input context, pop it back and continue
// parsing with it
bool returned_from_include = false;

    if (iter_ctx->first == iter_ctx->last && ctx.get_iteration_depth() > 0) {
    // restore the previous iteration context after preprocessing the included
    // file
        iter_ctx = ctx.pop_iteration_context();
        returned_from_include = true; // fake a newline to trigger pp_directive

    // restore the actual current directory 
        ctx.set_current_directory(iter_ctx->filename.c_str());
    }
    
// try to generate the next token 
    if (iter_ctx->first != iter_ctx->last) {
        do {
        bool was_seen_newline = seen_newline || returned_from_include;
    
        act_token = *iter_ctx->first;
        if (act_token == T_NEWLINE) {   // a newline is to be returned asap
            seen_newline = true;
            ++iter_ctx->first;
            return act_token;
        }
        seen_newline = false;

        if (was_seen_newline && pp_directive()) {
        // a pp directive was found, return the corresponding eol only
            seen_newline = true;
            return result_type(T_NEWLINE, 
                typename result_type::string_t("\n"), 
                cpp_grammar_t::pos_of_newline);
        }
        else if (ctx.get_if_block_status()) {
            // preprocess this token, eat up more, if appropriate, return the 
            // next preprocessed token
            return pp_token();
        }
        else {
            // compilation condition is false: if the current token is a 
            // newline, return it, otherwise discard the actual token and try 
            // the next one
            if (act_token == T_NEWLINE) {
                seen_newline = true;
                return act_token;
            }

            // next token
                ++iter_ctx->first;
        }
            
        } while (iter_ctx->first != iter_ctx->last);
    }
    else if (returned_from_include) {
    // if there was an '#include' statement on the last line of a file we have
    // to return an additional newline token
        return result_type(T_NEWLINE, 
            typename result_type::string_t("\n"), 
            cpp_grammar_t::pos_of_newline);
    }
    return result_type();   // return eof token
}

///////////////////////////////////////////////////////////////////////////////
//
//  pptoken(): return the next preprocessed token
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline typename pp_iterator_functor<ContextT>::result_type
pp_iterator_functor<ContextT>::pp_token()
{
    // TODO: take the next preprocessed token from the unput queue or
    //       call the lexer, preprocess the required number of tokens, put them
    //       into the unput queue and return the next preprocessed token
    return *iter_ctx->first++;
}

///////////////////////////////////////////////////////////////////////////////
//
//  pp_directive(): recognize a preprocessor directive
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline bool
pp_iterator_functor<ContextT>::pp_directive()
{
boost::spirit::tree_parse_info<lex_t> hit = 
    cpp_grammar_t::parse_cpp_grammar(iter_ctx->first, iter_ctx->last);

    if (hit.match) {
    // position the iterator past the matched sequence to allow 
    // resynchronisation, if an error occurs
        iter_ctx->first = hit.stop;
        
    // found a valid pp directive, dispatch to the correct function to handle 
    // the found pp directive
        dispatch_directive (hit);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  dispatch_directive(): dispatch a recognized preprocessor directive
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::dispatch_directive(
    boost::spirit::tree_parse_info<lex_t> const &hit)
{
    using namespace boost::spirit;

    typedef node_val_data_factory<nil_t> node_factory_t;
    typedef tree_match<lex_t, node_factory_t> parse_tree_match_t;
    typedef typename parse_tree_match_t::container_t parse_tree_t;
    typedef typename parse_tree_match_t::parse_node_t parse_node_t;
        
parse_tree_t::const_iterator begin = hit.trees.begin();

// decide, which preprocessor directive was found
parse_tree_t const &root = (*begin).children;
parse_node_t const &node = boost::spirit::get_first_leaf(*root.begin()).value;
long node_id = node.id().to_long();

    if (node_id == cpp_grammar_t::rule_ids.include_file_id) {
    // #include "..."
        on_include (*node.begin(), false);
    }
    else if (node_id == cpp_grammar_t::rule_ids.sysinclude_file_id) {
    // #include <...>
        on_include (*node.begin(), true);
    }
    else if (node_id == cpp_grammar_t::rule_ids.macroinclude_file_id) {
    // #include ...
    }
    else if (node_id == cpp_grammar_t::rule_ids.plain_define_id) {
    // #define
    }
    else if (node_id == cpp_grammar_t::rule_ids.undefine_id) {
    // #undef
    }
    else if (node_id == cpp_grammar_t::rule_ids.ifdef_id) {
    // #ifdef
    }
    else if (node_id == cpp_grammar_t::rule_ids.ifndef_id) {
    // #ifndef
    }
    else if (node_id == cpp_grammar_t::rule_ids.if_id) {
    // #if
    }
    else if (node_id == cpp_grammar_t::rule_ids.elif_id) {
    // #elif
    }
    else if (node_id == cpp_grammar_t::rule_ids.else_id) {
    // #else
    }
    else if (node_id == cpp_grammar_t::rule_ids.endif_id) {
    // #endif
    }
    else if (node_id == cpp_grammar_t::rule_ids.line_id) {
    // #line
    }
    else if (node_id == cpp_grammar_t::rule_ids.error_id) {
    // #error
    }
    else if (node_id == cpp_grammar_t::rule_ids.warning_id) {
    // #warning
    }
    else if (node_id == cpp_grammar_t::rule_ids.null_id) {
    // #
    }
    else if (node_id == cpp_grammar_t::rule_ids.pragma_id) {
    // #pragma
    }
    else if (node_id == cpp_grammar_t::rule_ids.illformed_id) {
    // #something else
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
//  on_include: handle #include <...> or #include "..." directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_include (result_type const &t, bool is_system) 
{
    namespace fs = boost::filesystem;
    
//  skip this include, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// strip quotes first, extract filename
string_t const &s = t.get_value();
string_t::size_type pos_end = s.find_last_of(is_system ? '>' : '\"');

    if (string_t::size_type(string::npos) == pos_end) {
        CPP_THROW(preprocess_exception, bad_include_statement, s, act_token);
    }

string_t::size_type pos_begin = 
    s.find_last_of(is_system ? '<' : '\"', pos_end-1);

    if (string_t::size_type(string::npos) == pos_begin) {
        CPP_THROW(preprocess_exception, bad_include_statement, s, act_token);
    }

string_t file_path(s.substr(pos_begin+1, pos_end-pos_begin-1));

// try to locate the given file, searching through the include path lists
    if (!ctx.find_include_file (file_path, is_system)) {
        CPP_THROW(preprocess_exception, bad_include_file, file_path, act_token);
    }
    
fs::path native_path(file_path, fs::native);
std::ifstream inclstrm(native_path.native_file_string().c_str());

    if (!fs::exists(native_path)) {
        CPP_THROW(preprocess_exception, bad_include_file, file_path, act_token);
    }

// the new include file determines the actual current directory
    ctx.set_current_directory(file_path.c_str());
    
// preprocess the opened file
boost::shared_ptr<base_iteration_context_t> new_iter_ctx (
    new iteration_context_t(native_path.native_file_string(), act_token));

// push the old iteration context onto the stack and continue with the new
    ctx.push_iteration_context(iter_ctx);
    iter_ctx = new_iter_ctx;
    seen_newline = true;    // fake a newline to trigger pp_directive
}

///////////////////////////////////////////////////////////////////////////////
//  
//  pp_iterator_functor_shim
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT> 
class pp_iterator_functor_shim 
{
    typedef typename ContextT::lex_t        lex_t;
    typedef pp_iterator_functor<ContextT>   functor_t;
    
public:
    template <typename IteratorT>
    pp_iterator_functor_shim(ContextT &ctx, IteratorT const &first, 
            IteratorT const &last, std::string const &filename)
    :   functor_ptr(new functor_t(ctx, first, last, filename)) 
    {}

// interface to the boost::spirit::multi_pass_policies::functor_input policy
    typedef typename lex_t::token_t result_type;

// VC7.1 gives a linker error, if the following is defined static
    /*static*/ result_type const eof;
    
    result_type operator()() 
    { 
        BOOST_ASSERT(0 != functor_ptr.get());
        return functor_ptr->get(); 
    }

private:
    boost::shared_ptr<functor_t> functor_ptr;
};

///////////////////////////////////////////////////////////////////////////////
//  eof token
// VC7.1 gives a linker error, if the following is defined static
//template <typename LexT>
//typename pp_iterator_functor_shim<LexT>::result_type const
//    pp_iterator_functor_shim<LexT>::eof;

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
//  
//  pp_iterator
//
//      The cpp::pp_iterator template is the iterator, through which
//      the resulting preprocessed input stream is accessible.
//  
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT>
class pp_iterator 
:   public boost::spirit::multi_pass<
        impl::pp_iterator_functor_shim<ContextT>,
        boost::spirit::multi_pass_policies::functor_input
    >
{
    typedef impl::pp_iterator_functor_shim<ContextT> input_policy_t;
    typedef 
        boost::spirit::multi_pass<input_policy_t, 
                boost::spirit::multi_pass_policies::functor_input>
        base_t;
    typedef pp_iterator<ContextT> self_t;

public:
    pp_iterator() 
    {}
    
    template <typename IteratorT>
    pp_iterator(ContextT &ctx, IteratorT const &first, IteratorT const &last,
            std::string const &filename)
    :   base_t(input_policy_t(ctx, first, last, filename))
    {}
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_ITERATOR_HPP__175CA88F_7273_43FA_9039_BCF7459E1F29__INCLUDED_)
