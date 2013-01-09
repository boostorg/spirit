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
#include <boost/fusion/container/deque.hpp>
#include <boost/fusion/sequence/intrinsic/front.hpp>
#include <boost/fusion/algorithm/query/find_if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace boost { namespace spirit { namespace x3
{
    namespace detail
    {
        template <typename ID, typename T>
        struct has_rule_id : mpl::false_ {};

        template <typename ID, typename RHS, typename Attribute>
        struct has_rule_id<ID, rule_definition<ID, RHS, Attribute>>
          : mpl::true_ {};

        template <typename Iterator>
        struct get_rule_definition
        {
            typedef typename
                fusion::result_of::deref<Iterator>::type
            type;
        };
    }

    template <typename Elements, typename Next>
    struct grammar_context
    {
        grammar_context(Elements const& elements, Next const& next)
            : elements(elements), next(next) {}

        template <typename ID>
        struct get_result
        {
            typedef typename
                fusion::result_of::find_if<
                    Elements, detail::has_rule_id<ID, mpl::_>>::type
            iterator_type;

            typedef fusion::result_of::equal_to<
                    iterator_type
                  , typename fusion::result_of::end<Elements>::type>
            id_not_found;

            typedef typename mpl::eval_if<
                id_not_found
              , typename Next::template get_result<ID>
              , detail::get_rule_rhs<iterator_type>>::type
            type;
        };

        template <typename R, typename ID>
        R get_impl(ID id, mpl::true_) const
        {
            return *fusion::find_if<detail::has_rule_id<ID, mpl::_>>(elements);
        }

        template <typename R, typename ID>
        R get(ID id, mpl::false_) const
        {
            return next.get(id);
        }

        template <typename ID>
        typename get_result<ID>::type
        get(ID id) const
        {
            return get_impl<typename get_result<ID>::type>(
                id, get_result<ID>::id_not_found());
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
            >::type
        start_rule;

        typedef typename start_rule::attribute_type attribute_type;
        static bool const has_attribute = start_rule::has_attribute;

        grammar_parser(Elements const& elements)
          : elements(elements) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            grammar_context<Elements, Context> our_context(elements, context);
            return fusion::front(elements).parse(first, last, our_context, attr);
        }

        Elements elements;
    };

    template <typename ...Elements>
    grammar_parser<fusion::deque<Elements...>>
    grammar(Elements const&... elements)
    {
        typedef fusion::deque<Elements...> sequence;
        return grammar_parser<sequence>(sequence(elements...));
    }
}}}

#endif
