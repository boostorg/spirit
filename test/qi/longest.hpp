/*=============================================================================
Copyright (c) 2016  Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////
// Implementation of variadic operator "longest. Returns the attribute of 
// that argument parser, which consumed most of the input while succeeding
//
// The implementation shows how to implement a variadic operator
// which lives in a user's namespace (here mxc::qitoo).
//
// This is for demonstration only. A real implementation should obviously have
// to care about the input consumed by the skipper.
///////////////////////////////////////////////////////////////////////////

#if !defined(BOOST_SPIRIT_TEST_LONGEST_FUNCTION)
#define BOOST_SPIRIT_TEST_LONGEST_FUNCTION

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/tuple/tuple.hpp>

namespace mxc {
    namespace qitoo {
        BOOST_SPIRIT_TERMINAL(longest)
    }
}

namespace boost {
    namespace spirit {
        template <>
        struct use_function<qi::domain, mxc::qitoo::tag::longest>
            : mpl::true_ {};
    }
}

namespace mxc {
    namespace qitoo {
        using boost::tuples::get;

        namespace detail {

            // to hold            max length, max length idx, current idx
            typedef boost::tuple< int,        int,            int> longest_data;

            template <typename Iterator, typename Context, typename Skipper>
            struct find_longest_function
            {
                find_longest_function(Iterator& first, Iterator const& last
                    , Context& context, Skipper const& skipper, longest_data& result)
                    : first(first), last(last), context(context), skipper(skipper), result(result)
                {}

                template<typename Component>
                void operator()(Component& component) const
                {
                    Iterator f = first;
                    if (component.parse(f, last, context, skipper, qi::unused))
                    {
                        int l = std::distance(first, f);
                        // if current length is > then stored max length 
                        // taking zero lenght matches in account
                        if (l > get<0>(result)) {
                            get<0>(result) = l;                 // update max lenght
                            get<1>(result) = get<2>(result);    // update target index accordingly
                        }
                    }
                    get<2>(result)++;                           // increment cursor
                }

                longest_data& result;
                Iterator& first;
                Iterator const& last;
                Context& context;
                Skipper const& skipper;
            };

            template <typename Iterator, typename Context, typename Skipper, typename Attribute>
            struct longest_function
            {
                longest_function(Iterator& first, Iterator const& last
                    , Context& context, Skipper const& skipper, Attribute& attr, longest_data& result)
                    : first(first), last(last), context(context), skipper(skipper), attr(attr), result(result)
                {}

                template<typename Component>
                bool operator()(Component& component) const
                {
                    // return false while target index != current index
                    if (get<1>(result) != get<2>(result)) {
                        get<2>(result)++;
                        return false;
                    }
                    // current index == target index
                    return component.parse(first, last, context, skipper, attr);
                }

                Attribute& attr;
                longest_data& result;
                Iterator& first;
                Iterator const& last;
                Context& context;
                Skipper const& skipper;
            };
        }

        namespace fusion = boost::fusion;
        namespace spirit = boost::spirit;
        namespace qi = spirit::qi;

        template <typename Elements>
        struct longest_parser : qi::nary_parser<longest_parser<Elements>>
        {

            longest_parser(Elements elements) 
                : elements(elements)
            {}

            template <typename Context, typename Iterator>
            struct attribute
            {
                typedef typename spirit::traits::build_variant<Elements>::type type;
            };

            template <typename Iterator, typename Context, typename Skipper, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
                , Context& context, Skipper const& skipper
                , Attribute& attr_) const
            {

                spirit::traits::make_container(attr_);
                detail::longest_data result(-1, 0, 0);

                detail::find_longest_function<Iterator, Context, Skipper>
                    find_longest(first, last, context, skipper, result);


                fusion::for_each(elements, find_longest);
                // if any of the parsers succeeded, the length of input consumed
                // is > -1
                if (get<0>(result) != -1) { 
                    get<2>(result) = 0;       // reset current index (cursor)

                    detail::longest_function<Iterator, Context, Skipper, Attribute>
                        longest(first, last, context, skipper, attr_, result);

                    return fusion::any(elements, longest); // always true
                }

                return false;
            }

            template <typename Context>
            qi::info what(Context& context) const
            {
                qi::info result("longest");
                fusion::for_each(elements,
                    spirit::detail::what_function<Context>(result, context));

                return result;
            }

            Elements elements;
        };
    }
}

namespace boost {
    namespace spirit {

        namespace qi {
            ///////////////////////////////////////////////////////////////////////////
            // Parser generator: make_xxx function (objects)
            ///////////////////////////////////////////////////////////////////////////
            template <typename Expr, typename Modifiers>
            struct make_composite<mxc::qitoo::tag::longest, Expr, Modifiers>
                : make_nary_composite<Expr, mxc::qitoo::longest_parser>
            { };
        }

        namespace traits {
            ///////////////////////////////////////////////////////////////////////////
            template <typename Elements>
            struct has_semantic_action<mxc::qitoo::longest_parser<Elements> >
                : nary_has_semantic_action<Elements> {};

            ///////////////////////////////////////////////////////////////////////////
            template <typename Elements, typename Attribute, typename Context
                , typename Iterator>
                struct handles_container<mxc::qitoo::longest_parser<Elements>, Attribute, Context
                , Iterator>
                : mpl::true_ {};
        }
    }
}

#endif
