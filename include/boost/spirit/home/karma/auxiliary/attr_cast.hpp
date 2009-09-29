//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_ATTR_CAST_SEP_26_2009_0348PM)
#define SPIRIT_KARMA_ATTR_CAST_SEP_26_2009_0348PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/detail/output_iterator.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/attributes.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // enables attr_cast<>() pseudo generator
    template <typename Expr, typename Attribute>
    struct use_terminal<karma::domain
          , tag::stateful_tag<Expr, tag::attr_cast, Attribute> >
      : mpl::true_ {};

    ///////////////////////////////////////////////////////////////////////////
    // This one is the function that the user can call directly in order 
    // to create a customized attr_cast component
    template <typename Attribute, typename Data>
    stateful_tag_type<Data, tag::attr_cast, Attribute>
    attr_cast(Data const& data)
    {
        return stateful_tag_type<Data, tag::attr_cast, Attribute>(data);
    }
}}

namespace boost { namespace spirit { namespace karma
{
    using spirit::attr_cast;

    ///////////////////////////////////////////////////////////////////////////
    // attr_cast_generator consumes the attribute of subject generator without
    // generating anything
    ///////////////////////////////////////////////////////////////////////////
    template <typename Attribute, typename Subject>
    struct attr_cast_generator
      : unary_generator<attr_cast_generator<Attribute, Subject> >
    {
        typedef typename result_of::compile<karma::domain, Subject>::type
            subject_type;

        typedef mpl::int_<subject_type::properties::value> properties;

        attr_cast_generator(Subject const& subject)
          : subject(subject) 
        {
            // If you got an error_invalid_expression error message here,
            // then the expression (Subject) is not a valid spirit karma
            // expression.
            BOOST_SPIRIT_ASSERT_MATCH(karma::domain, Subject);
        }

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef Attribute type;
        };

        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute_>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute_ const& attr) const
        {
            // create the proper attribute transformation
            typedef traits::transform_attribute<
                Attribute, Attribute_ const> transform;

            return compile<karma::domain>(subject).
                generate(sink, ctx, d, transform::call(attr));
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("attr_cast"
              , compile<karma::domain>(subject).what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generator: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Expr, typename Attribute, typename Modifiers>
    struct make_primitive<
        tag::stateful_tag<Expr, tag::attr_cast, Attribute>, Modifiers>
    {
        typedef attr_cast_generator<Attribute, Expr> result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            typedef tag::stateful_tag<Expr, tag::attr_cast, Attribute> tag_type;
            using spirit::detail::get_stateful_data;
            return result_type(get_stateful_data<tag_type>::call(term));
        }
    };

}}}

#endif
