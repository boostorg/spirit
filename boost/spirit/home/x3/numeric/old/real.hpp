/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman
    Copyright (c) 2011      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_REAL_APRIL_18_2006_0850AM)
#define BOOST_SPIRIT_REAL_APRIL_18_2006_0850AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/lit.hpp>
#include <boost/spirit/home/x3/core/compiler.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/numeric/real_policies.hpp>
#include <boost/spirit/home/x3/numeric/numeric_utils.hpp>
#include <boost/spirit/home/x3/numeric/detail/real_impl.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////
    // forward declaration only
    template <typename T>
    struct real_policies;

    ///////////////////////////////////////////////////////////////////////
    // This is the class that the user can instantiate directly in
    // order to create a customized real parser
    template <typename T = double, typename Policies = real_policies<T>>
    struct real_parser
      : spirit::terminal<real_parser<T, Policies>>
    {};

    struct float_type : terminal<float_type> {};
    struct double_type : terminal<double_type> {};
    struct long_double_type : terminal<long_double_type> {};

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
    float_type const float_ = float_type();
    double_type const double_ = double_type();
    long_double_type const long_double = long_double_type();
#endif

    ///////////////////////////////////////////////////////////////////////////
    // This is the actual real number parser
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename RealPolicies = real_policies<T> >
    struct any_real_parser
      : parser<any_real_parser<T, RealPolicies> >
    {
        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef T type;
        };

        template <typename Iterator, typename Context, typename Skipper>
        bool parse(Iterator& first, Iterator const& last
          , Context& /*context*/, Skipper const& skipper
          , T& attr) const
        {
            typedef detail::real_impl<T, RealPolicies> extract;
            x3::skip_over(first, last, skipper);
            return extract::parse(first, last, attr, RealPolicies());
        }

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            // this case is called when Attribute is not T
            T attr_;
            if (parse(first, last, context, skipper, attr_))
            {
                traits::assign_to(attr_, attr);
                return true;
            }
            return false;
        }
    };

    template <typename T, typename RealPolicies>
    struct get_info<any_real_parser<T, RealPolicies>>
    {
        typedef std::string result_type;

        template <typename P>
        std::string operator()(P const& p) const
        {
            return "real number";
        }
    };

    template <typename T, typename RealPolicies = real_policies<T>
            , bool no_attribute = true>
    struct literal_real_parser
      : parser<literal_real_parser<T, RealPolicies, no_attribute> >
    {
        template <typename Value>
        literal_real_parser(Value const& n) : n_(n) {}

        template <typename Context, typename Iterator>
        struct attribute
          : mpl::if_c<no_attribute, unused_type, T>
        {};

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context&, Skipper const& skipper
          , Attribute& attr) const
        {
            typedef detail::real_impl<T, RealPolicies> extract;
            x3::skip_over(first, last, skipper);

            Iterator save = first;
            T attr_;

            if (extract::parse(first, last, attr_, RealPolicies()) &&
                (attr_ == n_))
            {
                traits::assign_to(attr_, attr);
                return true;
            }

            first = save;
            return false;
        }

        T n_;
    };

    template <typename T, typename RealPolicies, bool no_attribute>
    struct get_info<literal_real_parser<T, RealPolicies, no_attribute>>
    {
        typedef std::string result_type;

        template <typename P>
        std::string operator()(P const& p) const
        {
            return "real number";
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser compilers
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Policies = real_policies<T>>
    struct make_real
    {
        typedef any_real_parser<T, Policies> result_type;

        result_type operator()(unused_type) const
        {
            return result_type();
        }
    };

    template <typename T, typename Policies = real_policies<T>>
    struct make_direct_real
    {
        typedef literal_real_parser<T, Policies, false> result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term) const
        {
            return result_type(T(fusion::at_c<0>(term.args)));
        }
    };

    template <typename T, typename Policies = real_policies<T>>
    struct make_literal_real
    {
        typedef literal_real_parser<T, Policies> result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<float>>>
      : make_literal_real<float> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<double>>>
      : make_literal_real<double> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<long double>>>
      : make_literal_real<long double> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Policies>
    struct compiler<
        real_parser<T, Policies>>
      : make_real<T, Policies> {};

    template <typename T, typename Policies, typename A0>
    struct compiler<
        terminal_ex<real_parser<T, Policies>, fusion::vector1<A0>>>
      : make_direct_real<T, Policies> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<float_type>
      : make_real<float> {};

    template <typename A0>
    struct compiler<terminal_ex<float_type, fusion::vector1<A0>>>
      : make_direct_real<float> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<double_type>
      : make_real<double> {};

    template <typename A0>
    struct compiler<
        terminal_ex<double_type, fusion::vector1<A0>>>
      : make_direct_real<double> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<long_double_type>
      : make_real<long double> {};

    template <typename A0>
    struct compiler<
        terminal_ex<long_double_type, fusion::vector1<A0>>>
      : make_direct_real<long double> {};
}}}

#endif
