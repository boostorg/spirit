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

    macromap() {}
    ~macromap() {}

    bool add_macro(TokenT const &name, bool has_parameters, 
        container_t &parameters, container_t &definition);
    bool is_defined(string_t const &name) const;
    bool remove_macro(string_t const &name);
    
    template <typename ContainerT>
    bool expand_macro(ContainerT &sequence, 
        typename ContainerT::iterator &first);
    template <typename ContainerT>
    bool resolve_defined(ContainerT &sequence, 
        typename ContainerT::iterator &first,
        typename ContainerT::iterator &last);
    
protected:
    template <typename IteratorT, typename SizeT>
    void collect_arguments (std::vector<std::vector<TokenT> > &arguments, 
        IteratorT &next, IteratorT const &end, SizeT const &parameter_count);

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
template <typename IteratorT, typename SizeT>
inline void 
macromap<TokenT>::collect_arguments (
    std::vector<std::vector<TokenT> > &arguments, IteratorT &next, 
        IteratorT const &end, SizeT const &parameter_count)
{
    using namespace cpplexer;
    
    arguments.push_back(std::vector<TokenT>());
    
// collect the actual arguments
int nested_parenthesis_level = 1;
std::vector<TokenT> *argument = &arguments[0];
    
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
            if (--nested_parenthesis_level > 1)
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
                arguments.push_back(std::vector<TokenT>()); // add new arg
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
template <typename ContainerT>
inline bool 
macromap<TokenT>::expand_macro(ContainerT &sequence, 
    typename ContainerT::iterator &first) 
{
    using namespace cpplexer;
    
    defined_macros_t::iterator it = defined_macros.find((*first).get_value());
    if (it == defined_macros.end()) 
        return false;

// ensure the parameters to be replaced with special parameter tokens
    (*it).second.replace_parameters();

typename ContainerT::iterator next = first;
std::insert_iterator<ContainerT> insert_iter (sequence, first);

    if (T_LEFTPAREN == *++next) {
    // called as a function-like macro 
        if ((*it).second.is_function_like()) {
        // defined as a function-like macro
        std::vector<std::vector<TokenT> > arguments;
        
            collect_arguments (arguments, next, sequence.end(), 
                (*it).second.parameter_count());
            if (arguments.size() < (*it).second.parameter_count()) {
            // too many macro arguments
                CPP_THROW(preprocess_exception, too_few_macroarguments, 
                    "", (*next));
            }
            
        // replace macro
            typedef 
                typename macro_definition_t::const_iterator_t 
                macro_definition_iter_t;
                
            macro_definition_iter_t cend = (*it).second.macrodefinition.end();
            for (macro_definition_iter_t cit = (*it).second.macrodefinition.begin();
                cit != cend; ++cit)
            {
                if (IS_CATEGORY((*cit), ParameterTokenType)) {
                // copy argument i instead of the parameter token i
                std::vector<TokenT> &arg = 
                    arguments[token_id(*cit) - T_PARAMETERBASE];
                
                    std::copy(arg.begin(), arg.end(), insert_iter);
                }
                else {
                // insert the actual replacement token
                    *insert_iter = *cit;
                }
            }
            sequence.erase(first, next);    // erase macro invocation
        }
        else {
        // defined as an object-like macro: no replacement at all
            first = next;
            return true;
        }
    }
    else {
    // defined as an object-like macro
        std::copy ((*it).second.macrodefinition.begin(), 
            (*it).second.macrodefinition.end(), insert_iter);
        sequence.erase(first);      // remove the current token
    }
    
    first = next;                   // next token position to be used
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  resolve_defined(): resolve the operator defined() and replace it with the 
//                     correct T_INTLIT token
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
template <typename ContainerT>
inline bool 
macromap<TokenT>::resolve_defined(ContainerT &sequence, 
    typename ContainerT::iterator &first,
    typename ContainerT::iterator &last) 
{
    using namespace cpplexer;
    
TokenT result;
cpp::defined_grammar<TokenT> g;
parse_info<typename ContainerT::iterator> hit =
    parse(first, last, g[assign(result)], ch_p(T_SPACE) | ch_p(T_CCOMMENT));

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            "defined()", (*first));
    }

// erase defined()
    sequence.erase(first, hit.stop);
    first = hit.stop;

// insert token refelcting outcome
bool isdefined = is_defined(result.get_value());
TokenT new_token(T_INTLIT, isdefined ? "1" : "0", result.get_position());

    sequence.insert(first, new_token);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
