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

#include <boost/shared_ptr.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/spirit/core/assert.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>
#include <boost/spirit/tree/parse_tree_utils.hpp>

#include "cpplexer/cpp_lex_iterator.hpp"

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
    typedef cpp::cpp_grammar_gen<result_type>       cpp_grammar_t;

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
        seen_newline(true)
    {}
    
// get the next preprocessed token
    result_type operator()();

// get the last recognized token (for error processing etc.)
    result_type const &current_token() const { return act_token; }
    
protected:
    result_type pp_token();
    bool pp_directive();
    void dispatch_directive(boost::spirit::tree_parse_info<lex_t> const &hit);

    void on_include(result_type const &t, bool is_system);

    void on_define(parse_node_t const &node);
    void retrieve_macroname(parse_node_t const &node, 
        result_type &macroname);
    bool retrieve_macrodefinition(parse_node_t const &node, 
        boost::spirit::parser_id id, std::vector<result_type> &macroparameters);

    void on_undefine(result_type const &t);
    
    void on_ifdef(result_type const &t);
    void on_ifndef(result_type const &t);
    void on_else();
    void on_endif();
    
    void on_if(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);
    void on_elif(typename parse_tree_t::const_iterator const &begin,
        typename parse_tree_t::const_iterator const &end);

private:
    ContextT &ctx;              // context, this iterator is associated with
    boost::shared_ptr<base_iteration_context_t> iter_ctx;
    
    result_type act_token;      // current token 
    bool seen_newline;          // needed for recognizing begin of line
    
    std::list<result_type> unput_queue;
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
//      throws a pp_exception, if appropriate
//
///////////////////////////////////////////////////////////////////////////////
namespace {

    template <typename TokenT>
    struct pop_front_onexit {

        pop_front_onexit(std::list<TokenT> &list_) : list(list_) {}
        ~pop_front_onexit() { list.pop_front(); }
        
        std::list<TokenT> &list;
    };
}

template <typename ContextT> 
inline typename pp_iterator_functor<ContextT>::result_type
pp_iterator_functor<ContextT>::operator()()
{
    using namespace cpplexer;

// if there is something in the unput_queue, then return the next token from
// there (all tokens in the queue are preprocessed already)
    if (unput_queue.size() > 0) {
    pop_front_onexit<result_type> popfront(unput_queue);
    
        return act_token = unput_queue.front();    // get next token
    }
    
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
            if (T_NEWLINE == act_token || T_CPPCOMMENT == act_token) {   
            // a newline is to be returned asap, a C++ comment too
            // (the C++ comment token includes the trailing newline)
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
//  call the lexer, preprocess the required number of tokens, put them
//  into the unput queue 
    BOOST_SPIRIT_ASSERT(0 == unput_queue.size());
    ctx.expand_tokensequence(iter_ctx->first, iter_ctx->last, unput_queue); 

// return the next preprocessed token
pop_front_onexit<result_type> popfront(unput_queue);

    BOOST_SPIRIT_ASSERT(0 != unput_queue.size());
    return act_token = unput_queue.front();    
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

// this iterator points to the root node of the parse tree
parse_tree_t::const_iterator begin = hit.trees.begin();

// decide, which preprocessor directive was found
parse_tree_t const &root = (*begin).children;
parse_node_value_t const &nodeval = get_first_leaf(*root.begin()).value;
long node_id = nodeval.id().to_long();

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
        on_if((*root.begin()).children.begin(), 
            (*root.begin()).children.end());
    }
    else if (node_id == cpp_grammar_t::rule_ids.elif_id) {
    // #elif
        on_elif((*root.begin()).children.begin(), 
            (*root.begin()).children.end());
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
//  on_define(): handle #define statements
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
inline bool  
pp_iterator_functor<ContextT>::retrieve_macrodefinition(
    parse_node_t const &node, boost::spirit::parser_id id, 
    std::vector<result_type> &macrodefinition)
{
    typedef typename parse_node_t::const_tree_iterator const_tree_iterator;

// find macro parameters inside the parse tree
std::pair<const_tree_iterator, const_tree_iterator> nodes;

    using boost::spirit::get_node_range;
    if (get_node_range(node, id, nodes)) {
    // copy all parameters to the supplied parameter vector
        const_tree_iterator end = nodes.second;
        for (const_tree_iterator cit = nodes.first; cit != end; ++cit) {
            if ((*cit).value.begin() != (*cit).value.end())
                macrodefinition.push_back(*(*cit).value.begin());
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
std::vector<result_type> macrodefinition;
bool has_parameters = false;

    retrieve_macroname(node, macroname);
    has_parameters = retrieve_macrodefinition(node, 
        cpp_grammar_t::rule_ids.macro_parameters_id, macroparameters);
    retrieve_macrodefinition(node, cpp_grammar_t::rule_ids.macro_definition_id, 
        macrodefinition);
    
    ctx.add_macro_definition(macroname, has_parameters, macroparameters, 
        macrodefinition);
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_define(): handle #undef statements
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
    ctx.remove_macro_definition(token.get_value());       // never fails
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_ifdef(): handle #ifdef statements
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
//  on_ifndef(): handle #ifndef statements
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
//  on_else(): handle #else statements
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
//  on_endif(): handle #endif statements
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
//  on_if(): handle #if statements
//
///////////////////////////////////////////////////////////////////////////////

namespace {

//=============================================================================
// Transform Iterator Adaptor
//
// Upon deference, apply some unary function object and return the
// result by reference.

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
}

template <typename ContextT> 
inline void  
pp_iterator_functor<ContextT>::on_if(
    typename parse_tree_t::const_iterator const &begin,
    typename parse_tree_t::const_iterator const &end)
{
// copy the sequence to preprocess into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;
    
    ctx.expand_tokensequence(
        make_ref_transform_iterator(begin, get_value),
        make_ref_transform_iterator(end, get_value), 
        expanded, false, true);
    
// parse the expression and enter the #if block
    ctx.enter_if_block(expression_grammar_gen<result_type>::
        evaluate(expanded.begin(), expanded.end(), act_token));
}

///////////////////////////////////////////////////////////////////////////////
//  
//  on_elif(): handle #elif statements
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
            
// copy the sequence to preprocess into the provided list
std::list<result_type> expanded;
get_token_value<result_type, parse_node_t> get_value;
    
    ctx.expand_tokensequence(
        make_ref_transform_iterator(begin, get_value),
        make_ref_transform_iterator(end, get_value), 
        expanded, false, true);
    
// parse the expression and enter the #elif block
    ctx.enter_elif_block(expression_grammar_gen<result_type>::
            evaluate(expanded.begin(), expanded.end(), act_token));
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
    typedef impl::pp_iterator_functor<ContextT> input_policy_t;
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
