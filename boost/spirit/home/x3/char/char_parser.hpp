/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_CHAR_PARSER_APR_16_2006_0906AM)
#define BOOST_SPIRIT_CHAR_PARSER_APR_16_2006_0906AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/support/traits/assign_to.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    // The base char_parser
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct char_parser : parser<Derived>
    {
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            x3::skip_over(first, last, context);

            if (first != last && this->derived().test(*first, context))
            {
                spirit::traits::assign_to(*first, attr);
                ++first;
                return true;
            }
            return false;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // negated_char_parser handles ~cp expressions (cp is a char_parser)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Positive>
    struct negated_char_parser :
        char_parser<negated_char_parser<Positive>>
    {
        negated_char_parser(Positive const& positive)
          : positive(positive) {}

        template <typename CharParam, typename Context>
        bool test(CharParam ch, Context& context) const
        {
            return !positive.test(ch, context);
        }

        Positive positive;
    };

    template <typename Positive>
    inline negated_char_parser<Positive>
    operator~(char_parser<Positive> const& cp)
    {
        return negated_char_parser<Positive>(cp.derived());
    }

    template <typename Positive>
    inline Positive const&
    operator~(negated_char_parser<Positive> const& cp)
    {
        return cp.positive;
    }
}}}

#endif
