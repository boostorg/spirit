/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_GRAMMAR_JAN_19_2012_0454PM)
#define BOOST_SPIRIT_X3_GRAMMAR_JAN_19_2012_0454PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
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
            typedef typename Next::template get_result<ID> next_get_result;

            typedef typename mpl::eval_if<
                fusion::result_of::has_key<Elements const, id_type>
              , fusion::result_of::at_key<Elements const, id_type>
              , next_get_result>::type
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

    template <typename ...Elements>
    grammar_parser<fusion::map<fusion::pair<typename Elements::id, Elements>...>>
    grammar(char const* name, Elements const&... elements)
    {
        typedef fusion::map<fusion::pair<typename Elements::id, Elements>...> sequence;
        return grammar_parser<sequence>(name,
            sequence(fusion::make_pair<typename Elements::id>(elements)...));
    }

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

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename Elements, typename Context>
    struct attribute_of<x3::grammar_parser<Elements>, Context>
        : attribute_of<
            typename x3::grammar_parser<Elements>::start_rule, Context> {};
    
    template <typename Elements, typename Context>
    struct has_attribute<x3::grammar_parser<Elements>, Context>
        : has_attribute<
            typename x3::grammar_parser<Elements>::start_rule, Context> {};
}}}}

#endif
