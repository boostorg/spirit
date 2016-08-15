/*=============================================================================
Copyright (c) 2016  Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////
// Implementation of variadic operator "nabialek. 
//
// The first parser acts as selector and is expected to expose an int attribute.
// The selector does not contribute to the nabialek attribute.
// It's attribute is taken as an index to select the parser to be executed.
// 
// If the selector succeeds, the parser of that index gets executed and it's
// result returned.
//
// This example illustrates how a functionality as currently provided
// by qi::/ and qi::kwd can be implemented with PR#200 n-ary operators.
//
// The implementation shows how to implement a variadic operator
// which lives in a user's namespace (here mxc::qitoo).
//
// This is for demonstration only. It has to be questioned, if that would be 
// an optimal way to implement nabialek. I don't think so.
// I wanted to stay near the implementation of / and kwd.
// 
///////////////////////////////////////////////////////////////////////////

#if !defined(BOOST_SPIRIT_TEST_NABIALEK_FUNCTION)
#define BOOST_SPIRIT_TEST_NABIALEK_FUNCTION

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/tuple/tuple.hpp>

namespace mxc { namespace qitoo {
    BOOST_SPIRIT_TERMINAL(nabialek)
}}

namespace boost { namespace spirit {
    template <>
    struct use_function<qi::domain, mxc::qitoo::tag::nabialek>
        : mpl::true_ {};
}}

namespace mxc { namespace qitoo {

    namespace detail {

        typedef std::pair<int, int> nabialek_data;

        template<typename F>
        struct nabialek_function
        {
            nabialek_function(F const& f, nabialek_data& idx)
                : f(f), idx(idx)
            {}

            template<typename Component>
            bool operator()(Component& component) const
            {
                // return false while target index != current index
                if (idx.first != idx.second) {
                    idx.second++;
                    return false;
                }
                // current index == target index
                return !f(component);
            }
            F const& f;
            nabialek_data& idx;
        };
    }

    namespace fusion = boost::fusion;
    namespace spirit = boost::spirit;
    namespace qi = spirit::qi;

    template <typename Elements>
    struct nabialek_parser : qi::nary_parser<nabialek_parser<Elements>>
    {

        nabialek_parser(Elements elements)
            : elements(elements)
        {}

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef typename fusion::result_of::pop_front<Elements>::type optionals;

            // Put all the element attributes in a tuple
            // #todo (fhe): implement spirit::traits::nabialek_attribute_transform hook 
            //              if this was ever considered for release
            typedef typename spirit::traits::build_attribute_sequence <
                optionals, Context, spirit::traits::sequence_attribute_transform
                , Iterator, qi::domain
            >::type all_attributes;
                
            typedef typename spirit::traits::build_variant<all_attributes>::type type;
        };

        template <typename F>
        bool parse_container(F f) const
        {
            detail::nabialek_data idx(-1, 0);
            if (!fusion::at_c<0>(elements).parse(f.f.first, f.f.last, 
                    f.f.context, f.f.skipper, idx.first))
                return false;

            detail::nabialek_function<F> nabialek(f, idx);

            return fusion::any(elements, nabialek); // selected parser might fail
        }

        template <typename Iterator, typename Context, typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
            , Context& context, Skipper const& skipper
            , Attribute& attr_) const
        {
            spirit::traits::make_container(attr_);
            qi::detail::fail_function<Iterator, Context, Skipper>
                f(first, last, context, skipper);
            return parse_container(qi::detail::make_pass_container(f, attr_));
        }

        template <typename Context>
        qi::info what(Context& context) const
        {
            qi::info result("nabialek");
            fusion::for_each(elements,
                spirit::detail::what_function<Context>(result, context));

            return result;
        }

        Elements elements;
    };
}}

namespace boost { namespace spirit {

    namespace qi {
        ///////////////////////////////////////////////////////////////////////////
        // Parser generator: make_xxx function (objects)
        ///////////////////////////////////////////////////////////////////////////
        template <typename Expr, typename Modifiers>
        struct make_composite<mxc::qitoo::tag::nabialek, Expr, Modifiers>
            : make_nary_composite<Expr, mxc::qitoo::nabialek_parser>
        { };
    }

    namespace traits {
        ///////////////////////////////////////////////////////////////////////////
        template <typename Elements>
        struct has_semantic_action<mxc::qitoo::nabialek_parser<Elements> >
            : nary_has_semantic_action<Elements> {};

        ///////////////////////////////////////////////////////////////////////////
        template <typename Elements, typename Attribute, typename Context
            , typename Iterator>
            struct handles_container<mxc::qitoo::nabialek_parser<Elements>, Attribute, Context
            , Iterator>
            : mpl::true_ {};
    }
}}

#endif

