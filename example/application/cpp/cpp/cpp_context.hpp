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
        { return iterator_t(*this, first, last, position_t(filename)); }
    iterator_t end()   
        { return iterator_t(); }

// conditional compilation contexts
    bool get_if_block_status() const { return ifblocks.get_status(); }
    void enter_if_block(bool new_status) 
        { ifblocks.enter_if_block(new_status); }
    bool enter_elif_block(bool new_status) 
        { return ifblocks.enter_elif_block(new_status); }
    bool enter_else_block() { return ifblocks.enter_else_block(); }
    bool exit_if_block() { return ifblocks.exit_if_block(); }

// maintain include pathes
    bool add_include_path(char const *path_, bool is_system = false)
        { return includes.add_include_path(path_, is_system);}
    void set_sys_include_delimiter() { includes.set_sys_include_delimiter(); }
    bool find_include_file (std::string &s, bool is_system) const
        { return includes.find_include_file(s, is_system); }
    void set_current_directory(char const *path_) 
        { includes.set_current_directory(path_); }

// stack of iteration contexts
    int get_iteration_depth() const { return iter_ctxs.size(); }
    iteration_ptr_t pop_iteration_context()
        { iteration_ptr_t top = iter_ctxs.top(); iter_ctxs.pop(); return top; }
    void push_iteration_context(iteration_ptr_t iter_ctx)
        { iter_ctxs.push(iter_ctx); }

// maintain defined macros
    bool add_macro_definition(TokenT const &name, bool has_params,
            std::vector<TokenT> &parameters, std::vector<TokenT> &definition)
        { return macros.add_macro(name, has_params, parameters, definition); }
    bool is_defined_macro(typename TokenT::string_t const &name) const
        { return macros.is_defined(name); }
    bool remove_macro_definition(typename TokenT::string_t const &name)
        { return macros.remove_macro(name); }

    void expand_tokensequence(std::list<token_t> &expanded, 
        bool expand_undefined = false);
        
protected:
    bool expand_tokensequence_worker(std::list<token_t> &expanded, 
        bool expand_undefined);
    void concat_tokensequence(std::list<token_t> &expanded);
            
private:
// the main input stream
    target_iterator_t const &first; // underlying input stream
    target_iterator_t const &last;
    std::string filename;           // associated main filename

    if_block_stack ifblocks;        // conditional compilation contexts
    include_pathes includes;        // lists of include directories to search
    iteration_context_stack_t iter_ctxs;    // iteration contexts
    macromap<TokenT> macros;        // map of defined macros
};

///////////////////////////////////////////////////////////////////////////////
//
//  expand_tokensequence(): 
//      expands all macros contained in a given token sequence, handles '##' 
//      and '#' pp operators and re-scans the resulting sequence, if nessecary 
//      (essentially preprocesses the token sequence).
//
//      The expand_undefined parameter is true during macro expansion inside
//      a C++ expression given for a #if or #elif statement. 
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT, typename InputPolicyT>
inline 
void context<TokenT, InputPolicyT>::expand_tokensequence(
    std::list<TokenT> &expanded, bool expand_undefined)
{
    while (expand_tokensequence_worker (expanded, expand_undefined)) 
        concat_tokensequence(expanded);     // handle '##' and '#' operators
}

template <typename TokenT, typename InputPolicyT>
inline 
bool context<TokenT, InputPolicyT>::expand_tokensequence_worker(
    std::list<TokenT> &expanded, bool expand_undefined)
{
    using namespace cpplexer;
    
//  Iterate over all the elements of the given sequence, if it is an 
//  T_IDENTIFIER token, try to replace this as a macro
bool replaced = false;       // return, whether a replacement occured

    std::list<TokenT>::iterator end = expanded.end();
    for (std::list<TokenT>::iterator it = expanded.begin(); it != end; /**/) {
    token_id id = token_id(*it);
    
        if (T_DEFINED == id) {
        // resolve operator defined()
            macros.resolve_defined(expanded, it, expanded.end());
            continue;           // 'it' is already adjusted correctly
        }
        else if (T_IDENTIFIER == id) {
        // try to replace this identifier as a macro
            if (macros.is_defined((*it).get_value())) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
                replaced = macros.expand_macro(expanded, it) 
                    || replaced;
                continue;       // 'it' is already adjusted correctly
            }
            else if (expand_undefined) {
            // in preprocessing conditionals undefined identifiers and keywords 
            // are to be replaced with '0' (see. C++ standard 16.1.4)
                (*it).set_token_id(T_INTLIT);
                (*it).set_value("0");
                replaced = true;
            }
        }
        else if (expand_undefined && IS_CATEGORY(*it, KeywordTokenType) && 
                 T_TRUE != id && T_FALSE != id)
        {
        // all remaining identifiers and keywords, except for true and false, 
        // are replaced with the pp-number 0 (C++ standard 16.1.4, [cpp.cond])
            (*it).set_token_id(T_INTLIT);
            (*it).set_value("0");
            replaced = true;
        }
        ++it;
    }
    return replaced;
}

namespace {

    // return the string representation of a token sequence
    template <typename TokenT>
    typename TokenT::string_t
    as_string (std::list<TokenT> const &token_sequence)
    {
        typename TokenT::string_t result;
        std::list<TokenT>::const_iterator end = token_sequence.end();
        for (std::list<TokenT>::const_iterator it = token_sequence.begin(); 
             it != end; ++it) 
        {
            result += (*it).get_value();
        }
        return result;
    }
}

template <typename TokenT, typename InputPolicyT>
inline 
void context<TokenT, InputPolicyT>::concat_tokensequence(
    std::list<TokenT> &expanded)
{
    using namespace cpplexer;
    
bool found_concat = false;

    std::list<TokenT>::iterator end = expanded.end();
    std::list<TokenT>::iterator prev = end;
    for (std::list<TokenT>::iterator it = expanded.begin(); it != end; ++it) {
    
        if (T_POUND_POUND == token_id(*it)) {
        std::list<TokenT>::iterator next = it;
    
            ++next;
            if (prev == end || next == end) {
            // error, '##' should be in between two tokens
                CPP_THROW(preprocess_exception, ill_formed_operator,
                    "concat ('##')", (*it));
            }

        // replace prev##next with the concatenated value, skip whitespace
        // before and after the '##' operator
            while (T_SPACE == token_id(*next) || T_SPACE2 == token_id(*next))
                ++next;

        // we leave the token_id unchanged, because the whole sequence is to be
        // re-scanned anyway
            (*prev).set_value((*prev).get_value() + (*next).get_value());

        // remove the '##' and the next tokens from the sequence
        std::list<TokenT>::iterator it_tmp = next; 
        
            ++it_tmp;                  // now points behind next
            expanded.erase(next);      // remove next token
            expanded.erase(it);        // remove '##' token
            it = it_tmp;

            found_concat = true;
            prev = it;
            continue;
        }

    // save last non-whitespace token position
        if (T_SPACE != token_id(*it) && T_SPACE2 != token_id(*it))        
            prev = it;
    }
    
// re-scan the sequence, if necessary
    if (found_concat) {
        typename TokenT::string_t str (as_string(expanded));
        std::list<TokenT> rescanned;
        
        iterator_t it = iterator_t(*this, str.begin(), str.end(), 
            (*expanded.begin()).get_position());
        iterator_t end = iterator_t();
        for (/**/; it != end; ++it) 
            rescanned.push_back(*it);
    
        std::swap(expanded, rescanned);
    }
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_CONTEXT_HPP__907485E2_6649_4A87_911B_7F7225F3E5B8__INCLUDED_)
