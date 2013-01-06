/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_SEQUENCE_DETAIL_JAN_06_2012_1015AM)
#define SPIRIT_SEQUENCE_DETAIL_JAN_06_2012_1015AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/mpl/if.hpp>

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
        static int const value = Parser::has_attribute;
    };

    template <typename L, typename R>
    struct sequence_size<sequence<L, R>>
    {
        static int const value =
            sequence_size<L>::value + sequence_size<R>::value;
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_unused
    {
        typedef unused_type type;

        template <typename Attribute_>
        static unused_type
        call(Attribute_& attr)
        {
            return unused_type();
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_used
    {
        typedef typename fusion::result_of::front<Attribute>::type type;

        template <typename Attribute_>
        static typename add_reference<type>::type
        call(Attribute_& attr)
        {
            return fusion::front(attr);
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute :
        mpl::if_<fusion::result_of::empty<Attribute>
          , pass_sequence_attribute_unused<Parser, Attribute>
          , pass_sequence_attribute_used<Parser, Attribute>>::type
    {
    };

    template <typename L, typename R, typename Attribute>
    struct pass_sequence_attribute<sequence<L, R>, Attribute>
    {
        typedef Attribute type;

        template <typename Attribute_>
        static Attribute_&
        call(Attribute_& attr)
        {
            return attr;
        }
    };

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
}}}}

#endif
