/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_CONFIG_HPP)
#define BOOST_SPIRIT_X3_CALC7_CONFIG_HPP

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3.hpp>

namespace client
{
    // this id is used to tag the position_cache so we can
    // access it from the x3 context
    struct position_cache_tag;
}

namespace client { namespace parser
{
    typedef std::string::const_iterator iterator_type;
    typedef x3::phrase_parse_context<x3::ascii::space_type>::type phrase_context_type;
    typedef x3::position_cache<std::vector<iterator_type>> position_cache_type;

    typedef x3::with_context<
        position_cache_tag
      , std::reference_wrapper<position_cache_type> const
      , phrase_context_type>::type
    context_type;
}}

#endif
