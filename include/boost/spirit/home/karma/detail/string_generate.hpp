//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_STRING_GENERATE_FEB_23_2007_1232PM)
#define BOOST_SPIRIT_KARMA_STRING_GENERATE_FEB_23_2007_1232PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <string>
#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/karma/detail/generate_to.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    //  generate a string given by a pointer 
    template <typename OutputIterator, typename Char>
    inline bool string_generate(OutputIterator& sink, Char const* str)
    {
        for (Char ch = *str; ch != 0; ch = *++str)
        {
            *sink = ch;
            ++sink;
        }
        return detail::sink_is_good(sink);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  generate a string given by a std::string
    template <typename OutputIterator, typename Char, typename Traits
      , typename Allocator>
    inline bool string_generate(OutputIterator& sink
      , std::basic_string<Char, Traits, Allocator> const& str)
    {
        return string_generate(sink, str.c_str());
    }

    template <typename OutputIterator, typename Iterator>
    inline bool string_generate(OutputIterator& sink
      , boost::iterator_range<Iterator> const& r)
    {
        Iterator end = r.end();
        for (Iterator it = r.begin(); it != end; ++it)
        {
            *sink = *it;
            ++sink;
        }
        return detail::sink_is_good(sink);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  generate a string given by a pointer, converting according using a 
    //  given character class and case tag
    template <typename OutputIterator, typename Char, typename CharEncoding
      , typename Tag>
    inline bool string_generate(OutputIterator& sink, Char const* str
      , CharEncoding, Tag)
    {
        for (Char ch = *str; ch != 0; ch = *++str)
        {
            *sink = spirit::char_class::convert<CharEncoding>::to(Tag(), ch);
            ++sink;
        }
        return detail::sink_is_good(sink);
    }

    template <typename OutputIterator, typename Char>
    inline bool string_generate(OutputIterator& sink, Char const* str
      , unused_type, unused_type)
    {
        return string_generate(sink, str);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  generate a string given by a std::string, converting according using a 
    //  given character class and case tag
    template <typename OutputIterator, typename Char, typename CharEncoding
      , typename Tag, typename Traits, typename Allocator>
    inline bool string_generate(OutputIterator& sink
      , std::basic_string<Char, Traits, Allocator> const& str
      , CharEncoding ce, Tag tag)
    {
        return string_generate(sink, str.c_str(), ce, tag);
    }

    template <typename OutputIterator, typename Iterator, typename CharEncoding
      , typename Tag>
    inline bool string_generate(OutputIterator& sink
      , boost::iterator_range<Iterator> const& r, CharEncoding, Tag)
    {
        Iterator end = r.end();
        for (Iterator it = r.begin(); it != end; ++it)
        {
            *sink = spirit::char_class::convert<CharEncoding>::to(Tag(), *it);
            ++sink;
        }
        return detail::sink_is_good(sink);
    }

    template <typename OutputIterator, typename Char, typename Traits
      , typename Allocator>
    inline bool string_generate(OutputIterator& sink
      , std::basic_string<Char, Traits, Allocator> const& str
      , unused_type, unused_type)
    {
        return string_generate(sink, str.c_str());
    }

    template <typename OutputIterator, typename Iterator>
    inline bool string_generate(OutputIterator& sink
      , boost::iterator_range<Iterator> const& r, unused_type, unused_type)
    {
        Iterator end = r.end();
        for (Iterator it = r.begin(); it != end; ++it)
        {
            *sink = *it;
            ++sink;
        }
        return detail::sink_is_good(sink);
    }

}}}}

#endif
