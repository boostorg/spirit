/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_PARSER_OCTOBER_16_2008_0254PM)
#define BOOST_SPIRIT_PARSER_OCTOBER_16_2008_0254PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/home/support/unused.hpp>

namespace boost { namespace spirit { namespace x3
{
    using spirit::unused_type;
    using spirit::unused;

    template <typename Subject, typename Action>
    struct action;

    struct parser_base {};
    struct parser_id;

    template <typename Derived>
    struct parser : parser_base
    {
        typedef Derived derived_type;
        static bool const handles_container = false;
        static bool const is_pass_through_unary = false;

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }

        template <typename Action>
        action<Derived, Action>
        operator[](Action const& f) const
        {
            return action<Derived, Action>(this->derived(), f);
        }
    };

    struct unary_category;
    struct binary_category;

    template <typename Subject, typename Derived>
    struct unary_parser : parser<Derived>
    {
        typedef unary_category category;
        typedef Subject subject_type;
        static bool const has_attribute = Subject::has_attribute;

        unary_parser(Subject subject)
            : subject(subject) {}

        unary_parser const& get_unary() const { return *this; }

        Subject subject;
    };

    template <typename Left, typename Right, typename Derived>
    struct binary_parser : parser<Derived>
    {
        typedef binary_category category;
        typedef Left left_type;
        typedef Right right_type;
        static bool const has_attribute =
            left_type::has_attribute || right_type::has_attribute;

        binary_parser(Left left, Right right)
            : left(left), right(right) {}

        binary_parser const& get_binary() const { return *this; }

        Left left;
        Right right;
    };

    namespace extension
    {
        template <typename T, typename Enable = void>
        struct as_parser {};

        template <>
        struct as_parser<unused_type>
        {
            typedef unused_type type;
            typedef unused_type value_type;
            static type call(unused_type)
            {
                return unused;
            }
        };

        template <typename Derived>
        struct as_parser<Derived
            , typename enable_if<is_base_of<parser_base, Derived>>::type>
        {
            typedef Derived const& type;
            typedef Derived value_type;
            static type call(Derived const& p)
            {
                return p;
            }
        };

        template <typename Derived>
        struct as_parser<parser<Derived>>
        {
            typedef Derived const& type;
            typedef Derived value_type;
            static type call(parser<Derived> const& p)
            {
                return p.derived();
            }
        };
    }

    template <typename T>
    inline typename extension::as_parser<T>::type
    as_parser(T const& x)
    {
        return extension::as_parser<T>::call(x);
    }

    template <typename Derived>
    inline Derived const&
    as_parser(parser<Derived> const& p)
    {
        return p.derived();
    }
}}}

#endif
