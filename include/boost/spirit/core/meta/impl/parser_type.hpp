/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Joel de Guzman
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_PARSER_TYPE_HPP)
#define BOOST_SPIRIT_PARSER_TYPE_HPP

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
#include "boost/spirit/core/primitives/primitives.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Helper templates to derive the parser type from an auxilliary type and to
//  generate an object of the required parser type given an auxilliary object.
//  Supported types to convert are parsers, single characters and character
//  strings.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

///////////////////////////////////////////////////////////////////////////////
//  conformant compilers support PTS
template<typename T>
struct as_parser
{
    typedef T type;
    static type const& convert(type const& p) { return p; }
};

template<>
struct as_parser<char>
{
    typedef chlit<char> type;
    static type convert(char ch) { return type(ch); }
};

template<>
struct as_parser<wchar_t>
{
    typedef chlit<wchar_t> type;
    static type convert(wchar_t ch) { return type(ch); }
};

template<>
struct as_parser<char const *>
{
    typedef strlit<char const *> type;
    static type convert(char const* str) { return type(str); }
};

template<>
struct as_parser<wchar_t const *>
{
    typedef strlit<wchar_t const *> type;
    static type convert(wchar_t const* str) { return type(str); }
};

#ifdef __BORLANDC__

template<>
struct as_parser<char*>
{
    typedef strlit<char*> type;
    static type convert(char* str) { return type(str); }
};

template<>
struct as_parser<wchar_t*>
{
    typedef strlit<wchar_t*> type;
    static type convert(wchar_t* str) { return type(str); }
};

#endif // __BORLANDC__

template<int N>
struct as_parser<char[N]>
{
    typedef strlit<char const *> type;
    static type convert(char const str[N]) { return type(str); }
};

template<int N>
struct as_parser<wchar_t[N]>
{
    typedef strlit<wchar_t const *> type;
    static type convert(wchar_t const str[N]) { return type(str); }
};

#if !defined(__MWERKS__) || (__MWERKS__ > 0x2407)

template<int N>
struct as_parser<char const[N]>
{
    typedef strlit<char const *> type;
    static type convert(char const str[N]) { return type(str); }
};

template<int N>
struct as_parser<wchar_t const[N]>
{
    typedef strlit<wchar_t const *> type;
    static type convert(wchar_t const str[N]) { return type(str); }
};

#endif // !defined(__MWERKS__) || (__MWERKS__ > 0x2407)

#else // !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
///////////////////////////////////////////////////////////////////////////////
//  some compilers do not support PTS

template<typename T>
struct as_parser
{
    enum
    {
        is_cptr = boost::is_convertible<T, char const *>::value,
        is_wcptr = boost::is_convertible<T, wchar_t const *>::value
    };

// helps to dispatch to the correct version of get for char[] and T
    typedef
        typename if_t<bool_t<is_cptr || is_wcptr>, int_t<0>, int_t<1> >::type
        selector_t;

// return type (type) and formal argument (arg_type) of convert
    typedef
        typename if_t<
            bool_t<is_cptr>, char const *,
                typename if_t<bool_t<is_wcptr>, wchar_t const *, T>::type
        >::type
        arg_type;

    typedef
        typename if_t<
            bool_t<is_cptr>, strlit<char const *>,
                typename if_t<bool_t<is_wcptr>, strlit<wchar_t const *>, T>::type
        >::type
        type;

    static 
    typename as_parser::type::embed_t 
    convert(arg_type const &t)
    { return convert(t, selector_t()); }

private:

    static 
    typename as_parser::type::embed_t 
    convert(arg_type str, int_t<0> sel)
    { return type(str); }

    static 
    typename as_parser::type::embed_t 
    convert(arg_type const &p, int_t<1> sel)
    { return p; }
};

template<>
struct as_parser<char>
{
    typedef chlit<char> type;
    static type convert(char ch) { return type(ch); }
};

template<>
struct as_parser<wchar_t>
{
    typedef chlit<wchar_t> type;
    static type convert(wchar_t ch) { return type(ch); }
};

template<>
struct as_parser<char const *>
{
    typedef strlit<char const *> type;
    static type convert(char const* str) { return type(str);}
};

template<>
struct as_parser<wchar_t const *>
{
    typedef strlit<wchar_t const *> type;
    static type convert(wchar_t const* str) { return type(str);}
};

#endif // !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

///////////////////////////////////////////////////////////////////////////////
}} // namespace boost::spirit

#endif
