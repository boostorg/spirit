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

#if !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
#define _CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_

#include <set>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "cpp/unput_queue_iterator.hpp"
#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_defined_grammar.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

namespace {

    // return the string representation of a token sequence
    template <typename ContainerT>
    typename ContainerT::value_type::string_t
    as_string (ContainerT const &token_sequence)
    {
        typename ContainerT::value_type::string_t result;
        ContainerT::const_iterator end = token_sequence.end();
        for (ContainerT::const_iterator it = token_sequence.begin(); 
             it != end; ++it) 
        {
            result += (*it).get_value();
        }
        return result;
    }

    // escape a string literal (insert '\\' before every '\"' and '\\'
    template <typename StringT>
    StringT
    escape_lit(StringT const &value)
    {
        StringT result(value);
        StringT::size_type pos = 0;
        while ((pos = result.find_first_of ("\"\\", pos)) !=
            StringT::size_type(StringT::npos))
        {
            result.insert (pos, 1, '\\');
            pos += 2;
        }
        return result;
    }
    
    // return the string representation of a token sequence
    template <typename ContainerT>
    typename ContainerT::value_type::string_t
    as_stringlit (ContainerT const &token_sequence)
    {
        using namespace cpplexer;
        typedef typename ContainerT::value_type::string_t string_t;
        
        string_t result("\"");
        bool was_whitespace = false;
        ContainerT::const_iterator end = token_sequence.end();
        for (ContainerT::const_iterator it = token_sequence.begin(); 
             it != end; ++it) 
        {
            token_id id = token_id(*it);
            if (IS_CATEGORY(*it, WhiteSpaceTokenType) || T_NEWLINE == id) {
                if (!was_whitespace) {
                // C++ standard 16.3.2.2 [cpp.stringize]
                // Each occurrence of white space between the argument’s 
                // preprocessing tokens becomes a single space character in the 
                // character string literal.
                    result += " ";
                    was_whitespace = true;
                }
            }
            else if (T_STRINGLIT == id || T_CHARLIT == id) {
            // a string literals and character literals have to be escaped
                result += escape_lit((*it).get_value());
                was_whitespace = false;
            }
            else {
                result += (*it).get_value();
                was_whitespace = false;
            }
        }
        result += "\"";
        return result;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  macro_definition
//
//      This class containes all infos for a defined macro. 
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
struct macro_definition {

    typedef std::vector<TokenT> container_t;
    typedef typename std::vector<TokenT>::const_iterator const_iterator_t;
    
    macro_definition(TokenT const &token_, bool has_parameters)
    :   macroname(token_), is_functionlike(has_parameters), 
        replaced_parameters(false)
    {
    }
    // generated copy constructor
    // generated destructor
    // generated assignment operator

    // Replace all occurences of the parameters throughout the macrodefinition
    // with special parameter tokens to simplify later macro replacement.
    // Additionally mark all occurences of the macro name itself throughout
    // the macro definition
    void replace_parameters()
    {
        using namespace cpplexer;
        
        if (!replaced_parameters) {
            container_t::iterator end = macrodefinition.end();
            for (container_t::iterator it = macrodefinition.begin(); 
                 it != end; ++it)
            {
                if (T_IDENTIFIER == token_id(*it)) {
                // may be a parameter to replace
                    container_t::const_iterator cend = macroparameters.end();
                    container_t::const_iterator cit = macroparameters.begin();
                    for (container_t::size_type i = 0; cit != cend; ++cit, ++i) {
                        if ((*it).get_value() == (*cit).get_value()) {
                            (*it).set_token_id(token_id(T_PARAMETERBASE+i));
                            break;
                        }
                    }
                    
                // maybe a recursive call of the macro itself
                    if ((*it).get_value() == macroname.get_value())
                        (*it).set_token_id(T_NONREPLACABLE_IDENTIFIER);
                }
            }
            replaced_parameters = true;     // do it only once
        }
    }

    bool is_function_like() const 
        { return is_functionlike; }
    typename container_t::size_type parameter_count() const 
        { return macroparameters.size(); }
    
    TokenT macroname;
    bool is_functionlike;
    container_t macroparameters;
    container_t macrodefinition;
    bool replaced_parameters;
};

///////////////////////////////////////////////////////////////////////////////
//
//  macromap
// 
//      This class holds all currently defined macros
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
class macromap {

    typedef macromap<TokenT>                        self_t;
    typedef typename TokenT::string_t               string_t;
    typedef macro_definition<TokenT>                macro_definition_t;
    typedef std::map<string_t, macro_definition_t>  defined_macros_t;

    typedef std::vector<TokenT>                     container_t;
    
public:
    BOOST_STATIC_CONSTANT(int, must_continue = 0x1);
    BOOST_STATIC_CONSTANT(int, replaced_tokens = 0x2);
    BOOST_STATIC_CONSTANT(int, must_relex = 0x4);
    
    macromap() {}
    ~macromap() {}

    bool add_macro(TokenT const &name, bool has_parameters, 
        container_t &parameters, container_t &definition);
    bool is_defined(string_t const &name) const;
    bool remove_macro(string_t const &name);
    
    template <typename IteratorT, typename ContainerT>
    int expand_tokensequence(IteratorT &first, IteratorT const &last,
        ContainerT &expanded, bool one_token_only, 
        bool expand_undefined);

protected:
    template <typename IteratorT, typename ContainerT>
    int expand_tokensequence(IteratorT &first, IteratorT const &last,
        ContainerT &expanded, string_t &replaced_name,
        bool one_token_only, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    int expand_tokensequence_worker(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> insert_iter, 
        string_t &replaced_name, bool one_token_only, 
        bool expand_undefined);

    template <typename IteratorT, typename ContainerT, typename SizeT>
    void collect_arguments (std::vector<ContainerT> &arguments, 
        IteratorT &next, IteratorT const &end, 
        SizeT const &parameter_count);

    template <typename IteratorT, typename ContainerT>
    int expand_macro(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> insert_iter, 
        string_t &replaced_name, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    bool resolve_defined(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> insert_iter);

    template <typename ContainerT>
    int concat_tokensequence(ContainerT &expanded);

    template <typename ContainerT>
    void relex_tokensequence(ContainerT &expanded);

    template <typename ContainerT>
    void reset_nonreplacable_identifiers(ContainerT &expanded);

private:
    defined_macros_t defined_macros;
};

///////////////////////////////////////////////////////////////////////////////
// 
//  add_macro(): adds a new macro to the macromap
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
inline bool 
macromap<TokenT>::add_macro(TokenT const &name, bool has_parameters,
    container_t &parameters, container_t &definition)
{
    defined_macros_t::iterator it = defined_macros.find(name.get_value());
    if (it != defined_macros.end()) {
    // redefinition, should not be different
        if ((*it).second.is_function_like() != has_parameters ||
            (*it).second.macroparameters != parameters ||
            (*it).second.macrodefinition != definition)
        {
            CPP_THROW(preprocess_exception, macro_redefinition, 
                name.get_value(), name);
        }
        return true;
    }

// insert a new macro node
    std::pair<defined_macros_t::iterator, bool> p = 
        defined_macros.insert(defined_macros_t::value_type(
            name.get_value(), macro_definition<TokenT>(name, has_parameters)));
            
    if (!p.second){
        CPP_THROW(preprocess_exception, macro_insertion_error, 
            name.get_value(), name);
    }

// add the parameters and the definition
    std::swap((*p.first).second.macroparameters, parameters);
    std::swap((*p.first).second.macrodefinition, definition);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// 
//  is_defined(): returnes if a given macro is already defined
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
inline bool 
macromap<TokenT>::is_defined(typename TokenT::string_t const &name) const
{
    return defined_macros.find(name) != defined_macros.end();
}

///////////////////////////////////////////////////////////////////////////////
// 
//  remove_macro(): remove a macro from the macromap
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
inline bool 
macromap<TokenT>::remove_macro(typename TokenT::string_t const &name)
{
    defined_macros_t::iterator it = defined_macros.find(name);
    if (it != defined_macros.end()) {
        defined_macros.erase(it);
        return true;
    }
    return false;       // macro was not defined
}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_tokensequence(): expands a sequence of tokens
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int 
macromap<TokenT>::expand_tokensequence(IteratorT &first, IteratorT const &last,
    ContainerT &expanded, bool one_token_only, bool expand_undefined)
{
    string_t replaced_name;
    int return_flags = expand_tokensequence(first, last, expanded, replaced_name, 
        one_token_only, expand_undefined);
    reset_nonreplacable_identifiers(expanded);        
    return return_flags;
}

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int 
macromap<TokenT>::expand_tokensequence(IteratorT &first, IteratorT const &last,
    ContainerT &expanded, string_t &replaced_name,
    bool one_token_only, bool expand_undefined)
{
    int return_flags = 0;
    int rescan_flags = 0;
    
    if ((return_flags = expand_tokensequence_worker(first, last, 
        std::inserter(expanded, expanded.begin()), replaced_name, 
        one_token_only, expand_undefined)) & must_continue)
    {
    // re-scanning is required
        typedef impl::gen_unput_queue_iterator<IteratorT, TokenT> gen_t;

    int replaced_flags = 0;
    ContainerT eof_queue;
    ContainerT expanded_list;
    
        rescan_flags |= concat_tokensequence(expanded);     // handle '##'

        typename gen_t::return_t first_it = gen_t::do_(expanded, first);
//        string_t replaced_name_next;
        
        replaced_flags = expand_tokensequence_worker(
            first_it,  gen_t::do_(eof_queue, last),
            std::inserter(expanded_list, expanded_list.begin()),
            replaced_name, one_token_only, expand_undefined);
        return_flags |= replaced_flags;

        first = first_it.base();    // adjust iterator

        if (replaced_flags & replaced_tokens)
            std::swap(expanded, expanded_list);
    }
    rescan_flags |= concat_tokensequence(expanded);         // handle '##'

// there were '##' operators while expanding macros, so we need to re-lex
// the whole token sequence to rebuild the C++ tokens
    if (must_relex & rescan_flags)
        relex_tokensequence(expanded);

    return return_flags;
}

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int
macromap<TokenT>::expand_tokensequence_worker(IteratorT &first, IteratorT const &last, 
    std::insert_iterator<ContainerT> insert_iter, string_t &replaced_name, 
    bool one_token_only, bool expand_undefined)
{
    using namespace cpplexer;
    
//  Iterate over all the elements of the given sequence, if it is an 
//  T_IDENTIFIER token, try to replace this as a macro
int return_flags = 0;       // return, whether a replacement occured

    while(first != last) {
    token_id id = token_id(*first);
    
        if (T_DEFINED == id) {
        // resolve operator defined()
            resolve_defined(first, last, insert_iter);

        // expand only the current macro, if requested
            if (one_token_only && impl::is_fresh_token<IteratorT>::test_(first))
                break;
            continue;           // 'it' is already adjusted correctly
        }
        else if (T_IDENTIFIER == id) {
        // try to replace this identifier as a macro
            if (is_defined((*first).get_value())) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
                return_flags = expand_macro(first, last, insert_iter, 
                    replaced_name, expand_undefined) | return_flags;

            // expand only the current macro, if requested
                if (one_token_only && impl::is_fresh_token<IteratorT>::test_(first))
                    break;
                continue;       // 'it' is already adjusted correctly
            }
            else if (expand_undefined) {
            // in preprocessing conditionals undefined identifiers and keywords 
            // are to be replaced with '0' (see. C++ standard 16.1.4)
            TokenT new_token (*first);
            
                new_token.set_token_id(T_INTLIT);
                new_token.set_value("0");
                *insert_iter = new_token;
                return_flags |= (replaced_tokens | must_continue);
            }
            else {
                *insert_iter = *first;
                return_flags |= replaced_tokens;
            }
        }
        else if (expand_undefined && IS_CATEGORY(*first, KeywordTokenType) && 
                 T_TRUE != id && T_FALSE != id)
        {
        // all remaining identifiers and keywords, except for true and false, 
        // are replaced with the pp-number 0 (C++ standard 16.1.4, [cpp.cond])
        TokenT new_token (*first);
        
            new_token.set_token_id(T_INTLIT);
            new_token.set_value("0");
            *insert_iter = new_token;
            return_flags |= (replaced_tokens | must_continue);
        }
        else {
            *insert_iter = *first;
            return_flags |= replaced_tokens;
        }
        ++first;
        
    // expand only the current macro, if requested
        if (one_token_only && impl::is_fresh_token<IteratorT>::test_(first))
            break;
    }
    return return_flags;
}

///////////////////////////////////////////////////////////////////////////////
// 
//  collect_arguments(): collect the actual arguments of a macro invocation
//
///////////////////////////////////////////////////////////////////////////////

namespace {

    template <typename ContainerT>
    inline void
    trim_argument (ContainerT *argument)
    {
        using namespace cpplexer;
        
    // strip leading whitespace (should be only one token)
        if (argument->size() > 0 &&
            IS_CATEGORY((argument->front()), WhiteSpaceTokenType))
        {
            argument->pop_front();
        }
        
    // strip trailing whitespace (should be only one token)
        if (argument->size() > 0 &&
            IS_CATEGORY((argument->back()), WhiteSpaceTokenType))
        {
            argument->pop_back();
        }
    }
}

template <typename TokenT>
template <typename IteratorT, typename ContainerT, typename SizeT>
inline void 
macromap<TokenT>::collect_arguments (
    std::vector<ContainerT> &arguments, IteratorT &next, IteratorT const &end, 
    SizeT const &parameter_count)
{
    using namespace cpplexer;
    
    arguments.push_back(ContainerT());
    
// collect the actual arguments
int nested_parenthesis_level = 1;
ContainerT *argument = &arguments[0];
bool was_whitespace = false;

    while (++next != end && nested_parenthesis_level) {
    token_id id = token_id(*next);
    
        if (0 == parameter_count && 
            !IS_CATEGORY((*next), WhiteSpaceTokenType) &&
            id != T_RIGHTPAREN && id != T_LEFTPAREN) 
        {
        // there shouldn't be any arguments
            CPP_THROW(preprocess_exception, too_many_macroarguments, 
                "", (*next));
        }
        
        switch (id) {
        case T_LEFTPAREN:
            ++nested_parenthesis_level;
            argument->push_back(*next);
            was_whitespace = false;
            break;
            
        case T_RIGHTPAREN:
            if (--nested_parenthesis_level >= 1)
                argument->push_back(*next);
            else
                trim_argument(argument);
            was_whitespace = false;
            break;
        
        case T_COMMA:
            if (1 == nested_parenthesis_level) {
            // next parameter
                if (arguments.size() == parameter_count) {
                // too many macro arguments
                    CPP_THROW(preprocess_exception, too_many_macroarguments, 
                        "", (*next));
                }
                trim_argument(argument);
                arguments.push_back(ContainerT()); // add new arg
                argument = &arguments[arguments.size()-1];
            }
            else {
            // surrounded by parenthesises, so store to current argument
                argument->push_back(*next);
            }
            was_whitespace = false;
            break;

        case T_SPACE:
        case T_SPACE2:
        case T_CCOMMENT:
            if (!was_whitespace) 
                argument->push_back(TokenT(T_SPACE, " ", (*next).get_position()));
            was_whitespace = true;
            break;      // skip whitespace
            
        default:
            argument->push_back(*next);
            was_whitespace = false;
            break;
        }
    }

    if (nested_parenthesis_level > 1) {
    // missing ')': improperly terminated macro invocation
        CPP_THROW(preprocess_exception, improperly_terminated_macro, 
            "missing ')'", (*next));
    }
}        

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_macro(): expands a defined macro
//
//      This functions tries to expand the macro, to which points the 'first'
//      iterator. The functions eats up more tokens, if the macro to expand is
//      a function-like macro.
//
///////////////////////////////////////////////////////////////////////////////
namespace {

    using namespace cpplexer;
    
    template <typename IteratorT>
    bool next_token_is(IteratorT it, IteratorT const &end, token_id id)
    {
        if (++it == end) return false;
        while (T_SPACE == token_id(*it) || T_SPACE2 == token_id(*it)) {
            if (++it == end)
                return false;
        }
        return (token_id(*it) == id) ? true : false;
    }

    template <typename IteratorT>
    bool skip_to_token(IteratorT &it, IteratorT const &end, token_id id)
    {
        if (token_id(*it) == id) return true;
        if (++it == end) return false;
        while (T_SPACE == token_id(*it) || T_SPACE2 == token_id(*it)) {
            if (++it == end)
                return false;
        }
        BOOST_SPIRIT_ASSERT(token_id(*it) == id);
        return true;
    }
}

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int 
macromap<TokenT>::expand_macro(IteratorT &first, IteratorT const &last, 
    std::insert_iterator<ContainerT> insert_iter, 
    string_t &replaced_name, bool expand_undefined) 
{
    using namespace cpplexer;
    
    defined_macros_t::iterator it = defined_macros.find((*first).get_value());
    if (it == defined_macros.end()) 
        return 0;

// ensure the parameters to be replaced with special parameter tokens
    (*it).second.replace_parameters();

// 
TokenT act_token = *first;
bool no_expansion = replaced_name == act_token.get_value();

    if (!no_expansion) {
    // save the macro name, to avoid expansion of this macro during replacement
        if (0 == replaced_name.size()) 
            replaced_name = act_token.get_value();
    }
    else {
    // do not expand this instance of the actual macro 
        *insert_iter = act_token;      // copy macroname itself
    }

    if (next_token_is(first, last, T_LEFTPAREN)) {
    // called as a function-like macro 
        skip_to_token(first, last, T_LEFTPAREN);
        if ((*it).second.is_function_like()) {
        // defined as a function-like macro
            if (no_expansion) {
            // do not expand this macro
            
            // copy the arguments
            int nested_parenthesis_level = 0;
            
                while (first != last) {
                    switch (token_id(*first)) {
                    case T_LEFTPAREN:
                        ++nested_parenthesis_level;
                        break;
                        
                    case T_RIGHTPAREN:
                        --nested_parenthesis_level;
                        break;
                    }
                    
                    *insert_iter = *first;
                    ++first;
                    if (0 == nested_parenthesis_level)
                        break;
                }
            }
            else {
            // expand this macro
            
            // collect the arguments
            std::vector<ContainerT> arguments;
            
                collect_arguments (arguments, first, last, 
                    (*it).second.parameter_count());
                if (arguments.size() < (*it).second.parameter_count()) {
                // too many macro arguments
                    CPP_THROW(preprocess_exception, too_few_macroarguments, 
                        "", act_token);
                }

            // try to expand the arguments itself
            std::vector<ContainerT> expanded_args(arguments.size());
            std::vector<ContainerT>::size_type i = 0;
            
                std::vector<ContainerT>::iterator arg_end = arguments.end();
                for (std::vector<ContainerT>::iterator arg_it = arguments.begin(); 
                    arg_it != arg_end; ++arg_it, ++i)
                {
                    string_t arg_replaced;
                    typename ContainerT::iterator begin = (*arg_it).begin();
                    expand_tokensequence(begin, (*arg_it).end(),
                        expanded_args[i], arg_replaced, false, expand_undefined);
                }
                        
            // replace macro
                typedef typename macro_definition_t::const_iterator_t 
                    macro_definition_iter_t;

                bool adjacent_concat = false;
                bool adjacent_stringize = false;
                macro_definition_iter_t cend = (*it).second.macrodefinition.end();
                for (macro_definition_iter_t cit = (*it).second.macrodefinition.begin();
                    cit != cend; ++cit)
                {
                bool use_replaced_arg = true;
                token_id id = token_id(*cit);
                
                    if (T_POUND_POUND == id) {
                    // conactination operator
                        adjacent_concat = true;
                    }
                    else if (T_POUND == id) {
                    // stringize operator
                        adjacent_stringize = true;
                    }
                    else {
                        if (adjacent_stringize || adjacent_concat || 
                            next_token_is(cit, cend, T_POUND_POUND))
                        {
                            use_replaced_arg = false;
                        }
                        if (adjacent_concat)    // spaces after '##' ?
                            adjacent_concat = (T_SPACE == id || T_SPACE2 == id);
                    }

                    if (IS_CATEGORY((*cit), ParameterTokenType)) {
                    // copy argument 'i' instead of the parameter token i
                    std::vector<TokenT>::size_type i = token_id(*cit) - T_PARAMETERBASE;
                    
                        if (use_replaced_arg && expanded_args[i].size() > 0) {
                        // replace argument
                        ContainerT &arg = expanded_args[i];
                        
                            std::copy(arg.begin(), arg.end(), insert_iter);
                        }
                        else if (adjacent_stringize && 
                                !(T_SPACE == id || T_SPACE2 == id)) 
                        {
                        // stringize the current argument
                            BOOST_SPIRIT_ASSERT(arguments[i].size() > 0);
                            *insert_iter = TokenT(T_STRINGLIT, 
                                as_stringlit(arguments[i]), 
                                (*arguments[i].begin()).get_position());
                            adjacent_stringize = false;
                        }
                        else {
                        ContainerT &arg = arguments[i];
                        
                            std::copy(arg.begin(), arg.end(), insert_iter);
                        }
                    }
                    else {
                    // insert the actual replacement token
                        *insert_iter = *cit;
                    }
                }
            }
        }
        else {
        // defined as an object-like macro
            if (!no_expansion) {
                std::copy ((*it).second.macrodefinition.begin(), 
                    (*it).second.macrodefinition.end(), insert_iter);
            }
        }
    }
    else {
        ++first;                // skip macro name
        if (!no_expansion) {
            if ((*it).second.is_function_like()) {
            // defined as a function-like macro
                *insert_iter = act_token;
                return replaced_tokens;
            }
            else {
            // defined as an object-like macro
                std::copy ((*it).second.macrodefinition.begin(), 
                    (*it).second.macrodefinition.end(), insert_iter);
            }
        }
    }
    return replaced_tokens | (no_expansion ? 0 : must_continue);
}

///////////////////////////////////////////////////////////////////////////////
//
//  resolve_defined(): resolve the operator defined() and replace it with the 
//                     correct T_INTLIT token
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline bool 
macromap<TokenT>::resolve_defined(
    IteratorT &first, IteratorT const &last,
    std::insert_iterator<ContainerT> insert_iter) 
{
    using namespace cpplexer;
    
TokenT result;
cpp::defined_grammar<TokenT> g;
parse_info<IteratorT> hit =
    parse(first, last, g[assign(result)], ch_p(T_SPACE) | ch_p(T_CCOMMENT));

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            "defined()", (*first));
    }
    impl::assign_iterator<IteratorT>::do_(first, hit.stop);

// insert token refelcting outcome
bool isdefined = is_defined(result.get_value());
TokenT new_token(T_INTLIT, isdefined ? "1" : "0", result.get_position());

    *insert_iter = new_token;
    return true;
}

template <typename TokenT>
template <typename ContainerT>
inline int 
macromap<TokenT>::concat_tokensequence(ContainerT &expanded)
{
    using namespace cpplexer;
    
bool found_concat = false;

    typename ContainerT::iterator end = expanded.end();
    typename ContainerT::iterator prev = end;
    for (typename ContainerT::iterator it = expanded.begin(); it != end; ++it) 
    {
        token_id id = token_id(*it);
        if (T_POUND_POUND == id) {
        typename ContainerT::iterator next = it;
    
            ++next;
            if (prev == end || next == end) {
            // error, '##' should be in between two tokens
                CPP_THROW(preprocess_exception, ill_formed_operator,
                    "concat ('##')", (*it));
            }

        // replace prev##next with the concatenated value, skip whitespace
        // before and after the '##' operator
            while (T_SPACE == token_id(*next) || T_SPACE2 == token_id(*next)) {
            typename ContainerT::iterator temp_it = next;
            
                ++next;
                if (next == end) {
                // error, '##' should be in between two tokens
                    CPP_THROW(preprocess_exception, ill_formed_operator,
                        "concat ('##')", (*temp_it));
                }
            }
            
        // we leave the token_id unchanged, because the whole sequence is to be
        // re-lexed anyway
            (*prev).set_value((*prev).get_value() + (*next).get_value());

        // remove the '##' and the next tokens from the sequence
            expanded.erase(++prev, ++next);     // remove not needed tokens
            prev = it = next;
            found_concat = true;
            continue;
        }
        else if (T_POUND == id) {
        // handle stringize operator (remove this token)
        typename ContainerT::iterator pound_it = it;

            ++it;
            if (pound_it == end) {
            // error, '#' should not be the last token
                CPP_THROW(preprocess_exception, ill_formed_operator,
                    "stringize ('#')", (*pound_it));
            }
            expanded.erase(pound_it);           
        }

    // save last non-whitespace token position
        if (T_SPACE != token_id(*it) && T_SPACE2 != token_id(*it))        
            prev = it;
    }
    return found_concat ? must_relex : 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  re-lex the sequence, if necessary
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename ContainerT>
inline void 
macromap<TokenT>::relex_tokensequence(ContainerT &expanded)
{
    typedef cpplexer::lex_iterator<TokenT> lex_t;
    
    typename TokenT::string_t str (as_string(expanded));
    ContainerT rescanned;
    
    lex_t it = lex_t(str.begin(), str.end(), 
        (*expanded.begin()).get_position());
    lex_t end = lex_t();
    for (/**/; it != end; ++it) 
        rescanned.push_back(*it);

    std::swap(expanded, rescanned);
}

///////////////////////////////////////////////////////////////////////////////
//
//  reset_nonreplacable_identifiers() 
//      reset the T_NONREPLACABLE_IDENTIFIER token_id's
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename ContainerT>
inline void 
macromap<TokenT>::reset_nonreplacable_identifiers(ContainerT &expanded)
{
    typename ContainerT::iterator end = expanded.end();
    for (typename ContainerT::iterator it = expanded.begin(); it != end; ++it) 
    {
        if (T_NONREPLACABLE_IDENTIFIER == token_id(*it))
            (*it).set_token_id(T_IDENTIFIER);
    }
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
