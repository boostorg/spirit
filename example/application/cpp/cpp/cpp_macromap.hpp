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

#include "cpp/cpp_exceptions.hpp"

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
    
    macro_definition(TokenT const &token_)
    :   macroname(token_), replaced_parameters(false)
    {
    }
    // generated copy constructor
    // generated destructor
    // generated assignment operator

    bool replace_parameters()
    {
        if (macroparameters.size() > 0 && !replaced_parameters) {
            container_t::iterator end = macrodefinition.end();
            for (container_t::iterator it = macrodefinition.begin(); 
                 it != end; ++it)
            {
                if (T_IDENTIFIER == ID_FROM_TOKEN(token_id(*it))) {
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
    
    TokenT macroname;
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

    typedef typename TokenT::string_t               string_t;
    typedef macro_definition<TokenT>                macro_definition_t;
    typedef std::map<string_t, macro_definition_t>  defined_macros_t;

    typedef std::vector<TokenT>                     container_t;
    
public:
    macromap() {}
    ~macromap() {}

    bool add_macro(TokenT const &name, container_t &parameters, 
        container_t &definition);
    bool is_defined(string_t const &name) const;
    bool remove_macro(string_t const &name);
    
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
macromap<TokenT>::add_macro(TokenT const &name, container_t &parameters, 
    container_t &definition)
{
    defined_macros_t::iterator it = defined_macros.find(name.get_value());
    if (it != defined_macros.end()) {
    // redefinition, should not be different
        if ((*it).second.macroparameters != parameters ||
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
            name.get_value(), macro_definition<TokenT>(name)));
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
//  add_macro(): remove a macro from the macromap
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
}   // namespace cpp

#endif // !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
