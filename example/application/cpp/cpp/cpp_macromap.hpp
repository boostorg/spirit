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

#include <cstdlib>
#include <ctime>

#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "util/time_conversion_helper.hpp"
#include "cpplexer/validate_universal_char.hpp"
#include "cpp/insert_whitespace_detection.hpp"
#include "cpp/unput_queue_iterator.hpp"
#include "cpp/cpp_exceptions.hpp"
#include "cpp/cpp_defined_grammar.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace util {

namespace impl {

    // escape a string literal (insert '\\' before every '\"' and '\\'
    template <typename StringT>
    inline StringT
    escape_lit(StringT const &value)
    {
        StringT result(value);
        typename StringT::size_type pos = 0;
        while ((pos = result.find_first_of ("\"\\?", pos)) != StringT::npos)
        {
            result.insert (pos, 1, '\\');
            pos += 2;
        }
        return result;
    }
    
    // return the string representation of a token sequence
    template <typename ContainerT, typename PositionT>
    inline typename ContainerT::value_type::string_t
    as_stringlit (ContainerT const &token_sequence, PositionT const &pos)
    {
        using namespace cpplexer;
        typedef typename ContainerT::value_type::string_t string_t;
        util::insert_whitespace_detection whitespace;
        
        string_t result("\"");
        bool was_whitespace = false;
        typename ContainerT::const_iterator end = token_sequence.end();
        for (typename ContainerT::const_iterator it = token_sequence.begin(); 
             it != end; ++it) 
        {
            token_id id = token_id(*it);
            bool must_insert_whitespace = 
                whitespace.must_insert(id, (*it).get_value());
            
            if (IS_CATEGORY(*it, WhiteSpaceTokenType) || T_NEWLINE == id) {
                if (!was_whitespace) {
                // C++ standard 16.3.2.2 [cpp.stringize]
                // Each occurrence of white space between the argument’s 
                // preprocessing tokens becomes a single space character in the 
                // character string literal.
                    result += " ";
                    whitespace.shift_tokens(T_SPACE);
                    was_whitespace = true;
                }
            }
            else if (T_STRINGLIT == id || T_CHARLIT == id) {
                if (must_insert_whitespace) {
                // insert additional whitespace if appropriate
                    whitespace.shift_tokens(T_SPACE);
                    result += " ";
                }
                
            // string literals and character literals have to be escaped
                result += impl::escape_lit((*it).get_value());
                whitespace.shift_tokens(id);
                was_whitespace = false;
            }
            else {
                if (must_insert_whitespace) {
                // insert additional whitespace if appropriate
                    whitespace.shift_tokens(T_SPACE);
                    result += " ";
                }

            // now append this token to the string
                whitespace.shift_tokens(id);
                result += (*it).get_value();
                was_whitespace = false;
            }
        }
        result += "\"";

    // validate the resulting literal to contain no invalid universal character
    // value (throws if invalid chars found)
        cpplexer::impl::validate_literal(result, pos.line, pos.column, pos.file); 
        return result;
    }

}   // namespace impl

namespace on_exit {

    template <typename TokenT>
    struct pop_front {

        pop_front(std::list<TokenT> &list_) : list(list_) {}
        ~pop_front() { list.pop_front(); }
        
        std::list<TokenT> &list;
    };
    
    template <typename TypeT>
    struct reset {
    
        reset(TypeT &target_value_, TypeT new_value)
        :   target_value(target_value_), old_value(target_value_)
        {
            target_value_ = new_value;
        }
        ~reset() { target_value = old_value; }
        
        TypeT &target_value;
        TypeT old_value;
    };

    template <typename IteratorT, typename UnputIteratorT>
    struct assign {

        assign(IteratorT &it_, UnputIteratorT const &uit_) 
        :   it(it_), uit(uit_) {}
        ~assign() { it = uit.base(); }
        
        IteratorT &it;
        UnputIteratorT const &uit;
    };

}   // namespace on_exit

///////////////////////////////////////////////////////////////////////////////
//
//  macro_definition
//
//      This class containes all infos for a defined macro. 
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
struct macro_definition {

    typedef std::vector<TokenT> parameter_container_t;
    typedef 
        typename parameter_container_t::const_iterator 
        const_parameter_iterator_t;
    typedef std::list<TokenT> definition_container_t;
    typedef 
        typename definition_container_t::const_iterator 
        const_definition_iterator_t;
    
    macro_definition(TokenT const &token_, bool has_parameters, 
            bool is_predefined_ = false)
    :   macroname(token_), is_functionlike(has_parameters), 
        replaced_parameters(false), is_available_for_replacement(true),
        is_predefined(is_predefined_)
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
        typename definition_container_t::iterator end = macrodefinition.end();
        typename definition_container_t::iterator it = macrodefinition.begin(); 

            for (/**/; it != end; ++it) {
                if (T_IDENTIFIER == token_id(*it)) {
                // may be a parameter to replace
                    const_parameter_iterator_t cend = macroparameters.end();
                    const_parameter_iterator_t cit = macroparameters.begin();
                    for (typename parameter_container_t::size_type i = 0; 
                         cit != cend; ++cit, ++i) 
                    {
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

    TokenT macroname;
    parameter_container_t macroparameters;
    definition_container_t macrodefinition;
    bool is_functionlike;
    bool replaced_parameters;
    bool is_available_for_replacement;
    bool is_predefined;
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

    typedef std::vector<TokenT>                     parameter_container_t;
    typedef std::list<TokenT>                       definition_container_t;
    
public:
    macromap() {}
    ~macromap() {}

    bool add_macro(TokenT const &name, bool has_parameters, 
        parameter_container_t &parameters, definition_container_t &definition,
        bool is_predefined = false);
    bool is_defined(string_t const &name) const;
    bool remove_macro(TokenT const &name);
    
    template <typename IteratorT, typename ContainerT>
    TokenT const &expand_tokensequence(IteratorT &first, IteratorT const &last,
        ContainerT &pending, ContainerT &expanded, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    void expand_whole_tokensequence(ContainerT &expanded, 
        IteratorT const &first, IteratorT const &last, bool expand_undefined);

    void init_predefined_macros();
    void reset_macromap();

    TokenT &get_main_token() { return main_token; }
    
protected:
    template <typename IteratorT, typename ContainerT>
    TokenT const &expand_tokensequence_worker(
        ContainerT &pending, 
        unput_queue_iterator<IteratorT, TokenT, ContainerT> &first, 
        unput_queue_iterator<IteratorT, TokenT, ContainerT> const &last, 
        bool expand_undefined);

    template <typename IteratorT, typename ContainerT, typename SizeT>
    void collect_arguments (std::vector<ContainerT> &arguments, 
        IteratorT &next, IteratorT const &end, 
        SizeT const &parameter_count);

    template <typename IteratorT, typename ContainerT>
    bool expand_macro(ContainerT &pending, IteratorT &first, 
        IteratorT const &last, bool expand_undefined);

    template <typename ContainerT>
    bool expand_predefined_macro(TokenT const &curr_token, 
        ContainerT &expanded);

    template <typename ContainerT>
    void expand_arguments (std::vector<ContainerT> &arguments, 
        std::vector<ContainerT> &expanded_args, bool expand_undefined);

    template <typename ContainerT>
    void expand_replacement_list(
        macro_definition<TokenT> const &macrodefinition,
        std::vector<ContainerT> const &arguments, 
        std::vector<ContainerT> const &expanded_args, ContainerT &expanded);

    template <typename ContainerT>
    void rescan_replacement_list(TokenT const &curr_token, 
        macro_definition<TokenT> &macrodef, ContainerT &replacement_list, 
        ContainerT &expanded, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    TokenT const &resolve_defined(IteratorT &first, IteratorT const &last,
        ContainerT &expanded);

    template <typename ContainerT>
    void concat_tokensequence(ContainerT &expanded);

private:
    defined_macros_t defined_macros;
    TokenT act_token;
    TokenT main_token;      // last returned token from the pp_iterator
};

///////////////////////////////////////////////////////////////////////////////
// 
//  add_macro(): adds a new macro to the macromap
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
inline bool 
macromap<TokenT>::add_macro(TokenT const &name, bool has_parameters,
    parameter_container_t &parameters, definition_container_t &definition,
    bool is_predefined)
{
typename defined_macros_t::iterator it = defined_macros.find(name.get_value());

    if (it != defined_macros.end()) {
    // redefinition, should not be different
        if ((*it).second.is_functionlike != has_parameters ||
            (*it).second.macroparameters != parameters ||
            (*it).second.macrodefinition != definition)
        {
            CPP_THROW(preprocess_exception, macro_redefinition, 
                name.get_value(), name);
        }
        return true;
    }

// insert a new macro node
    std::pair<typename defined_macros_t::iterator, bool> p = 
        defined_macros.insert(defined_macros_t::value_type(
            name.get_value(), macro_definition<TokenT>(name, has_parameters, 
                is_predefined)));
            
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
    return 
        defined_macros.find(name) != defined_macros.end() ||
        name == "__LINE__" || name == "__FILE__";
}

///////////////////////////////////////////////////////////////////////////////
// 
//  remove_macro(): remove a macro from the macromap
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
inline bool 
macromap<TokenT>::remove_macro(TokenT const &name)
{
    typename defined_macros_t::iterator it = 
        defined_macros.find(name.get_value());
    
    if (it != defined_macros.end()) {
        if ((*it).second.is_predefined) {
            CPP_THROW(preprocess_exception, bad_undefine_statement, 
                name.get_value(), name);
        }
        defined_macros.erase(it);
        return true;
    }
    else if ("__LINE__" == name.get_value() || "__FILE__" == name.get_value()) 
    {
        CPP_THROW(preprocess_exception, bad_undefine_statement, 
            name.get_value(), name);
    }
    return false;       // macro was not defined
}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_tokensequence
//
//      This function is a helper function which wrappes the given iterator 
//      range into corresponding unput_iterator's and calles the main workhorse
//      of the macro expansion engine (the function expand_tokensequence_worker)
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline TokenT const &
macromap<TokenT>::expand_tokensequence(IteratorT &first, IteratorT const &last,
    ContainerT &pending, ContainerT &expanded, bool expand_undefined)
{
    typedef impl::gen_unput_queue_iterator<IteratorT, TokenT, ContainerT> gen_t;
    typedef typename gen_t::return_t iterator_t;

    ContainerT eof_queue;
    iterator_t first_it = gen_t::generate(expanded, first);
    iterator_t last_it = gen_t::generate(eof_queue, last);
    
on_exit::assign<IteratorT, iterator_t> on_exit(first, first_it);

    return expand_tokensequence_worker(pending, first_it, last_it, expand_undefined);
}

///////////////////////////////////////////////////////////////////////////////
//
//  expand_tokensequence_worker
//
//      This function is the main workhorse of the macro expansion engine. It
//      expands as much tokens as needed to identify the next preprocessed 
//      token to return to the caller. 
//      It returns the next preprocessed token.
//
//      Thhe iterator 'first' is adjusted accordingly.
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline TokenT const &
macromap<TokenT>::expand_tokensequence_worker(
    ContainerT &pending, 
    unput_queue_iterator<IteratorT, TokenT, ContainerT> &first, 
    unput_queue_iterator<IteratorT, TokenT, ContainerT> const &last, 
    bool expand_undefined)
{
    using namespace cpplexer;

// if there exist pending tokens (tokens, which are already preprocessed), then
// return the next one from there
    if (!pending.empty()) {
    on_exit::pop_front<TokenT> pop_front_token(pending);
    
        return act_token = pending.front();
    }

//  analyze the next element of the given sequence, if it is an 
//  T_IDENTIFIER token, try to replace this as a macro etc.
    if (first != last) {
    token_id id = token_id(*first);

    // ignore placeholder tokens
        if (T_PLACEHOLDER == id) {
        TokenT placeholder = *first;
        
            ++first;
            if (first == last)
                return act_token = placeholder;
            id = token_id(*first);
        }
            
        if (T_IDENTIFIER == id) {
        // try to replace this identifier as a macro
            if (typename TokenT::string_t("defined") == (*first).get_value()) {
            // resolve operator defined()
                return resolve_defined(first, last, pending);
            }
            if (is_defined((*first).get_value())) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
                if (expand_macro(pending, first, last, expand_undefined)) {
                // the tokens returned by expand_macro should be rescanned
                // beginning at the last token of the returned replacement list
                    if (first != last) {
                    typename ContainerT::reverse_iterator rit = pending.rbegin();
                    
                        first.get_unput_queue().splice(
                            first.get_unput_queue().begin(), pending,
                            (++rit).base(), pending.end());
                    }
                                            
                // fall through ...
                }
                else if (!pending.empty()) {
                // return the first token from the pending queue
                on_exit::pop_front<TokenT> pop_front_queue (pending);
                
                    return act_token = pending.front();
                }
                else {
                // macro expansion reached the eoi
                    return act_token = TokenT();
                }

            // return the next preprocessed token
                return expand_tokensequence_worker(pending, first, last, 
                    expand_undefined);
            }
            else if (expand_undefined) {
            // in preprocessing conditionals undefined identifiers and keywords 
            // are to be replaced with '0' (see. C++ standard 16.1.4)
                return act_token = TokenT(T_INTLIT, "0", (*first++).get_position());
            }
            else {
                act_token = *first;
                ++first;
                return act_token;
            }
        }
        else if (expand_undefined && IS_CATEGORY(*first, KeywordTokenType) && 
                 T_TRUE != id && T_FALSE != id)
        {
        // all remaining identifiers and keywords, except for true and false, 
        // are replaced with the pp-number 0 (C++ standard 16.1.4, [cpp.cond])
            return act_token = TokenT(T_INTLIT, "0", (*first++).get_position());
        }
        else {
            act_token = *first;
            ++first;
            return act_token;
        }
    }
    return act_token = TokenT();     // eoi
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
TokenT startof_argument_list = *next;

    while (++next != end && nested_parenthesis_level) {
    token_id id = token_id(*next);

        if (0 == parameter_count && 
            !IS_CATEGORY((*next), WhiteSpaceTokenType) && id != T_NEWLINE &&
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
        case T_NEWLINE:
            if (!was_whitespace) 
                argument->push_back(TokenT(T_SPACE, " ", (*next).get_position()));
            was_whitespace = true;
            break;      // skip whitespace

        case T_PLACEHOLDER:
            break;      // ignore placeholder
                        
        default:
            argument->push_back(*next);
            was_whitespace = false;
            break;
        }
    }

    if (nested_parenthesis_level >= 1) {
    // missing ')': improperly terminated macro invocation
        CPP_THROW(preprocess_exception, improperly_terminated_macro, 
            "missing ')'", startof_argument_list);
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
    
//    template <typename IteratorT>
//    bool next_token_is(IteratorT it, IteratorT const &end, token_id id)
//    {
//        using namespace cpplexer;
//        if (++it == end) return false;
//        while (IS_CATEGORY(*it, WhiteSpaceTokenType) || 
//               T_NEWLINE == token_id(*it)) 
//        {
//            if (++it == end)
//                return false;
//        }
//        return (token_id(*it) == id) ? true : false;
//    }

    template <typename IteratorT>
    bool skip_to_token(IteratorT &it, IteratorT const &end, token_id id)
    {
        using namespace cpplexer;
        if (token_id(*it) == id) 
            return true;
        if (++it == end) 
            return false;
        while (IS_CATEGORY(*it, WhiteSpaceTokenType) || 
               T_NEWLINE == token_id(*it)) 
        {
            if (++it == end)
                return false;
        }
        BOOST_SPIRIT_ASSERT(token_id(*it) == id);
        return true;
    }

}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_arguments
//
//      fully expands the arguments of a macro call 
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline void
macromap<TokenT>::expand_whole_tokensequence(ContainerT &expanded, 
    IteratorT const &first, IteratorT const &last, bool expand_undefined)
{
    typedef impl::gen_unput_queue_iterator<IteratorT, TokenT, ContainerT> gen_t;
    typedef typename gen_t::return_t iterator_t;

    ContainerT eof_queue;
    ContainerT unput_queue;
    iterator_t first_it = gen_t::generate(unput_queue, first);
    iterator_t last_it = gen_t::generate(eof_queue, last);
    
    ContainerT pending_queue;
    while (!pending_queue.empty() || first_it != last_it) {
        expanded.push_back(
            expand_tokensequence_worker(pending_queue, first_it, 
                last_it, expand_undefined)
        );
    }
    
// should have returend all expanded tokens
    BOOST_SPIRIT_ASSERT(unput_queue.empty() && pending_queue.empty());
}

template <typename TokenT>
template <typename ContainerT>
inline void 
macromap<TokenT>::expand_arguments (std::vector<ContainerT> &arguments, 
    std::vector<ContainerT> &expanded_args, bool expand_undefined)
{
typename std::vector<ContainerT>::size_type i = 0;

    typename std::vector<ContainerT>::iterator arg_end = arguments.end();
    for (typename std::vector<ContainerT>::iterator arg_it = arguments.begin(); 
         arg_it != arg_end; ++arg_it, ++i)
    {
        expand_whole_tokensequence(expanded_args[i], (*arg_it).begin(), 
            (*arg_it).end(), expand_undefined);
    }
}
                    
///////////////////////////////////////////////////////////////////////////////
// 
//  expand_replacement_list
//
//      fully expands the replacement list of a given macro with the 
//      actual arguments/expanded arguments
//      handles the '#' [cpp.stringize] and the '##' [cpp.concat] operator
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename ContainerT>
inline void
macromap<TokenT>::expand_replacement_list(
    macro_definition<TokenT> const &macrodef,
    std::vector<ContainerT> const &arguments, 
    std::vector<ContainerT> const &expanded_args, ContainerT &expanded)
{
    BOOST_SPIRIT_ASSERT(arguments.size() == expanded_args.size());
    
    using namespace cpplexer;
    typedef typename macro_definition_t::const_definition_iterator_t 
        macro_definition_iter_t;

bool seen_concat = false;
bool adjacent_concat = false;
bool adjacent_stringize = false;

    macro_definition_iter_t cend = macrodef.macrodefinition.end();
    for (macro_definition_iter_t cit = macrodef.macrodefinition.begin();
        cit != cend; ++cit)
    {
    bool use_replaced_arg = true;
    token_id id = token_id(*cit);
    
        if (T_POUND_POUND == id) {
        // conactination operator
            adjacent_concat = true;
            seen_concat = true;
        }
        else if (T_POUND == id) {
        // stringize operator
            adjacent_stringize = true;
            seen_concat = true;
        }
        else {
            if (adjacent_stringize || adjacent_concat || 
                impl::next_token_is<macro_definition_iter_t>
                    ::test(cit, cend, T_POUND_POUND))
            {
                use_replaced_arg = false;
            }
            if (adjacent_concat)    // spaces after '##' ?
                adjacent_concat = IS_CATEGORY(*cit, WhiteSpaceTokenType);
        }

        if (IS_CATEGORY((*cit), ParameterTokenType)) {
        // copy argument 'i' instead of the parameter token i
        typename ContainerT::size_type i = token_id(*cit) - T_PARAMETERBASE;

            BOOST_SPIRIT_ASSERT(i < arguments.size());
            if (use_replaced_arg && !expanded_args[i].empty()) {
            // replace argument
            ContainerT const &arg = expanded_args[i];
            
                std::copy(arg.begin(), arg.end(), 
                    std::inserter(expanded, expanded.end()));
            }
            else if (adjacent_stringize && 
                    !IS_CATEGORY(*cit, WhiteSpaceTokenType)) 
            {
            // stringize the current argument
                BOOST_SPIRIT_ASSERT(!arguments[i].empty());
                
            typename TokenT::position_t const &pos =
                (*arguments[i].begin()).get_position();
            
                expanded.push_back(TokenT(T_STRINGLIT, 
                    impl::as_stringlit(arguments[i], pos), pos));
                adjacent_stringize = false;
            }
            else {
            // simply copy the original argument (adjacent '##' or
            ContainerT const &arg = arguments[i];
            
                std::copy(arg.begin(), arg.end(), 
                    std::inserter(expanded, expanded.end()));
            }
        }
        else {
        // insert the actual replacement token
            expanded.push_back(*cit);
        }
    }
    
// handle the cpp.concat operator
    if (seen_concat)
        concat_tokensequence(expanded);
}

template <typename TokenT>
template <typename ContainerT>
inline void 
macromap<TokenT>::rescan_replacement_list(TokenT const &curr_token, 
    macro_definition<TokenT> &macro_def, ContainerT &replacement_list, 
    ContainerT &expanded, bool expand_undefined)
{
    using namespace cpplexer;

// rescan the replacement list
    if (!replacement_list.empty()) {
    on_exit::reset<bool> on_exit(macro_def.is_available_for_replacement, false);

        expand_whole_tokensequence(expanded, replacement_list.begin(),
            replacement_list.end(), expand_undefined);
    }
    
    
    if (expanded.empty()) {
    // the resulting replacement list should contain at least a placeholder
    // token
        BOOST_SPIRIT_ASSERT(expanded.empty());
        expanded.push_back(TokenT(T_PLACEHOLDER, "§", curr_token.get_position()));
    }
}

template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline bool 
macromap<TokenT>::expand_macro(ContainerT &expanded, IteratorT &first, 
    IteratorT const &last, bool expand_undefined) 
{
    using namespace cpplexer;
    
TokenT curr_token = *first;

    BOOST_SPIRIT_ASSERT(T_IDENTIFIER == token_id(curr_token));
    typename defined_macros_t::iterator it = 
        defined_macros.find(curr_token.get_value());
    if (it == defined_macros.end()) {
        ++first;    // advance

    // try to expand a predefined macro (__FILE__ or __LINE__)
        if (expand_predefined_macro(curr_token, expanded))
            return false;
        
    // not defined as a macro
        expanded.push_back(curr_token);
        return false;
    }

// ensure the parameters to be replaced with special parameter tokens
macro_definition<TokenT> &macro_def = (*it).second;

    macro_def.replace_parameters();

    if (!macro_def.is_available_for_replacement) {
    // this macro is marked as non-replaceable
    // copy the macro name itself
        expanded.push_back(TokenT(T_NONREPLACABLE_IDENTIFIER,
            curr_token.get_value(), curr_token.get_position()));
        ++first;
        return false;
    }

ContainerT replacement_list;

    if (impl::next_token_is<IteratorT>::test(first, last, T_LEFTPAREN)) {
    // called as a function-like macro 
        skip_to_token(first, last, T_LEFTPAREN);
        
        if (macro_def.is_functionlike) {
        // defined as a function-like macro

        // collect the arguments
        std::vector<ContainerT> arguments;
        
            collect_arguments (arguments, first, last, 
                macro_def.macroparameters.size());
            if (arguments.size() < macro_def.macroparameters.size()) {
            // too many macro arguments
                CPP_THROW(preprocess_exception, too_few_macroarguments, 
                    "", curr_token);
            }

        // try to expand the arguments itself
        std::vector<ContainerT> expanded_args(arguments.size());
        
            expand_arguments(arguments, expanded_args, expand_undefined);
            
        // expand the replacement list of this macro
            expand_replacement_list(macro_def, arguments, expanded_args,
                replacement_list);
        }
        else {
        // defined as an object-like macro
            std::copy (macro_def.macrodefinition.begin(), 
                macro_def.macrodefinition.end(), 
                std::inserter(replacement_list, replacement_list.end()));
        }
    }
    else {
    // called as an object like macro
        if ((*it).second.is_functionlike) {
        // defined as a function-like macro
            expanded.push_back(curr_token);
            ++first;                // skip macro name
            return false;           // no further preprocessing required
        }
        else {
        // defined as an object-like macro (expand it)
            std::copy (macro_def.macrodefinition.begin(), 
                macro_def.macrodefinition.end(), 
                std::inserter(replacement_list, replacement_list.end()));
            ++first;                // skip macro name
        }
    }

// rescan the replacement list
    rescan_replacement_list(curr_token, macro_def, replacement_list, expanded, 
        expand_undefined);
    return true;        // rescan is required
}

template <typename TokenT>
template <typename ContainerT>
inline bool 
macromap<TokenT>::expand_predefined_macro(TokenT const &curr_token, 
    ContainerT &expanded)
{
    using namespace cpplexer;
    
string_t const &value = curr_token.get_value();

    if (value == "__LINE__") { 
    // expand the __LINE__ macro
    char buffer[22];    // 21 bytes holds all NUL-terminated unsigned 64-bit numbers

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "%d", main_token.get_position().line);
        expanded.push_back(TokenT(T_INTLIT, buffer, curr_token.get_position()));
        return true;
    }
    else if (value == "__FILE__") {
    // expand the __FILE__ macro
        expanded.push_back(TokenT(T_STRINGLIT, main_token.get_position().file, 
            curr_token.get_position()));
        return true;
    }
    return false;   // no predefined token
}

///////////////////////////////////////////////////////////////////////////////
//
//  resolve_defined(): resolve the operator defined() and replace it with the 
//                     correct T_INTLIT token
//
///////////////////////////////////////////////////////////////////////////////
template <typename TokenT>
template <typename IteratorT, typename ContainerT>
inline TokenT const &
macromap<TokenT>::resolve_defined(IteratorT &first, IteratorT const &last,
    ContainerT &expanded) 
{
    using namespace cpplexer;
    
TokenT result;
cpp::grammars::defined_grammar<TokenT> g;
boost::spirit::parse_info<IteratorT> hit =
    parse(first, last, g[assign(result)], ch_p(T_SPACE) | ch_p(T_CCOMMENT));

    if (!hit.hit) {
        CPP_THROW(preprocess_exception, ill_formed_expression, 
            "defined()", (*first));
    }
    impl::assign_iterator<IteratorT>::do_(first, hit.stop);

// insert a token, which reflects the outcome
bool isdefined = is_defined(result.get_value());

    return act_token = TokenT(T_INTLIT, isdefined ? "1" : "0", 
        result.get_position());
}

template <typename TokenT>
template <typename ContainerT>
inline void 
macromap<TokenT>::concat_tokensequence(ContainerT &expanded)
{
    using namespace cpplexer;
    
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
            while (IS_CATEGORY(*next, WhiteSpaceTokenType)) {
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
        if (!IS_CATEGORY(*it, WhiteSpaceTokenType))        
            prev = it;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  init_predefined_macros(): init the predefined macros
//
///////////////////////////////////////////////////////////////////////////////

namespace predefined_macros {

// list of static predefined macros
    struct static_macros {
        char const *name;
        cpplexer::token_id token_id;
        char const *value;
    } 
    static_data[] = {
        { "__STDC__", T_INTLIT, "1" },
        { "__cplusplus", T_INTLIT, "199711L" },
        { 0, T_EOF, 0 }
    }; 
    
// list of dynamic predefined macros
    typedef char const * (* get_dynamic_value)(bool);

// __DATE__ 
    inline char const *get_date(bool reset)
    {
    static const char *const monthnames[] =
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static std::string datestr;
    
        if (reset) {
            datestr.clear();
        }
        else {
        std::time_t tt = time(0);
        struct std::tm *tb = 0;

            if (tt != (std::time_t)-1) {
            char buffer[sizeof("\"Oct 11 1347\"")+1];

                using namespace std;    // for some systems sprintf is in namespace std
                tb = localtime (&tt);
                sprintf (buffer, "\"%s %2d %4d\"",
                    monthnames[tb->tm_mon], tb->tm_mday, tb->tm_year + 1900);
                datestr = buffer;
            }
            else {
                datestr = "\"??? ?? ????\"";
            }
        }
        return datestr.c_str();
    }

// __TIME__
    inline char const *get_time(bool reset)
    {
    static std::string timestr;
    
        if (reset) {
            timestr.clear();
        }
        else {
        std::time_t tt = time(0);
        struct std::tm *tb = 0;

            if (tt != (std::time_t)-1) {
            char buffer[sizeof("\"12:34:56\"")+1];

                using namespace std;    // for some systems sprintf is in namespace std
                tb = localtime (&tt);
                sprintf (buffer, "\"%02d:%02d:%02d\"", tb->tm_hour, 
                    tb->tm_min, tb->tm_sec);
                timestr = buffer;
            }
            else {
                timestr = "\"??? ?? ????\"";
            }
        }
        return timestr.c_str();
    }

// __SPIRIT_PP__
    inline char const *get_version(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("0x0000")+1];

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "0x%02d%1d%1d", CPP_VERSION_MAJOR, 
            CPP_VERSION_MINOR, CPP_VERSION_SUBMINOR);
        versionstr = buffer;
        return versionstr.c_str();
    }
    
// __SPIRIT_PP_VERSION__
    cpp::util::time_conversion_helper const 
        compilation_time(__DATE__ " " __TIME__);
        
    inline char const *get_fullversion(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("0x00000000")+1];

    // calculate the number of days since Dec 13 2001 
    // (the day the cpp project was started)
    std::tm first_day;

        using namespace std;    // for some systems memset is in namespace std
        memset (&first_day, 0, sizeof(std::tm));
        first_day.tm_mon = 11;           // Dec
        first_day.tm_mday = 13;          // 13
        first_day.tm_year = 101;         // 2001

    long seconds = long(std::difftime(compilation_time.get_time(), 
        std::mktime(&first_day)));

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "0x%02d%1d%1d%04ld", CPP_VERSION_MAJOR,
             CPP_VERSION_MINOR, CPP_VERSION_SUBMINOR, seconds/(3600*24));
        versionstr = buffer;
        return versionstr.c_str();
    }
    
// __SPIRIT_PP_VERSION_STR__
    inline char const *get_versionstr(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("\"00.00.00.0000\"")+1];

    // calculate the number of days since Dec 13 2001 
    // (the day the cpp project was started)
    std::tm first_day;

        using namespace std;    // for some systems memset is in namespace std
        memset (&first_day, 0, sizeof(std::tm));
        first_day.tm_mon = 11;           // Dec
        first_day.tm_mday = 13;          // 13
        first_day.tm_year = 101;         // 2001

    long seconds = long(std::difftime(compilation_time.get_time(), 
        std::mktime(&first_day)));

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "\"%d.%d.%d.%ld\"", CPP_VERSION_MAJOR,
             CPP_VERSION_MINOR, CPP_VERSION_SUBMINOR, seconds/(3600*24));
        versionstr = buffer;
        return versionstr.c_str();
    }
    
    struct dynamic_macros {
        char const *name;
        cpplexer::token_id token_id;
        get_dynamic_value generator;
    }
    dynamic_data[] = {
        { "__DATE__", T_STRINGLIT, get_date },
        { "__TIME__", T_STRINGLIT, get_time },
        { "__SPIRIT_PP__", T_INTLIT, get_version },
        { "__SPIRIT_PP_VERSION__", T_INTLIT, get_fullversion },
        { "__SPIRIT_PP_VERSION_STR__", T_STRINGLIT, get_versionstr },
        { 0, T_EOF, 0 }
    };

}   // namespace predefined_macros

template <typename TokenT>
inline void 
macromap<TokenT>::init_predefined_macros()
{
    using namespace predefined_macros;

// first, add the static macros
std::vector<TokenT> macroparameters;

    for (int i = 0; 0 != static_data[i].name; ++i) {
    std::list<TokenT> macrodefinition;
    typename TokenT::position_t pos;
    
        macrodefinition.push_back(TokenT(static_data[i].token_id, 
            static_data[i].value, pos));
        add_macro(TokenT(T_IDENTIFIER, static_data[i].name, pos), false, 
            macroparameters, macrodefinition, true);
    }

// now add the dynamic macros
    for (int j = 0; 0 != dynamic_data[j].name; ++j) {
    std::list<TokenT> macrodefinition;
    typename TokenT::position_t pos;
    
        macrodefinition.push_back(TokenT(dynamic_data[j].token_id, 
            dynamic_data[j].generator(false), pos));
        add_macro(TokenT(T_IDENTIFIER, dynamic_data[j].name, pos), false, 
            macroparameters, macrodefinition, true);
    }
}

template <typename TokenT>
inline void 
macromap<TokenT>::reset_macromap()
{
    defined_macros.clear();
    predefined_macros::get_time(true);
    predefined_macros::get_date(true);
    act_token = TokenT();
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace cpp

#endif // !defined(_CPP_MACROMAP_HPP__CB8F51B0_A3F0_411C_AEF4_6FF631B8B414__INCLUDED_)
