/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_EXPECT_FUNCTION_APR_29_2007_0558PM)
#define SPIRIT_EXPECT_FUNCTION_APR_29_2007_0558PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/multi_pass_wrapper.hpp>

namespace boost { namespace spirit { namespace qi { namespace detail
{
    template <
        typename Iterator, typename Context
      , typename Skipper, typename Exception>
    struct expect_function
    {
        typedef Iterator iterator_type;
        typedef Context context_type;

        expect_function(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper)
          : first(first)
          , last(last)
          , context(context)
          , skipper(skipper)
          , is_first(true)
        {
        }

        template <typename Component, typename Attribute>
        bool operator()(Component const& component, Attribute& attr) const
        {
            // if this is not the first component in the expect chain we 
            // need to flush any multi_pass iterator we might be acting on
            if (!is_first)
                spirit::traits::clear_queue(first);

            // if we are testing the first component in the sequence,
            // return true if the parser fails, if this not the first
            // component, throw exception if the parser fails
            if (!component.parse(first, last, context, skipper, attr))
            {
                if (is_first)
                {
                    is_first = false;
                    return true;
                }
                Exception x = { first, last, component.what(context) };
                throw x;
            }
            is_first = false;
            return false;
        }

        template <typename Component>
        bool operator()(Component const& component) const
        {
            // if this is not the first component in the expect chain we 
            // need to flush any multi_pass iterator we might be acting on
            if (!is_first)
                spirit::traits::clear_queue(first);

            // if we are testing the first component in the sequence,
            // return true if the parser fails, if this not the first
            // component, throw exception if the parser fails
            if (!component.parse(first, last, context, skipper, unused))
            {
                if (is_first)
                {
                    is_first = false;
                    return true;
                }
                Exception x = { first, last, component.what(context) };
                throw x;
            }
            is_first = false;
            return false;
        }

        Iterator& first;
        Iterator const& last;
        Context& context;
        Skipper const& skipper;
        mutable bool is_first;
    };
}}}}

#endif
