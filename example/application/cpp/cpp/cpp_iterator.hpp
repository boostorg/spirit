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
#include <vector>
#include <list>
#include <cstdlib>

#include <boost/shared_ptr.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/spirit/core/assert.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>
#include <boost/spirit/tree/parse_tree_utils.hpp>

#include "cpplexer/cpp_lex_iterator.hpp"

#include "cpp/insert_whitespace_detection.hpp"
#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_iteration_context.hpp"
#include "cpp/cpp_grammar_gen.hpp"
#include "cpp/cpp_expression_grammar_gen.hpp"

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

public:
// interface to the boost::spirit::multi_pass_policies::functor_input policy
    typedef typename ContextT::lex_t::token_t       result_type;

// VC7.1 gives a linker error, if the following is defined static
    /*static*/ result_type const eof;

private:
    typedef typename ContextT::lex_t                lex_t;
    typedef typename result_type::string_t          string_t;
    typedef cpp::grammars::cpp_grammar_gen<result_type> cpp_grammar_t;

//  iteration context related types (an iteration context represents a current
//  position in an included file)
    typedef base_iteration_context<lex_t>           base_iteration_context_t;
    typedef 
        iteration_context<lex_t, typename ContextT::input_policy_t>
        iteration_context_t;

// parse tree related types
    typedef 
        boost::spirit::node_val_data_factory<boost::spirit::nil_t> 
        node_factory_t;
    typedef 
        boost::spirit::tree_match<lex_t, node_factory_t> 
        parse_tree_match_t;
    typedef typename parse_tree_match_t::node_t         parse_node_t;       // tree_node<node_val_data<> >
    typedef typename parse_tree_match_t::parse_node_t   parse_node_value_t; // node_val_data<>
    typedef typename parse_tree_match_t::container_t    parse_tree_t;       // parse_node_t::children_t

public:
    template <typename IteratorT>
    pp_iterator_functor(ContextT &ctx_, IteratorT const &first_, 
            IteratorT const &last_, typename ContextT::position_t const &pos_)
    :   ctx(ctx_), 
        iter_ctx(new base_iteration_context_t(
            lex_t(first_, last_, pos_), lex_t(), pos_.file)), 
        seen_newline(true),
        act_token(ctx_.get_main_token())
    {}
    
// get the next preprocessed token
    result_type const &operator()();

// get the last recognized token (for error processing etc.)
    result_type const &current_token() const { return act_token; }
    
protected:
    result_type const &pp_token();
    bool pp_directive();
    bool dispatch_directive(boost::spirit::tree_parse_info<lex_t> const &hit);

    void on_include(result_type const &t, bool is_system);
    void on_include(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);

    void on_define(parse_node_t const &node);
    void retrieve_macroname(parse_node_t const &node, 
        result_type &macroname);
        
    template <typename ContainerT>
    bool retrieve_macrodefinition(parse_node_t const &node, 
        boost::spirit::parser_id id, ContainerT &macroparameters);

    void on_undefine(result_type const &t);
    
    void on_ifdef(result_type const &t);
    void on_ifndef(result_type const &t);
    void on_else();
    void on_endif();
    void on_null();
    void on_illformed(result_type const &t);
        
    void on_line(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
    void on_if(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
    void on_elif(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
    void on_error(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
#if defined(CPP_SUPPORT_WARNING_DIRECTIVE)
    void on_warning(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
#endif // defined(CPP_SUPPORT_WARNING_DIRECTIVE)
    bool on_pragma(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);

private:
    ContextT &ctx;              // context, this iterator is associated with
    boost::shared_ptr<base_iteration_context_t> iter_ctx;
    
    bool seen_newline;          // needed for recognizing begin of line
    result_type &act_token;     // current token (references the macromap)
    
    std::list<result_type> unput_queue;     // tokens to be preprocessed again
    std::list<result_type> pending_queue;   // tokens already preprocessed
    
    // detect whether to insert additional whitespace in between two adjacent 
    // tokens, which otherwise would form a different token type, if 
    // retokenized
    util::insert_whitespace_detection whitespace; 
};

///////////////////////////////////////////////////////////////////////////////
//  eof token
// VC7.1 gives a linker error, if the following is defined static
//template <typename ContextT>
//typename pp_iterator_functor<ContextT>::result_type const
//    pp_iterator_functor<ContextT>::eof;

///////////////////////////////////////////////////////////////////////////////
//
//  operator()(): get the next preprocessed token
//
//      throws a preprocess_exception, if appropriate
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline typename pp_iterator_functor<ContextT>::result_type const &
pp_iterator_functor<ContextT>::operator()()
{
    using namespace cpplexer;

// if there is something in the unput_queue, then return the next token from
// there (all tokens in the queue are preprocessed already)
    if (pending_queue.size() > 0 || unput_queue.size() > 0) 
        return pp_token();      // return next token
    
// test for EOI, if there is a pending input context, pop it back and continue
// parsing with it
bool returned_from_include = false;

    if (iter_ctx->first == iter_ctx->last && ctx.get_iteration_depth() > 0) {
    // restore the previous iteration context after finishing the preprocessing 
    // of the included file
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
            if (T_NEWLINE == act_token || T_CPPCOMMENT == act_token) {   
            // a newline is to be returned asap, a C++ comment too
            // (the C++ comment token includes the trailing newline)
                ++iter_ctx->first;
                seen_newline = true;
                whitespace.shift_tokens(T_NEWLINE);  // whitespace controller
                return act_token;
            }
            seen_newline = false;

            if (was_seen_newline && pp_directive()) {
            // a pp directive was found
                seen_newline = true;
                
#if defined(CPP_RETURN_EMPTY_LINES_FOR_DIRECTIVES)
            // return the corresponding eol only, if requested
                whitespace.shift_tokens(T_NEWLINE);  // whitespace controller
                return act_token = result_type(T_NEWLINE, 
                    typename result_type::string_t("\n"), 
                    cpp_grammar_t::pos_of_newline);
#endif // defined(CPP_RETURN_EMPTY_LINES_FOR_DIRECTIVES)
            // else loop to the next token to analyze
            // ...
            }
            else if (ctx.get_if_block_status()) {
            // preprocess this token, eat up more, if appropriate, return 
            // the next preprocessed token
                return pp_token();
            }
            else {
                // compilation condition is false: if the current token is a 
                // newline, return it, otherwise discard the actual token and 
                // try the next one
                if (act_token == T_NEWLINE) {
                    seen_newline = true;
                    whitespace.shift_tokens(T_NEWLINE);  // whitespace controller
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
        seen_newline = true;
        whitespace.shift_tokens(T_NEWLINE);  // whitespace controller
        return act_token = result_type(T_NEWLINE, 
            typename result_type::string_t("\n"), 
            cpp_grammar_t::pos_of_newline);
    }
    whitespace.shift_tokens(T_EOF);  // whitespace controller
    return act_token = eof;         // return eof token
}

///////////////////////////////////////////////////////////////////////////////
//
//  pptoken(): return the next preprocessed token
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline typename pp_iterator_functor<ContextT>::result_type const &
pp_iterator_functor<ContextT>::pp_token()
{
    using namespace cpplexer;

//  call the lexer, preprocess the required number of tokens, put them
//  into the unput queue 
    act_token = ctx.expand_tokensequence(iter_ctx->first, iter_ctx->last, 
        pending_queue, unput_queue); 

    if (whitespace.must_insert(token_id(act_token), act_token.get_value())) {
    // must insert some whitespace into the output stream to avoid adjacent
    // tokens, which would form different (and wrong) tokens
        whitespace.shift_tokens(T_SPACE);
        pending_queue.push_front(act_token);        // push this token back
        return act_token = result_type(T_SPACE, 
            typename result_type::string_t(" "), 
            act_token.get_position());
    }
    
token_id id = token_id(act_token);

    if (T_NONREPLACABLE_IDENTIFIER == id)
        act_token.set_token_id(T_IDENTIFIER);
    else if (T_NEWLINE == id || T_CPPCOMMENT == id)
        seen_newline = true;

    whitespace.shift_tokens(id);
    return act_token;
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
        return dispatch_directive (hit);
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  dispatch_directive(): dispatch a recognized preprocessor directive
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline bool
pp_iterator_functor<ContextT>::dispatch_directive(
    boost::spirit::tree_parse_info<lex_t> const &hit)
{
    using namespace boost::spirit;
    typedef typename parse_tree_t::const_iterator const_child_iterator_t;
    
// this iterator points to the root node of the parse tree
const_child_iterator_t begin = hit.trees.begin();

// decide, which preprocessor directive was found
parse_tree_t const &root = (*begin).children;
parse_node_value_t const &nodeval = get_first_leaf(*root.begin()).value;
long node_id = nodeval.id().to_long();

const_child_iterator_t begin_child_it = (*root.begin()).children.begin();
const_child_iterator_t end_child_it = (*root.begin()).children.end();

    if (node_id == cpp_grammar_t::rule_ids.include_file_id) {
    // #include "..."
        on_include (*nodeval.begin(), false);
    }
    else if (node_id == cpp_grammar_t::rule_ids.sysinclude_file_id) {
    // #include <...>
        on_include (*nodeval.begin(), true);
    }
    else if (node_id == cpp_grammar_t::rule_ids.macroinclude_file_id) {
    // #include ...
        on_include (begin_child_it, end_child_it);
    }
    else if (node_id == cpp_grammar_t::rule_ids.plain_define_id) {
    // #define
        on_define (*begin);
    }
    else if (node_id == cpp_grammar_t::rule_ids.undefine_id) {
    // #undef
        on_undefine(*nodeval.begin());
    }
    else if (node_id == cpp_grammar_t::rule_ids.ifdef_id) {
    // #ifdef
        on_ifdef(*nodeval.begin());
    }
    else if (node_id == cpp_grammar_t::rule_ids.ifndef_id) {
    // #ifndef
        on_ifndef(*nodeval.begin());
    }
    else if (node_id == cpp_grammar_t::rule_ids.if_id) {
    // #if
        on_if(begin_child_it, end_child_it);
    }
    else if (node_id == cpp_grammar_t::rule_ids.elif_id) {
    // #elif
        on_elif(begin_child_it, end_child_it);
    }
    else if (node_id == cpp_grammar_t::rule_ids.else_id) {
    // #else
        on_else();
    }
    else if (node_id == cpp_grammar_t::rule_ids.endif_id) {
    // #endif
        on_endif();
    }
    else if (node_id == cpp_grammar_t::rule_ids.line_id) {
    // #line
        on_line(begin_child_it, end_child_it);
    }
    else if (node_id == cpp_grammar_t::rule_ids.error_id) {
    // #error
        on_error(begin_child_it, end_child_it);
    }
#if defined(CPP_SUPPORT_WARNING_DIRECTIVE)
    else if (node_id == cpp_grammar_t::rule_ids.warning_id) {
    // #warning
        on_warning(begin_child_it, end_child_it);
    }
#endif // defined(CPP_SUPPORT_WARNING_DIRECTIVE)
    else if (node_id == cpp_grammar_t::rule_ids.null_id) {
    // #
        on_null();
    }
    else if (node_id == cpp_grammar_t::rule_ids.pragma_id) {
    // #pragma
        return on_pragma(begin_child_it, end_child_it);
    }
    else if (node_id == cpp_grammar_t::rule_ids.illformed_id) {
    // #something else
        on_illformed(*nodeval.begin());
    }
    return true;    // return newline only
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
typename string_t::size_type pos_end = s.find_last_of(is_system ? '>' : '\"');

    if (string_t::npos == pos_end) {
        CPP_THROW(preprocess_exception, bad_include_statement, s, act_token);
    }

typename string_t::size_type pos_begin = 
    s.find_last_of(is_system ? '<' : '\"', pos_end-1);

    if (string_t::npos == pos_begin) {
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

namespace {

///////////////////////////////////////////////////////////////////////////////
// 
//  Transform Iterator Adaptor
//
//  Upon deference, apply some unary function object and return the
//  result by reference.
//
//  This class is adapted from the Boost.Iterator library, where a similar 
//  class exists, which returns the next item by value

    template <class AdaptableUnaryFunctionT>
    struct ref_transform_iterator_policies 
    :   public boost::default_iterator_policies
    {
        ref_transform_iterator_policies() 
        {}
        ref_transform_iterator_policies(const AdaptableUnaryFunctionT &f) 
        : m_f(f) {}

        template <class IteratorAdaptorT>
        typename IteratorAdaptorT::reference
        dereference(const IteratorAdaptorT &iter) const
        { return m_f(*iter.base()); }

        AdaptableUnaryFunctionT m_f;
    };

    template <class AdaptableUnaryFunctionT, class IteratorT>
    class ref_transform_iterator_generator
    {
        typedef typename AdaptableUnaryFunctionT::result_type value_type;
        
    public:
        typedef boost::iterator_adaptor<
                IteratorT,
                ref_transform_iterator_policies<AdaptableUnaryFunctionT>,
                value_type, value_type const &, value_type const *, 
                std::input_iterator_tag>
            type;
    };

    template <class AdaptableUnaryFunctionT, class IteratorT>
    inline 
    typename ref_transform_iterator_generator<
        AdaptableUnaryFunctionT, IteratorT>::type
    make_ref_transform_iterator(
        IteratorT base,
        const AdaptableUnaryFunctionT &f = AdaptableUnaryFunctionT())
    {
        typedef typename ref_transform_iterator_generator<
                    AdaptableUnaryFunctionT, IteratorT>::type 
            result_t;
        return result_t(base, f);
    }

    //  Retrieve the token value given a parse node
    //  This is used in conjunctione with the ref_transform_iterator above, to
    //  get the token values while iterating directly over the parse tree.
    template <typename TokenT, typename ParseTreeNodeT>
    struct get_token_value {

        typedef TokenT result_type;
        
        TokenT const &operator()(ParseTreeNodeT const &node) const
        {
            BOOST_SPIRIT_ASSERT(1 == std::distance(node.value.begin(), 
                node.value.end()));
            return *node.value.begin();
        }
    };
    
}   // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
//  
//  on_include(): handle #include ... directives
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_include(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
//  skip this include, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// preprocess the given token sequence (the body of the #include)
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

typename ref_transform_iterator_generator<
        get_token_value<result_type, parse_node_t>, 
        typename parse_tree_t::const_iterator
    >::type first = make_ref_transform_iterator(begin, get_value);
    
    ctx.expand_whole_tokensequence(first, 
        make_ref_transform_iterator(end, get_value), expanded);

// now, include the file
string_t s (as_string(expanded));
bool is_system = '<' == s[0] && '>' == s[s.size()-1];

    if (!is_system && !('\"' == s[0] && '\"' == s[s.size()-1])) {
    // should resolve into something like <...> or "..."
        CPP_THROW(preprocess_exception, bad_include_statement, 
            s, act_token);
    }
    on_include(expanded.front(), is_system);
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_define(): handle #define directives
//
///////////////////////////////////////////////////////////////////////////////

// retrieve the macro name from the parse tree
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::retrieve_macroname(parse_node_t const &node, 
    result_type &macroname)
{
parse_node_t const *name_node = NULL;

    using boost::spirit::find_node;
    if (!find_node(node, cpp_grammar_t::rule_ids.plain_define_id, &name_node)) 
    {
        // ill formed define statement (unexpected, should not happen)
        CPP_THROW(preprocess_exception, bad_define_statement, 
            "bad parse tree", act_token);
    }
    
typename parse_node_t::children_t const &children = name_node->children;

    if (0 == children.size() || 
        children[0].value.begin() == children[0].value.end()) 
    {
        // ill formed define statement (unexpected, should not happen)
        CPP_THROW(preprocess_exception, bad_define_statement, 
            "bad parse tree", act_token);
    }

// retrieve the macro name
    macroname = *children[0].value.begin();
}

// retrieve the macro parameters or the macro definition from the parse tree
template <typename ContextT> 
template <typename ContainerT>
inline bool  
pp_iterator_functor<ContextT>::retrieve_macrodefinition(
    parse_node_t const &node, boost::spirit::parser_id id, 
    ContainerT &macrodefinition)
{
    using namespace cpplexer;
    typedef typename parse_node_t::const_tree_iterator const_tree_iterator;

// find macro parameters/macro definition inside the parse tree
std::pair<const_tree_iterator, const_tree_iterator> nodes;

    using boost::spirit::get_node_range;
    if (get_node_range(node, id, nodes)) {
    // copy all parameters to the supplied container
        typename ContainerT::iterator last_nonwhite = macrodefinition.end();
        const_tree_iterator end = nodes.second;
        
        for (const_tree_iterator cit = nodes.first; cit != end; ++cit) {
            if ((*cit).value.begin() != (*cit).value.end()) {
            typename ContainerT::iterator inserted = macrodefinition.insert(
                macrodefinition.end(), *(*cit).value.begin());
                
                if (!IS_CATEGORY(macrodefinition.back(), WhiteSpaceTokenType) &&
                    T_NEWLINE != token_id(macrodefinition.back()))
                {
                    last_nonwhite = inserted;
                }
            }
        }
        
    // trim trailing whitespace (leading whitespace is trimmed by the grammar)
        if (last_nonwhite != macrodefinition.end()) {
            macrodefinition.erase(++last_nonwhite, macrodefinition.end());
        }
        return true;
    }
    return false;
}

template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_define (parse_node_t const &node) 
{
//  skip this define, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// retrieve the macro definition from the parse tree
result_type macroname;
std::vector<result_type> macroparameters;
std::list<result_type> macrodefinition;
bool has_parameters = false;

    retrieve_macroname(node, macroname);
    has_parameters = retrieve_macrodefinition(node, 
        cpp_grammar_t::rule_ids.macro_parameters_id, macroparameters);
    retrieve_macrodefinition(node, cpp_grammar_t::rule_ids.macro_definition_id, 
        macrodefinition);

// add the new macro to the macromap
    ctx.add_macro_definition(macroname, has_parameters, macroparameters, 
        macrodefinition);
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_define(): handle #undef directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_undefine (result_type const &token) 
{
//  skip this undefine, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// retrieve the macro name to undefine from the parse tree
    ctx.remove_macro_definition(token);       // throws for predefined macros
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_ifdef(): handle #ifdef directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_ifdef(result_type const &token)
{
    ctx.enter_if_block(ctx.is_defined_macro(token.get_value()));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_ifndef(): handle #ifndef directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_ifndef(result_type const &token)
{
    ctx.enter_if_block(!ctx.is_defined_macro(token.get_value()));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_else(): handle #else directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_else()
{
    if (!ctx.enter_else_block()) {
    // #else without matching #if
        CPP_THROW(preprocess_exception, missing_matching_if, "#else", 
            act_token);
    }
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_endif(): handle #endif directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_endif()
{
    if (!ctx.exit_if_block()) {
    // #endif without matching #if
        CPP_THROW(preprocess_exception, missing_matching_if, "#endif", 
            act_token);
    }
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_if(): handle #if directives
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_if(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
// preprocess the given sequence into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

typename ref_transform_iterator_generator<
        get_token_value<result_type, parse_node_t>, 
        typename parse_tree_t::const_iterator
    >::type first = make_ref_transform_iterator(begin, get_value);
    
    ctx.expand_whole_tokensequence(first, 
        make_ref_transform_iterator(end, get_value), expanded);
    
// parse the expression and enter the #if block
    ctx.enter_if_block(grammars::expression_grammar_gen<result_type>::
        evaluate(expanded.begin(), expanded.end(), act_token));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_elif(): handle #elif directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_elif(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
    if (ctx.get_if_block_status()) {
        if (!ctx.enter_elif_block(false)) {
        // #else without matching #if
            CPP_THROW(preprocess_exception, missing_matching_if, "#elif", 
                act_token);
        }
        return;     // #if/previous #elif was true, so don't enter this #elif 
    }
            
// preprocess the given sequence into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

typename ref_transform_iterator_generator<
        get_token_value<result_type, parse_node_t>, 
        typename parse_tree_t::const_iterator
    >::type first = make_ref_transform_iterator(begin, get_value);
    
    ctx.expand_whole_tokensequence(first, 
        make_ref_transform_iterator(end, get_value), expanded);
    
// parse the expression and enter the #elif block
    ctx.enter_elif_block(grammars::expression_grammar_gen<result_type>::
            evaluate(expanded.begin(), expanded.end(), act_token));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_null(): handles the '# (null)' directive
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_null()
{
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_illformed(): handles the illegal directive
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_illformed(result_type const &t)
{
//  skip this directive, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

    CPP_THROW(preprocess_exception, ill_formed_directive, t.get_value(), 
        act_token);
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_line(): handle #line directives
//
///////////////////////////////////////////////////////////////////////////////

namespace {

    template <typename IteratorT, typename StringT>
    bool retrieve_line_info (IteratorT first, IteratorT const &last,
        int &line, StringT &file)
    {
        using namespace cpplexer;
        if (T_INTLIT == token_id(*first)) {
        // extract line number
            line = std::atoi((*first).get_value().c_str());
            
        // extract file name (if it is given)
            while (++first != last && IS_CATEGORY(*first, WhiteSpaceTokenType)) 
                ;   // skip whitespace
                
            if (first != last && T_STRINGLIT == token_id(*first)) {
                StringT const &file_lit = (*first).get_value();
                file = file_lit.substr(1, file_lit.size()-2);
            }
            return true;
        }
        return false;
    }
}

template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_line(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
//  skip this #line, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// Try to extract the line number and file name from the given token list
// directly. If that fails, preprocess the whole token sequence and try again 
// to extract this information.
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

    typedef typename ref_transform_iterator_generator<
            get_token_value<result_type, parse_node_t>, 
            typename parse_tree_t::const_iterator
        >::type const_tree_iterator_t;
        
const_tree_iterator_t first = make_ref_transform_iterator(begin, get_value);
const_tree_iterator_t last = make_ref_transform_iterator(end, get_value);
    
// try to interpret the #line body as a number followed by an optional
// string literal
int line = 0;
string_t file_name;

    if (!retrieve_line_info(first, last, line, file_name)) {
    // preprocess the body of this #line message
        ctx.expand_whole_tokensequence(first, 
            make_ref_transform_iterator(end, get_value), expanded, false);
        if (!retrieve_line_info(expanded.begin(), expanded.end(), line, 
            file_name))
        {
            CPP_THROW(preprocess_exception, bad_line_statement, 
                as_string(expanded), act_token)
        }
    }
    
// the queues should be empty at this point
    BOOST_SPIRIT_ASSERT(unput_queue.empty());
    BOOST_SPIRIT_ASSERT(pending_queue.empty());

    if (file_name.empty())
        file_name = act_token.get_position().file;  // reuse current file name    
    iter_ctx->first.set_position(
        typename ContextT::position_t(file_name, line));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_error(): handle #error directives
//
///////////////////////////////////////////////////////////////////////////////
namespace {

    // return the string representation of a token sequence
    template <typename ContainerT>
    typename ContainerT::value_type::string_t
    as_string (ContainerT const &token_sequence)
    {
        typename ContainerT::value_type::string_t result;
        typename ContainerT::const_iterator end = token_sequence.end();
        for (typename ContainerT::const_iterator it = token_sequence.begin(); 
             it != end; ++it) 
        {
            result += (*it).get_value();
        }
        return result;
    }
}

template <typename ContextT> 
inline void 
pp_iterator_functor<ContextT>::on_error(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
//  skip this #error, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// preprocess the given sequence into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

typename ref_transform_iterator_generator<
        get_token_value<result_type, parse_node_t>, 
        typename parse_tree_t::const_iterator
    >::type first = make_ref_transform_iterator(begin, get_value);
    
#if defined(CPP_PREPROCESS_ERROR_MESSAGE_BODY)
// preprocess the body of this #warning message
    ctx.expand_whole_tokensequence(first, 
        make_ref_transform_iterator(end, get_value), expanded, false);
#else
// simply copy the body of this #warning message to the issued diagnostic
// message
    std::copy(first, make_ref_transform_iterator(end, get_value), 
        std::inserter(expanded, expanded.end()));
#endif // defined(CPP_PREPROCESS_ERROR_MESSAGE_BODY)

// throw the corresponding exception
    CPP_THROW(preprocess_exception, error_directive, as_string(expanded), 
        act_token);
}

#if defined(CPP_SUPPORT_WARNING_DIRECTIVE)
///////////////////////////////////////////////////////////////////////////////
//  
//  on_warning(): handle #warning directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline void 
pp_iterator_functor<ContextT>::on_warning(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
//  skip this #warning, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return;

// preprocess the given sequence into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

typename ref_transform_iterator_generator<
        get_token_value<result_type, parse_node_t>, 
        typename parse_tree_t::const_iterator
    >::type first = make_ref_transform_iterator(begin, get_value);
    
#if defined(CPP_PREPROCESS_ERROR_MESSAGE_BODY)
// preprocess the body of this #warning message
    ctx.expand_whole_tokensequence(first, 
        make_ref_transform_iterator(end, get_value), expanded, false);
#else
// simply copy the body of this #warning message to the issued diagnostic
// message
    std::copy(first, make_ref_transform_iterator(end, get_value), 
        std::inserter(expanded, expanded.end()));
#endif // defined(CPP_PREPROCESS_ERROR_MESSAGE_BODY)

// throw the corresponding exception
    CPP_THROW(preprocess_exception, warning_directive, as_string(expanded), 
        act_token);
}
#endif // defined(CPP_SUPPORT_WARNING_DIRECTIVE)

///////////////////////////////////////////////////////////////////////////////
//  
//  on_pragma(): handle #pragma directives
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT> 
inline bool
pp_iterator_functor<ContextT>::on_pragma(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
    using namespace cpplexer;
    
//  skip this #pragma, if conditional compilation is off
    if (!ctx.get_if_block_status()) 
        return true;

#if defined(CPP_RETURN_PRAGMA_DIRECTIVES)
// Look at the pragma token sequence and decide, if the first token is STDC
// (see C99 standard (6.10.6.2), if it is, the sequence must _not_ be
// preprocessed.
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;

    typedef typename ref_transform_iterator_generator<
            get_token_value<result_type, parse_node_t>, 
            typename parse_tree_t::const_iterator
        >::type const_tree_iterator_t;
        
const_tree_iterator_t first = make_ref_transform_iterator(begin, get_value);
const_tree_iterator_t last = make_ref_transform_iterator(end, get_value);

    BOOST_SPIRIT_ASSERT(T_PP_PRAGMA == token_id(act_token));
    expanded.push_back(act_token);     // T_PP_PRAGMA;
    expanded.push_back(result_type(T_SPACE, " ", act_token.get_position()));
        
    while (++first != last && IS_CATEGORY(*first, WhiteSpaceTokenType)) 
        expanded.push_back(*first);   // skip whitespace

    if (first != last) {
        if (T_IDENTIFIER == token_id(*first) && (*first).get_value() == "STDC") {
        // do _not_ preprocess the token sequence
            std::copy(first, last, std::inserter(expanded, expanded.end()));
        }
        else {
#if defined(CPP_PREPROCESS_PRAGMA_BODY)
        // preprocess the given tokensequence
            ctx.expand_whole_tokensequence(first, last, expanded, false);
#else
        // do _not_ preprocess the token sequence
            std::copy(first, last, std::inserter(expanded, expanded.end()));
#endif // defined(CPP_PREPROCESS_PRAGMA_BODY)
        }
    }
    expanded.push_back(result_type(T_NEWLINE, "\n", act_token.get_position()));
        
// the queues should be empty at this point
    BOOST_SPIRIT_ASSERT(unput_queue.empty());
    BOOST_SPIRIT_ASSERT(pending_queue.empty());

// Move the resulting token sequence into the pending_queue, so it will be 
// returned to the caller.
    pending_queue.splice(pending_queue.begin(), expanded);
    return false;       // do not return newline only, but the whole pragma
                        // directive
#else
    return true;        // return newline only (skip the pragma at all)
#endif // defined(CPP_RETURN_PRAGMA_DIRECTIVES)
}

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
        impl::pp_iterator_functor<ContextT>,
        boost::spirit::multi_pass_policies::functor_input
    >
{
public:
    typedef impl::pp_iterator_functor<ContextT> input_policy_t;

private:
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
            typename ContextT::position_t const &pos)
    :   base_t(input_policy_t(ctx, first, last, pos))
    {}
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_ITERATOR_HPP__175CA88F_7273_43FA_9039_BCF7459E1F29__INCLUDED_)
