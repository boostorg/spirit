/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_SEQUENCE_DETAIL_JAN_06_2013_1015AM)
#define SPIRIT_SEQUENCE_DETAIL_JAN_06_2013_1015AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/support/traits/attribute_category.hpp>
#include <boost/spirit/home/support/traits/make_attribute.hpp>
#include <boost/spirit/home/support/traits/is_substitute.hpp>
#include <boost/spirit/home/x3/core/detail/get_types.hpp>

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/fusion/include/as_deque.hpp>
#include <boost/fusion/include/mpl.hpp>

#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>

#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct sequence;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Parser>
    struct sequence_size
    {
        //~ typedef typename traits::attribute_of<Parser>::type attr_type;
        //~ typedef typename
            //~ mpl::eval_if<
                //~ fusion::traits::is_sequence<attr_type>
              //~ , fusion::result_of::size<attr_type>
              //~ , mpl::not_<is_same<attr_type, unused_type>>
            //~ >::type
        //~ size_type;
        //~ static int const value = size_type::value;

        static int const value = Parser::has_attribute;
    };

    template <typename L, typename R>
    struct sequence_size<sequence<L, R>>
    {
        static int const value =
            sequence_size<L>::value + sequence_size<R>::value;
    };

    struct pass_sequence_attribute_unused
    {
        typedef unused_type type;

        template <typename T>
        static unused_type
        call(T&)
        {
            return unused_type();
        }
    };

    template <typename Attribute>
    struct pass_sequence_attribute_front
    {
        typedef typename fusion::result_of::front<Attribute>::type type;

        static typename add_reference<type>::type
        call(Attribute& attr)
        {
            return fusion::front(attr);
        }
    };

    template <typename Attribute>
    struct pass_through_sequence_attribute
    {
        typedef Attribute type;

        template <typename Attribute_>
        static Attribute_&
        call(Attribute_& attr)
        {
            return attr;
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_used :
        pass_sequence_attribute_front<Attribute> {};


    //~ template <typename Parser, typename Attribute>
    //~ struct pass_sequence_attribute_used :
        //~ mpl::if_<
            //~ traits::is_substitute<Attribute, typename traits::attribute_of<Parser>::type>
          //~ , pass_through_sequence_attribute<Attribute>
          //~ , pass_sequence_attribute_front<Attribute>>::type {};

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute :
        mpl::if_<
            fusion::result_of::empty<Attribute>
          , pass_sequence_attribute_unused
          , pass_sequence_attribute_used<Parser, Attribute>>::type {};

    template <typename L, typename R, typename Attribute>
    struct pass_sequence_attribute<sequence<L, R>, Attribute>
      : pass_through_sequence_attribute<Attribute> {};

    template <typename L, typename R, typename Attribute>
    struct partition_attribute
    {
        static int const l_size = sequence_size<L>::value;
        static int const r_size = sequence_size<R>::value;

        // If you got an error here, then you are trying to pass
        // a fusion sequence with the wrong number of elements
        // as that expected by the (sequence) parser.
        static_assert(
            fusion::result_of::size<Attribute>::value == (l_size + r_size)
          , "Attribute does not have the expected size."
        );

        typedef typename fusion::result_of::begin<Attribute>::type l_begin;
        typedef typename fusion::result_of::advance_c<l_begin, l_size>::type l_end;
        typedef typename fusion::result_of::end<Attribute>::type r_end;
        typedef fusion::iterator_range<l_begin, l_end> l_range;
        typedef fusion::iterator_range<l_end, r_end> r_range;
        typedef pass_sequence_attribute<L, l_range> l_pass;
        typedef pass_sequence_attribute<R, r_range> r_pass;

        static l_range left(Attribute& s)
        {
            auto i = fusion::begin(s);
            return l_range(i, fusion::advance_c<l_size>(i));
        }

        static r_range right(Attribute& s)
        {
            return r_range(
                fusion::advance_c<l_size>(fusion::begin(s))
              , fusion::end(s));
        }
    };

    template <typename L, typename R>
    struct get_sequence_types
    {
        typedef
            mpl::vector<
                typename traits::attribute_of<L>::type
              , typename traits::attribute_of<R>::type
            >
        type;
    };

    template <typename LL, typename LR, typename R>
    struct get_sequence_types<sequence<LL, LR>, R>
    {
        typedef typename
            mpl::push_front<
                typename get_sequence_types<LL, LR>::type
              , typename traits::attribute_of<R>::type
            >::type
        type;
    };

    template <typename L, typename RL, typename RR>
    struct get_sequence_types<L, sequence<RL, RR>>
    {
        typedef typename
            mpl::push_back<
                typename get_sequence_types<RL, RR>::type
              , typename traits::attribute_of<L>::type
            >::type
        type;
    };

    template <typename LL, typename LR, typename RL, typename RR>
    struct get_sequence_types<sequence<LL, LR>, sequence<RL, RR>>
    {
        typedef
            mpl::joint_view<
                typename get_sequence_types<LL, LR>::type
              , typename get_sequence_types<RL, RR>::type
            >
        type;
    };

    template <typename L, typename R>
    struct attribute_of_sequence
    {
        // Get all sequence attribute types
        typedef typename get_sequence_types<L, R>::type all_types;

        // Filter all unused_types
        typedef typename
            mpl::copy_if<
                all_types
              , mpl::not_<is_same<mpl::_1, unused_type>>
              , mpl::back_inserter<mpl::vector<>>
            >::type
        filtered_types;

        // Build a fusion::deque if filtered_types is not empty,
        // else just return unused_type
        typedef typename
            mpl::eval_if<
                mpl::empty<filtered_types>
              , mpl::identity<unused_type>
              , fusion::result_of::as_deque<filtered_types>
            >::type
        type;
    };

    template <typename Left, typename Right
      , typename Iterator, typename Context, typename Attribute>
    bool parse_sequence(
        Left const& left, Right const& right
      , Iterator& first, Iterator const& last
      , Context& context, Attribute& attr, traits::tuple_attribute)
    {
        typedef detail::partition_attribute<Left, Right, Attribute> partition;
        typedef typename partition::l_pass l_pass;
        typedef typename partition::r_pass r_pass;

        typename partition::l_range left_seq = partition::left(attr);
        typename partition::r_range right_seq = partition::right(attr);
        typename l_pass::type l_attr = l_pass::call(left_seq);
        typename r_pass::type r_attr = r_pass::call(right_seq);

        Iterator save = first;
        if (left.parse(first, last, context, l_attr)
            && right.parse(first, last, context, r_attr))
            return true;
        first = save;
        return false;
    }

    template <typename Left, typename Right
      , typename Iterator, typename Context, typename Attribute>
    bool parse_sequence(
        Left const& left, Right const& right
      , Iterator& first, Iterator const& last
      , Context& context, Attribute& attr, traits::plain_attribute)
    {
        typedef typename traits::attribute_of<Left>::type l_attr_type;
        typedef typename traits::attribute_of<Right>::type r_attr_type;
        typedef traits::make_attribute<l_attr_type, Attribute> l_make_attribute;
        typedef traits::make_attribute<r_attr_type, Attribute> r_make_attribute;

        typename l_make_attribute::type l_attr = l_make_attribute::call(attr);
        typename r_make_attribute::type r_attr = r_make_attribute::call(attr);

        Iterator save = first;
        if (left.parse(first, last, context, l_attr)
            && right.parse(first, last, context, r_attr))
            return true;
        first = save;
        return false;
    }

    template <typename Left, typename Right
      , typename Iterator, typename Context, typename Attribute>
    bool parse_sequence(
        Left const& left, Right const& right
      , Iterator& first, Iterator const& last
      , Context& context, Attribute& attr)
    {
        return parse_sequence(left, right, first, last, context, attr
          , typename traits::attribute_category<Attribute>::type());
    }

}}}}

#endif
