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

#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_defined_grammar.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

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

    // replace all occurences of the parameters throughout the macrodefinition
    // with special parameter tokens to simplify later macro replacement
    void replace_parameters()
    {
        using namespace cpplexer;
        
        if (macroparameters.size() > 0 && !replaced_parameters) {
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
    
    macromap() {}
    ~macromap() {}

    bool add_macro(TokenT const &name, bool has_parameters, 
        container_t &parameters, container_t &definition);
    bool is_defined(string_t const &name) const;
    bool remove_macro(string_t const &name);
    
    template <typename IteratorT, typename ContainerT>
    int expand_tokensequence(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> &insert_iter, 
        std::set<string_t> &replaced, bool one_token_only, bool expand_undefined);

protected:
    template <typename IteratorT, typename ContainerT, typename SizeT>
    void collect_arguments (std::vector<ContainerT> &arguments, 
        IteratorT &next, IteratorT const &end, 
        SizeT const &parameter_count);

    template <typename IteratorT, typename ContainerT>
    int expand_macro(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> &insert_iter, 
        std::set<string_t> &replaced, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    bool resolve_defined(IteratorT &first, IteratorT const &last,
        std::insert_iterator<ContainerT> &insert_iter);
    
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
//  expand_macro(): expandes a defined macro
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int
macromap<TokenT>::expand_tokensequence(IteratorT &first, IteratorT const &last, 
    std::insert_iterator<ContainerT> &insert_iter, std::set<string_t> &replaced, 
    bool one_token_only, bool expand_undefined)
{
    using namespace cpplexer;
    
//  Iterate over all the elements of the given sequence, if it is an 
//  T_IDENTIFIER token, try to replace this as a macro
int return_replaced = 0;       // return, whether a replacement occured

    while(first != last) {
    token_id id = token_id(*first);
    
        if (T_DEFINED == id) {
        // resolve operator defined()
            resolve_defined(first, last, insert_iter);

        // expand one token only if requested
            if (one_token_only)
                break;
            continue;           // 'it' is already adjusted correctly
        }
        else if (T_IDENTIFIER == id) {
        // try to replace this identifier as a macro
            if (is_defined((*first).get_value())) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
                return_replaced = expand_macro(first, last, insert_iter, 
                    replaced, expand_undefined) | return_replaced;

            // expand one token only if requested
                if (one_token_only)
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
                return_replaced |= (replaced_tokens | must_continue);
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
            return_replaced |= (replaced_tokens | must_continue);
        }
        else {
            *insert_iter = *first;
            return_replaced |= replaced_tokens;
        }
        ++first;
        
    // expand one token only if requested
        if (one_token_only)
            break;
    }
    return return_replaced;
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
    
    while (++next != end && nested_parenthesis_level) {
        if (0 == parameter_count && !IS_CATEGORY((*next), WhiteSpaceTokenType)) {
        // there shouldn't be any arguments
            CPP_THROW(preprocess_exception, too_many_macroarguments, 
                "", (*next));
        }
        
        switch (token_id(*next)) {
        case T_LEFTPAREN:
            ++nested_parenthesis_level;
            argument->push_back(*next);
            break;
            
        case T_RIGHTPAREN:
            if (--nested_parenthesis_level >= 1)
                argument->push_back(*next);
            break;
        
        case T_COMMA:
            if (1 == nested_parenthesis_level) {
            // next parameter
                if (arguments.size() == parameter_count) {
                // too many macro arguments
                    CPP_THROW(preprocess_exception, too_many_macroarguments, 
                        "", (*next));
                }
                arguments.push_back(ContainerT()); // add new arg
                argument = &arguments[arguments.size()-1];
            }
            else {
            // surrounded by parenthesises, so store to current argument
                argument->push_back(*next);
            }
            break;

        case T_SPACE:
        case T_SPACE2:
        case T_CCOMMENT:
            break;      // skip whitespace
            
        default:
            argument->push_back(*next);
            break;
        }
    }

    if (nested_parenthesis_level > 1) {
    // missing ')': improperly terminated macro invocation
        CPP_THROW(preprocess_exception, improperly_terminated_macro, 
            "missing ')'", (*next));
    }
}        

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline int 
macromap<TokenT>::expand_macro(IteratorT &first, IteratorT const &last, 
    std::insert_iterator<ContainerT> &insert_iter, 
    std::set<string_t> &replaced, bool expand_undefined) 
{
    using namespace cpplexer;
    
    defined_macros_t::iterator it = defined_macros.find((*first).get_value());
    if (it == defined_macros.end()) 
        return 0;

// ensure the parameters to be replaced with special parameter tokens
    (*it).second.replace_parameters();

// 
TokenT act_token = *first;
std::set<string_t>::const_iterator it_replaced = 
    replaced.find(act_token.get_value());
bool no_expansion = it_replaced != replaced.end();;

    if (!no_expansion)  // do not expand the actual macro 
        replaced.insert(std::set<string_t>::value_type(act_token.get_value()));
    else
        *insert_iter = act_token;      // copy macroname itself

//    IteratorT next = first;
    if (T_LEFTPAREN == *++first) {
    // called as a function-like macro 
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
                std::vector<ContainerT>::iterator arg_end = arguments.end();
                for (std::vector<ContainerT>::iterator arg_it = arguments.begin(); 
                    arg_it != arg_end; ++arg_it)
                {
                    //std::set<string_t> replaced;
                    ContainerT expanded_arg;
                    if (replaced_tokens & expand_tokensequence(
                            (*arg_it).begin(), (*arg_it).end(),
                            std::inserter(expanded_arg, expanded_arg.begin()),
                            replaced, false, expand_undefined))
                    {
                        std::swap(*arg_it, expanded_arg);
                    }
                }
                        
            // replace macro
                typedef typename macro_definition_t::const_iterator_t 
                    macro_definition_iter_t;
                    
                macro_definition_iter_t cend = (*it).second.macrodefinition.end();
                for (macro_definition_iter_t cit = (*it).second.macrodefinition.begin();
                    cit != cend; ++cit)
                {
                    if (IS_CATEGORY((*cit), ParameterTokenType)) {
                    // copy argument 'i' instead of the parameter token i
                    ContainerT &arg = arguments[token_id(*cit) - T_PARAMETERBASE];
                    
                        std::copy(arg.begin(), arg.end(), insert_iter);
                    }
                    else {
                    // insert the actual replacement token
                        *insert_iter = *cit;
                    }
                }
            }
        }
        else {
        // defined as an object-like macro: no replacement at all
            return no_expansion ? replaced_tokens : 0;
        }
    }
    else {
    // defined as an object-like macro
        if (!no_expansion) {
            std::copy ((*it).second.macrodefinition.begin(), 
                (*it).second.macrodefinition.end(), insert_iter);
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
    std::insert_iterator<ContainerT> &insert_iter) 
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

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
