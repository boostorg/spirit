/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the preprocessor context
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_CONTEXT_HPP__907485E2_6649_4A87_911B_7F7225F3E5B8__INCLUDED_)
#define _CPP_CONTEXT_HPP__907485E2_6649_4A87_911B_7F7225F3E5B8__INCLUDED_

#include <string>
#include <vector>
#include <stack>

#include "cpplexer/cpp_token_ids.hpp"

#include "cpp/unput_queue_iterator.hpp"
#include "cpp/cpp_ifblock.hpp"
#include "cpp/cpp_include_pathes.hpp"
#include "cpp/cpp_iterator.hpp"
#include "cpp/cpp_iteration_context.hpp"
#include "cpp/cpp_macromap.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

///////////////////////////////////////////////////////////////////////////////
// 
//  The C preprocessor context template class
//
//      The cpp::context template is the main interface class to 
//      control the behaviour of the preprocessing engine.
//
//      The following template parameter has to be supplied:
//
//      TokenT          The token type to return from the preprocessing
//      InputPolicyT    The input policy type to use for loading the files
//                      to be included. This template parameter is optional and 
//                      defaults to the 
//                          iteration_context_policies::
//                          load_file_to_string<cpplexer::lex_iterator<TokenT> >
//                      type
//
///////////////////////////////////////////////////////////////////////////////

template <
    typename TokenT, 
    typename InputPolicyT = iteration_context_policies::
        load_file_to_string<cpplexer::lex_iterator<TokenT> > 
>
class context
:   public InputPolicyT 
{
public:

    typedef typename TokenT::iterator_t     target_iterator_t;

    typedef cpplexer::lex_iterator<TokenT>  lex_t;
    typedef context<TokenT, InputPolicyT>   self_t;
    typedef pp_iterator<self_t>             iterator_t;
    typedef TokenT                          token_t;
    typedef InputPolicyT                    input_policy_t;
    typedef typename TokenT::position_t     position_t;
        
private:
// stack of shared_ptr's to the pending iteration contexts 
    typedef boost::shared_ptr<base_iteration_context<lex_t> > iteration_ptr_t;
    typedef std::stack<iteration_ptr_t> iteration_context_stack_t;

public:
    context(target_iterator_t const &first_, target_iterator_t const &last_, 
            char const *fname = "<Unknown>")
    :   first(first_), last(last_), filename(fname)
    {}

// iterator interface
    iterator_t begin() 
    {   
        macros.init_predefined_macros();
        return iterator_t(*this, first, last, position_t(filename)); 
    }
    iterator_t end() const { return iterator_t(); }

// maintain include pathes
    bool add_include_path(char const *path_)
        { return includes.add_include_path(path_, false);}
    bool add_sysinclude_path(char const *path_)
        { return includes.add_include_path(path_, true);}
    void set_sys_include_delimiter() { includes.set_sys_include_delimiter(); }
    int get_iteration_depth() const { return iter_ctxs.size(); }

// maintain defined macros
    bool add_macro_definition(TokenT const &name, bool has_params,
            std::vector<TokenT> &parameters, std::list<TokenT> &definition)
        { return macros.add_macro(name, has_params, parameters, definition); }
    bool is_defined_macro(typename TokenT::string_t const &name) const
        { return macros.is_defined(name); }
    bool remove_macro_definition(TokenT const &name)
        { return macros.remove_macro(name); }
    void reset_macro_definitions() { macros.reset_macromap(); }

// get the pp-iterator version information 
    static std::string get_version()  
        { return util::predefined_macros::get_fullversion(false); }
    static std::string get_version_string()  
        { return util::predefined_macros::get_versionstr(false); }
    
#if !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
protected:
    typedef typename iterator_t::input_policy_t pp_iterator_policy_t;
    friend pp_iterator_policy_t;
#endif
    
// maintain include pathes (helper functions)
    bool find_include_file (std::string &s, bool is_system) const
        { return includes.find_include_file(s, is_system); }
    void set_current_directory(char const *path_) 
        { includes.set_current_directory(path_); }

// conditional compilation contexts
    bool get_if_block_status() const { return ifblocks.get_status(); }
    void enter_if_block(bool new_status) 
        { ifblocks.enter_if_block(new_status); }
    bool enter_elif_block(bool new_status) 
        { return ifblocks.enter_elif_block(new_status); }
    bool enter_else_block() { return ifblocks.enter_else_block(); }
    bool exit_if_block() { return ifblocks.exit_if_block(); }

// stack of iteration contexts
    iteration_ptr_t pop_iteration_context()
        { iteration_ptr_t top = iter_ctxs.top(); iter_ctxs.pop(); return top; }
    void push_iteration_context(iteration_ptr_t iter_ctx)
        { iter_ctxs.push(iter_ctx); }

    token_t &get_main_token() { return macros.get_main_token(); }
    
///////////////////////////////////////////////////////////////////////////////
//
//  expand_tokensequence(): 
//      expands all macros contained in a given token sequence, handles '##' 
//      and '#' pp operators and re-scans the resulting sequence 
//      (essentially preprocesses the token sequence).
//
//      The expand_undefined parameter is true during macro expansion inside
//      a C++ expression given for a #if or #elif statement. 
//
///////////////////////////////////////////////////////////////////////////////
    template <typename IteratorT>
    TokenT expand_tokensequence(IteratorT &first, IteratorT const &last, 
        std::list<token_t> &pending, std::list<token_t> &expanded, 
        bool expand_undefined = false)
    {
        return macros.expand_tokensequence(first, last, pending, expanded, 
            expand_undefined);
    }

    template <typename IteratorT>
    void expand_whole_tokensequence(IteratorT &first, IteratorT const &last, 
        std::list<token_t> &expanded, bool expand_undefined = true)
    {
        macros.expand_whole_tokensequence(expanded, first, last, 
            expand_undefined);
    }

private:
// the main input stream
    target_iterator_t const &first; // underlying input stream
    target_iterator_t const &last;
    std::string filename;           // associated main filename

    util::if_block_stack ifblocks;  // conditional compilation contexts
    util::include_pathes includes;  // lists of include directories to search
    iteration_context_stack_t iter_ctxs;    // iteration contexts
    util::macromap<TokenT> macros;  // map of defined macros
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_CONTEXT_HPP__907485E2_6649_4A87_911B_7F7225F3E5B8__INCLUDED_)
