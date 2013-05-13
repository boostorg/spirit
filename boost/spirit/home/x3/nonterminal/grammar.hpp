/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_GRAMMAR_JAN_19_2012_0454PM)
#define BOOST_SPIRIT_GRAMMAR_JAN_19_2012_0454PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/nonterminal/grammar.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/has_key.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/config.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Elements, typename Next>
    struct grammar_context
    {
        grammar_context(Elements const& elements, Next const& next)
            : elements(elements), next(next) {}

        template <typename ID>
        struct get_result
        {
            typedef typename ID::type id_type;
            typedef typename mpl::eval_if<
                fusion::result_of::has_key<Elements const, id_type>
              , fusion::result_of::at_key<Elements const, id_type>
              , typename Next::template get_result<ID>>::type
            type;
        };

        template <typename ID>
        typename get_result<ID>::type
        get_impl(ID id, mpl::true_) const
        {
            typedef typename ID::type id_type;
            return fusion::at_key<id_type>(elements);
        }

        template <typename ID>
        typename get_result<ID>::type
        get_impl(ID id, mpl::false_) const
        {
            return next.get(id);
        }

        template <typename ID>
        typename get_result<ID>::type
        get(ID id) const
        {
            typedef typename ID::type id_type;
            typename fusion::result_of::has_key<Elements, id_type> has_key;
            return get_impl(id, has_key);
        }

        Elements const& elements;
        Next const& next;
    };

    template <typename Elements>
    struct grammar_parser : parser<grammar_parser<Elements>>
    {
        typedef typename
            remove_reference<
                typename fusion::result_of::front<Elements>::type
            >::type::second_type
        start_rule;

        typedef typename start_rule::attribute_type attribute_type;
        static bool const has_attribute = start_rule::has_attribute;

        grammar_parser(char const* name, Elements const& elements)
          : name(name), elements(elements) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute_& attr) const
        {
            grammar_context<Elements, Context> our_context(elements, context);
            return fusion::front(elements).second.parse(first, last, our_context, attr);
        }

        char const* name;
        Elements elements;
    };

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

    template <typename ...Elements>
    grammar_parser<fusion::map<fusion::pair<typename Elements::id, Elements>...>>
    grammar(char const* name, Elements const&... elements)
    {
        typedef fusion::map<fusion::pair<typename Elements::id, Elements>...> sequence;
        return grammar_parser<sequence>(name,
            sequence(fusion::make_pair<typename Elements::id>(elements)...));
    }

#else

    template <typename A>
    grammar_parser<fusion::map<A>>
    grammar(char const* name, A const& a)
    {
        typedef fusion::map<A> sequence;
        return grammar_parser<sequence>(name, sequence(a));
    }

    template <typename A, typename B>
    grammar_parser<fusion::map<A, B>>
    grammar(char const* name, A const& a, B const& b)
    {
        typedef fusion::map<A, B> sequence;
        return grammar_parser<sequence>(name, sequence(a, b));
    }

    template <typename A, typename B, typename C>
    grammar_parser<fusion::map<A, B, C>>
    grammar(char const* name, A const& a, B const& b, C const& c)
    {
        typedef fusion::map<A, B, C> sequence;
        return grammar_parser<sequence>(name, sequence(a, b, c));
    }

#endif

    template <typename Elements>
    struct get_info<grammar_parser<Elements>>
    {
        typedef std::string result_type;
        std::string operator()(grammar_parser<Elements> const& p) const
        {
            return p.name;
        }
    };

}}}

#endif
