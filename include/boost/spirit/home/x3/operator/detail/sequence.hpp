/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SEQUENCE_DETAIL_JAN_06_2013_1015AM)
#define BOOST_SPIRIT_X3_SEQUENCE_DETAIL_JAN_06_2013_1015AM

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_category.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/is_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/iterator_range.hpp>

#include <boost/mpl/if.hpp>

#include <iterator>

namespace boost::spirit::x3
{
    template <typename Left, typename Right>
    struct sequence;
} // boost::spirit::x3

namespace boost::spirit::x3::detail
{
    template <typename Parser, typename Context, typename Enable = void>
    struct sequence_size
    {
        static int const value = traits::has_attribute<Parser, Context>::value;
    };

    template <typename Parser, typename Context>
    struct sequence_size_subject
      : sequence_size<typename Parser::subject_type, Context> {};

    template <typename Parser, typename Context>
    struct sequence_size<Parser, Context
      , typename enable_if_c<(Parser::is_pass_through_unary)>::type>
      : sequence_size_subject<Parser, Context> {};

    template <typename L, typename R, typename Context>
    struct sequence_size<sequence<L, R>, Context>
    {
        static int const value =
            sequence_size<L, Context>::value +
            sequence_size<R, Context>::value;
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
    struct pass_sequence_attribute_size_one_view
    {
        typedef typename fusion::result_of::deref<
            typename fusion::result_of::begin<Attribute>::type
        >::type type;

        static type call(Attribute& attribute)
        {
            return fusion::deref(fusion::begin(attribute));
        }
    };

    template <typename Attribute>
    struct pass_through_sequence_attribute
    {
        typedef Attribute& type;

        template <typename Attribute_>
        static Attribute_&
        call(Attribute_& attribute)
        {
            return attribute;
        }
    };

    template <typename Parser, typename Attribute, typename Enable = void>
    struct pass_sequence_attribute :
        mpl::if_<
            traits::is_size_one_view<Attribute>
          , pass_sequence_attribute_size_one_view<Attribute>
          , pass_through_sequence_attribute<Attribute>>::type {};

    template <typename L, typename R, typename Attribute>
    struct pass_sequence_attribute<sequence<L, R>, Attribute>
      : pass_through_sequence_attribute<Attribute> {};

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_subject :
        pass_sequence_attribute<typename Parser::subject_type, Attribute> {};

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute<Parser, Attribute
      , typename enable_if_c<(Parser::is_pass_through_unary)>::type>
      : pass_sequence_attribute_subject<Parser, Attribute> {};

    template <typename L, typename R, typename Attribute, typename Context
      , typename Enable = void>
    struct partition_attribute
    {
        using attr_category = typename traits::attribute_category<Attribute>::type;
        static_assert(is_same<traits::tuple_attribute, attr_category>::value,
            "The parser expects tuple-like attribute type");

        static int const l_size = sequence_size<L, Context>::value;
        static int const r_size = sequence_size<R, Context>::value;

        static int constexpr actual_size = fusion::result_of::size<Attribute>::value;
        static int constexpr expected_size = l_size + r_size;

        // If you got an error here, then you are trying to pass
        // a fusion sequence with the wrong number of elements
        // as that expected by the (sequence) parser.
        static_assert(
            actual_size >= expected_size
          , "Size of the passed attribute is less than expected."
        );
        static_assert(
            actual_size <= expected_size
          , "Size of the passed attribute is bigger than expected."
        );

        typedef typename fusion::result_of::begin<Attribute>::type l_begin;
        typedef typename fusion::result_of::advance_c<l_begin, l_size>::type l_end;
        typedef typename fusion::result_of::end<Attribute>::type r_end;
        typedef fusion::iterator_range<l_begin, l_end> l_part;
        typedef fusion::iterator_range<l_end, r_end> r_part;
        typedef pass_sequence_attribute<L, l_part> l_pass;
        typedef pass_sequence_attribute<R, r_part> r_pass;

        static l_part left(Attribute& s)
        {
            auto i = fusion::begin(s);
            return l_part(i, fusion::advance_c<l_size>(i));
        }

        static r_part right(Attribute& s)
        {
            return r_part(
                fusion::advance_c<l_size>(fusion::begin(s))
              , fusion::end(s));
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(!traits::has_attribute<L, Context>::value &&
            traits::has_attribute<R, Context>::value)>::type>
    {
        typedef unused_type l_part;
        typedef Attribute& r_part;
        typedef pass_sequence_attribute_unused l_pass;
        typedef pass_sequence_attribute<R, Attribute> r_pass;

        static unused_type left(Attribute&)
        {
            return unused;
        }

        static Attribute& right(Attribute& s)
        {
            return s;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(traits::has_attribute<L, Context>::value &&
            !traits::has_attribute<R, Context>::value)>::type>
    {
        typedef Attribute& l_part;
        typedef unused_type r_part;
        typedef pass_sequence_attribute<L, Attribute> l_pass;
        typedef pass_sequence_attribute_unused r_pass;

        static Attribute& left(Attribute& s)
        {
            return s;
        }

        static unused_type right(Attribute&)
        {
            return unused;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(!traits::has_attribute<L, Context>::value &&
            !traits::has_attribute<R, Context>::value)>::type>
    {
        typedef unused_type l_part;
        typedef unused_type r_part;
        typedef pass_sequence_attribute_unused l_pass;
        typedef pass_sequence_attribute_unused r_pass;

        static unused_type left(Attribute&)
        {
            return unused;
        }

        static unused_type right(Attribute&)
        {
            return unused;
        }
    };

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute, typename AttributeCategory>
    bool parse_sequence(
        Parser const& parser, Iterator& first, Iterator const& last
      , Context const& context, RContext& rcontext, Attribute& attr
      , AttributeCategory)
    {
        using Left = typename Parser::left_type;
        using Right = typename Parser::right_type;
        using partition = partition_attribute<Left, Right, Attribute, Context>;
        using l_pass = typename partition::l_pass;
        using r_pass = typename partition::r_pass;

        typename partition::l_part l_part = partition::left(attr);
        typename partition::r_part r_part = partition::right(attr);
        typename l_pass::type l_attr = l_pass::call(l_part);
        typename r_pass::type r_attr = r_pass::call(r_part);

        Iterator save = first;
        if (parser.left.parse(first, last, context, rcontext, l_attr)
            && parser.right.parse(first, last, context, rcontext, r_attr))
            return true;
        first = save;
        return false;
    }

    template <typename Parser, typename Context>
    constexpr bool pass_sequence_container_attribute
        = sequence_size<Parser, Context>::value > 1;

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    typename enable_if_c<pass_sequence_container_attribute<Parser, Context>, bool>::type
    parse_sequence_container(
        Parser const& parser
      , Iterator& first, Iterator const& last, Context const& context
      , RContext& rcontext, Attribute& attr)
    {
        return parser.parse(first, last, context, rcontext, attr);
    }

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    typename disable_if_c<pass_sequence_container_attribute<Parser, Context>, bool>::type
    parse_sequence_container(
        Parser const& parser
      , Iterator& first, Iterator const& last, Context const& context
      , RContext& rcontext, Attribute& attr)
    {
        return parse_into_container(parser, first, last, context, rcontext, attr);
    }

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    bool parse_sequence(
        Parser const& parser , Iterator& first, Iterator const& last
      , Context const& context, RContext& rcontext, Attribute& attr
      , traits::container_attribute)
    {
        Iterator save = first;
        if (parse_sequence_container(parser.left, first, last, context, rcontext, attr)
            && parse_sequence_container(parser.right, first, last, context, rcontext, attr))
            return true;
        first = save;
        return false;
    }

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    bool parse_sequence_assoc(
        Parser const& parser , Iterator& first, Iterator const& last
	  , Context const& context, RContext& rcontext, Attribute& attr, mpl::false_ /*should_split*/)
    {
	    return parse_into_container(parser, first, last, context, rcontext, attr);
    }

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    bool parse_sequence_assoc(
        Parser const& parser , Iterator& first, Iterator const& last
	  , Context const& context, RContext& rcontext, Attribute& attr, mpl::true_ /*should_split*/)
    {
        Iterator save = first;
        if (parser.left.parse( first, last, context, rcontext, attr)
            && parser.right.parse(first, last, context, rcontext, attr))
            return true;
        first = save;
        return false;
    }

    template <typename Parser, typename Iterator, typename Context
      , typename RContext, typename Attribute>
    bool parse_sequence(
        Parser const& parser, Iterator& first, Iterator const& last
      , Context const& context, RContext& rcontext, Attribute& attr
      , traits::associative_attribute)
    {
        // we can come here in 2 cases:
        // - when sequence is key >> value and therefore must
        // be parsed with tuple synthesized attribute and then
        // that tuple is used to save into associative attribute provided here.
        // Example:  key >> value;
        //
        // - when either this->left or this->right provides full key-value
        // pair (like in case 1) and another one provides nothing.
        // Example:  eps >> rule<class x; fusion::map<...> >
        //
        // first case must be parsed as whole, and second one should
        // be parsed separately for left and right.

        typedef typename traits::attribute_of<
            decltype(parser.left), Context>::type l_attr_type;
        typedef typename traits::attribute_of<
            decltype(parser.right), Context>::type r_attr_type;

        typedef typename
            mpl::or_<
                is_same<l_attr_type, unused_type>
              , is_same<r_attr_type, unused_type> >
        should_split;

        return parse_sequence_assoc(parser, first, last, context, rcontext, attr
          , should_split());
    }

    template <typename Left, typename Right, typename Context, typename RContext>
    struct parse_into_container_impl<sequence<Left, Right>, Context, RContext>
    {
        using parser_type = sequence<Left, Right>;

        template <typename Attribute>
        static constexpr bool is_container_substitute = traits::is_substitute_v<
            traits::attribute_of_t<parser_type, Context>,
            traits::container_value_t<Attribute>
        >;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires is_container_substitute<Attribute>
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) noexcept(noexcept(parse_into_container_base_impl<parser_type>::call(
            parser, first, last, context, rcontext, attr
        )))
        {
            return parse_into_container_base_impl<parser_type>::call(
                parser, first, last, context, rcontext, attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires (!is_container_substitute<Attribute>)
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) // never noexcept (requires container insertion)
        {
            // inform user what went wrong if we jumped here in attempt to
            // parse incompatible sequence into fusion::map
            static_assert(
                !std::is_same_v<traits::attribute_category_t<Attribute>, traits::associative_attribute>,
                "To parse directly into fusion::map sequence must produce tuple attribute "
                "where type of first element is existing key in fusion::map and second element "
                "is value to be stored under that key"
            );

            Attribute attr_{};
            if (!detail::parse_sequence(
                parser, first, last, context, rcontext, attr_, traits::container_attribute()
            ))
            {
                return false;
            }
            traits::append(
                attr,
                std::make_move_iterator(traits::begin(attr_)),
                std::make_move_iterator(traits::end(attr_))
            );
            return true;
        }
    };

} // boost::spirit::x3::detail

#endif
