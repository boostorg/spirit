// char_traits.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_CHAR_TRAITS_H
#define BOOST_LEXER_CHAR_TRAITS_H

// Make sure wchar_t is defined
#include <string>

namespace boost
{
namespace lexer
{
template<typename CharT>
struct char_traits
{
    typedef CharT index_type;
};

template<>
struct char_traits<char>
{
    typedef unsigned char index_type;
};

template<>
struct char_traits<wchar_t>
{
    typedef wchar_t index_type;
};
}
}

#endif
