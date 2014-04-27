/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_DIRECTIVE_HPP_INCLUDED
#define BOOST_SPIRIT_X3_DIRECTIVE_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <tuple>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/detail/eval.hpp>
#include <boost/spirit/home/x3/core/detail/transform_params.hpp>
#include <boost/spirit/home/x3/support/utility/integer_sequence.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/handles_container.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Directive, typename = void>
    struct caller_is_pass_through_unary
      : mpl::bool_<Directive::is_pass_through_unary> {};
      
    template <typename Directive>
    struct caller_is_pass_through_unary<Directive,
        typename disable_if_substitution_failure<
            mpl::bool_<Directive::caller_is_pass_through_unary>>::type>
      : mpl::bool_<Directive::caller_is_pass_through_unary> {};
}}}}

namespace boost { namespace spirit { namespace x3
{
    template <typename Directive, typename Subject>
    struct directive_parser : unary_parser<Subject, directive_parser<Directive, Subject>>
    {
        typedef unary_parser<Subject, directive_parser<Directive, Subject>> base_type;
        static bool const is_pass_through_unary = Directive::is_pass_through_unary;
        static bool const handles_container = Subject::handles_container;

        directive_parser(Directive const& directive, Subject const& subject)
          : base_type(subject), directive(directive) {}
          
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return directive.parse(this->subject, first, last, context, attr);
        }
        
        Directive directive;
    };
    
    template <typename Directive, typename... Ts>
    struct directive_caller
    {
        typedef detail::transform_params<Directive, void, Ts...> transform;
        typedef typename transform::tag transform_tag;
        static bool const is_pass_through_unary =
            detail::caller_is_pass_through_unary<Directive>::value;
        
        template <typename... As>
        directive_caller(Directive const& directive, As&&... as)
          : directive_caller(transform_tag(),
                directive, std::forward<As>(as)...)
        {}
          
        template <typename... As>
        directive_caller(mpl::true_, Directive const& directive, As&&... as)
          : directive(directive)
          , params(Directive::transform_params(std::forward<As>(as)...))
        {}
          
        template <typename... As>
        directive_caller(mpl::false_, Directive const& directive, As&&... as)
          : directive(directive), params(std::forward<As>(as)...) {}

        template <typename Subject>
        directive_parser<directive_caller, typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return {*this, subject};
        }
        
        template <typename Subject, typename Iterator, typename Context, typename Attribute>
        bool parse(Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            transform_tag tag;
            make_index_sequence<sizeof...(Ts)> indices;
            return parse_impl(tag, indices, subject, first, last, context, attr);
        }
        
        // transformed
        template <std::size_t... Ns, typename Subject
            , typename Iterator, typename Context, typename Attribute>
        bool parse_impl(mpl::true_, index_sequence<Ns...>
          , Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return directive.parse(
                subject, first, last, context, attr, params);
        }
        
        // no transform_params
        template <std::size_t... Ns, typename Subject
            , typename Iterator, typename Context, typename Attribute>
        bool parse_impl(mpl::false_, index_sequence<Ns...>
          , Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return invoke_parse(subject, first, last, context, attr,
                detail::eval<Ts const&>(std::get<Ns>(params), context)...);
        }
        
        template <typename Subject, typename Iterator, typename Context
            , typename Attribute, typename... As>
        typename detail::transform_params<Directive, void, As...>::yes
        invoke_parse(Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, As&&... as) const
        {
            return directive.parse(subject, first, last, context, attr,
                Subject::transform_params(std::forward<As>(as)...));
        }
        
        template <typename Subject, typename Iterator, typename Context
            , typename Attribute, typename... As>
        typename detail::transform_params<Directive, void, As...>::no
        invoke_parse(Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, As&&... as) const
        {
            return directive.parse(
                subject, first, last, context, attr, std::forward<As>(as)...);
        }
        
        Directive directive;
        typename transform::type params;
    };

    template <typename Derived>
    struct directive
    {
        static bool const is_pass_through_unary = false;
        
        template <typename Subject>
        using traits = unused_type;

        template <typename Subject, typename... Ts>
        struct caller_traits : Derived::template traits<Subject> {};

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }

        template <typename Subject>
        directive_parser<Derived, typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return {derived(), subject};
        }
        
        template <typename... Ts>
        directive_caller<Derived, unrefcv<Ts>...>
        operator()(Ts&&... ts) const
        {
            return {derived(), std::forward<Ts>(ts)...};
        }
    };
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    // attribute_of
    template <typename Directive, typename Subject, typename Context>
    struct attribute_of<directive_parser<Directive, Subject>, Context,
        typename disable_if_substitution_failure<
            typename Directive::template traits<Subject>::attribute_type>::type>
      : mpl::identity<typename Directive::template traits<Subject>::attribute_type>
    {};
    
    template <typename Directive, typename Subject, typename Context>
    struct attribute_of<directive_parser<Directive, Subject>, Context,
        typename disable_if_substitution_failure<
            typename Directive::template traits<Subject>::template attribute<Context>::type>::type>
      : mpl::identity<typename Directive::template traits<Subject>::template attribute<Context>::type>
    {};
    
    // has_attribute
    template <typename Directive, typename Subject, typename Context>
    struct has_attribute<directive_parser<Directive, Subject>, Context,
        typename disable_if_substitution_failure<
            mpl::bool_<Directive::template traits<Subject>::has_attribute>>::type>
      : mpl::bool_<Directive::template traits<Subject>::has_attribute> {};
      
    // handles_container
    template <typename Directive, typename Subject, typename Context>
    struct handles_container<directive_parser<Directive, Subject>, Context,
        typename disable_if_substitution_failure<
            mpl::bool_<Directive::template traits<Subject>::handles_container>>::type>
      : mpl::bool_<Directive::template traits<Subject>::handles_container> {};

    // Caller
    //--------------------------------------------------------------------------
    // attribute_of
    template <typename Directive, typename... Ts, typename Subject, typename Context>
    struct attribute_of<directive_parser<directive_caller<Directive, Ts...>, Subject>,
        Context, typename disable_if_substitution_failure<
            typename Directive::template caller_traits<Subject,
                x3::detail::result_of_eval<Ts, Context>...>::attribute_type>::type>
      : mpl::identity<typename Directive::template caller_traits<Subject,
            x3::detail::result_of_eval<Ts, Context>...>::attribute_type>
    {};

    template <typename Directive, typename... Ts, typename Subject, typename Context>
    struct attribute_of<directive_parser<directive_caller<Directive, Ts...>, Subject>,
        Context, typename disable_if_substitution_failure<
            typename Directive::template caller_traits<Subject,
                x3::detail::result_of_eval<Ts, Context>...>::
                    template attribute<Context>::type>::type>
      : mpl::identity<typename Directive::template caller_traits<Subject,
            x3::detail::result_of_eval<Ts, Context>...>::
                template attribute<Context>::type>
    {};
    
    // has_attribute
    template <typename Directive, typename... Ts, typename Subject, typename Context>
    struct has_attribute<directive_parser<directive_caller<Directive, Ts...>, Subject>,
        Context, typename disable_if_substitution_failure<
            mpl::bool_<Directive::template caller_traits<Subject,
                x3::detail::result_of_eval<Ts, Context>...>::has_attribute>>::type>
      : mpl::bool_<Directive::template caller_traits<Subject,
            x3::detail::result_of_eval<Ts, Context>...>::has_attribute>
    {};
    
    // handles_container
    template <typename Directive, typename... Ts, typename Subject, typename Context>
    struct handles_container<directive_parser<directive_caller<Directive, Ts...>, Subject>,
        Context, typename disable_if_substitution_failure<
            mpl::bool_<Directive::template caller_traits<Subject,
                x3::detail::result_of_eval<Ts, Context>...>::handles_container>>::type>
      : mpl::bool_<Directive::template caller_traits<Subject,
            x3::detail::result_of_eval<Ts, Context>...>::handles_container>
    {};
}}}}


#endif

