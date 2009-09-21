/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_EXPECT_APRIL_29_2007_0445PM)
#define SPIRIT_EXPECT_APRIL_29_2007_0445PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/operator/sequence_base.hpp>
#include <boost/spirit/home/qi/detail/expect_function.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/support/info.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<qi::domain, proto::tag::greater> // enables >
      : mpl::true_ {};

    template <>
    struct flatten_tree<qi::domain, proto::tag::greater> // flattens >
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace qi
{
    template <typename Iterator>
    struct expectation_failure
    {
        Iterator first;
        Iterator last;
        info what;
    };

    template <typename Elements>
    struct expect : sequence_base<expect<Elements>, Elements>
    {
        friend struct sequence_base<expect<Elements>, Elements>;

        expect(Elements const& elements)
          : sequence_base<expect<Elements>, Elements>(elements) {}

    private:

        template <typename Iterator, typename Context, typename Skipper>
        static detail::expect_function<
            Iterator, Context, Skipper
          , expectation_failure<Iterator> >
        fail_function(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper)
        {
            return detail::expect_function<
                Iterator, Context, Skipper, expectation_failure<Iterator> >
                (first, last, context, skipper);
        }

        std::string id() const { return "expect"; }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::greater, Elements, Modifiers>
      : make_nary_composite<Elements, expect>
    {};
}}}

namespace boost { namespace spirit { namespace traits
{
    template <typename T>
    struct has_semantic_action;

    template <typename Elements>
    struct has_semantic_action<qi::expect<Elements> >
      : nary_has_semantic_action<Elements> {};
}}}

#endif
