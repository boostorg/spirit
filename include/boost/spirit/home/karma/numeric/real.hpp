//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_REAL_FEB_26_2007_0512PM)
#define BOOST_SPIRIT_KARMA_REAL_FEB_26_2007_0512PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config/no_tr1/cmath.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/string_traits.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/detail/get_encoding.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/char.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/auxiliary/lazy.hpp>
#include <boost/spirit/home/karma/detail/get_casetag.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/numeric/real_policies.hpp>
#include <boost/spirit/home/karma/numeric/detail/real_utils.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/include/vector.hpp>

namespace boost { namespace spirit 
{
    namespace tag
    {
        template <typename T, typename Policies>
        struct real_tag 
        {
            real_tag() {}
            real_tag(Policies const& policies)
              : policies_(policies) {}

            Policies policies_;
        };
    }

    namespace karma
    {
        ///////////////////////////////////////////////////////////////////////
        // This one is the class that the user can instantiate directly in 
        // order to create a customized real generator
        template <typename T = double, typename Policies = real_policies<T> >
        struct real_generator
          : spirit::terminal<tag::real_tag<T, Policies> > 
        {
            real_generator() {}
            real_generator(Policies const& p)
              : spirit::terminal<tag::real_tag<T, Policies> >(p) {}
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<karma::domain, tag::float_>       // enables float_
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::double_>      // enables double_
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::long_double>  // enables long_double
      : mpl::true_ {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<karma::domain, float>             // enables lit(1.of)
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, double>            // enables lit(1.0)
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, long double>       // enables lit(1.0l)
      : mpl::true_ {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename A0>
    struct use_terminal<karma::domain                   // enables float_(...)
      , terminal_ex<tag::float_, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain                   // enables double_(...)
      , terminal_ex<tag::double_, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain                   // enables long_double(...)
      , terminal_ex<tag::long_double, fusion::vector1<A0> >
    > : mpl::true_ {};

    // lazy float_(...), double_(...), long_double(...)
    template <>
    struct use_lazy_terminal<karma::domain, tag::float_, 1> 
      : mpl::true_ {};

    template <>
    struct use_lazy_terminal<karma::domain, tag::double_, 1> 
      : mpl::true_ {};

    template <>
    struct use_lazy_terminal<karma::domain, tag::long_double, 1> 
      : mpl::true_ {};

    ///////////////////////////////////////////////////////////////////////////
    // enables custom real generator
    template <typename T, typename Policy>
    struct use_terminal<karma::domain, tag::real_tag<T, Policy> >
      : mpl::true_ {};

    template <typename T, typename Policy, typename A0>
    struct use_terminal<karma::domain
      , terminal_ex<tag::real_tag<T, Policy>, fusion::vector1<A0> >
    > : mpl::true_ {};

    // enables *lazy* custom real generator
    template <typename T, typename Policy>
    struct use_lazy_terminal<
        karma::domain
      , tag::real_tag<T, Policy>
      , 1 // arity
    > : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    using spirit::float_;
    using spirit::float__type;
    using spirit::double_;
    using spirit::double__type;
    using spirit::long_double;
    using spirit::long_double_type;

    ///////////////////////////////////////////////////////////////////////////
    //  This specialization is used for real generators not having a direct
    //  initializer: float_, double_ etc. These generators must be used in
    //  conjunction with an attribute.
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, typename Policies, typename CharEncoding, typename Tag>
    struct any_real_generator
      : primitive_generator<any_real_generator<T, Policies, CharEncoding, Tag> >
    {
        typedef typename Policies::properties properties;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef T type;
        };

        any_real_generator(Policies const& policies = Policies())
          : p_(policies) {}

        // double_/float_/etc. has an attached attribute
        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr) const
        {
            if (!traits::has_optional_value(attr))
                return false;       // fail if it's an uninitialized optional

            typedef real_inserter<T, Policies, CharEncoding, Tag> inserter_type;
            return inserter_type::call(sink, traits::optional_value(attr), p_) &&
                   karma::delimit_out(sink, d);    // always do post-delimiting
        }

        // this double_/float_/etc. has no attribute attached, it needs to have
        // been initialized from a direct literal
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool generate(OutputIterator&, Context&, Delimiter const&
          , unused_type) 
        {
            BOOST_SPIRIT_ASSERT_MSG(false, real_not_usable_without_attribute, ());
            return false;
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("real");
        }

        Policies p_;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  This specialization is used for real generators having a direct
    //  initializer: float_(10.), double_(20.) etc.
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, typename Policies, typename CharEncoding, typename Tag
      , bool no_attribute>
    struct literal_real_generator
      : primitive_generator<literal_real_generator<T, Policies, CharEncoding
          , Tag, no_attribute> >
    {
        typedef typename Policies::properties properties;

        template <typename Context, typename Unused>
        struct attribute
          : mpl::if_c<no_attribute, unused_type, T>
        {};

        literal_real_generator(typename add_const<T>::type n
              , Policies const& policies = Policies())
          : n_(n), p_(policies) {}

        // A double_(1.0) which additionally has an associated attribute emits
        // its immediate literal only if it matches the attribute, otherwise
        // it fails.
        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr) const
        {
            if (!traits::has_optional_value(attr) || 
                n_ != traits::optional_value(attr))
            {
                return false;
            }

            typedef real_inserter<T, Policies, CharEncoding, Tag> inserter_type;
            return inserter_type::call(sink, n_, p_) &&
                   karma::delimit_out(sink, d);    // always do post-delimiting
        }

        // A double_(1.0) without any associated attribute just emits its 
        // immediate literal
        template <typename OutputIterator, typename Context, typename Delimiter>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , unused_type) const
        {
            typedef real_inserter<T, Policies, CharEncoding, Tag> inserter_type;
            return inserter_type::call(sink, n_, p_) &&
                   karma::delimit_out(sink, d);    // always do post-delimiting
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("real");
        }

        T n_;
        Policies p_;
    };

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        // extract policies if this is a real_tag
        template <typename Policies>
        struct get_policies
        {
            template <typename Tag>
            static Policies call(Tag) { return Policies(); }

            template <typename T>
            static Policies const& call(tag::real_tag<T, Policies> const& p) 
            { return p.policies_; }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, typename Modifiers, typename Policies = real_policies<T> >
    struct make_real
    {
        static bool const lower = 
            has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
        static bool const upper = 
            has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

        typedef any_real_generator<
            T, Policies
          , typename spirit::detail::get_encoding<
                Modifiers, unused_type, lower || upper>::type
          , typename detail::get_casetag<Modifiers, lower || upper>::type
        > result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            using karma::detail::get_policies;
            return result_type(get_policies<Policies>::call(term));
        }
    };

    template <typename Modifiers>
    struct make_primitive<tag::float_, Modifiers> 
      : make_real<float, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<tag::double_, Modifiers> 
      : make_real<double, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<tag::long_double, Modifiers> 
      : make_real<long double, Modifiers> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Policy, typename Modifiers>
    struct make_primitive<tag::real_tag<T, Policy>, Modifiers> 
      : make_real<T, Modifiers, Policy> {};

    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, typename Modifiers, typename Policies = real_policies<T> >
    struct make_real_direct
    {
        static bool const lower = 
            has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
        static bool const upper = 
            has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

        typedef literal_real_generator<
            T, Policies
          , typename spirit::detail::get_encoding<
                Modifiers, unused_type, lower || upper>::type
          , typename detail::get_casetag<Modifiers, lower || upper>::type
          , false
        > result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            return result_type(fusion::at_c<0>(term.args)
              , karma::detail::get_policies<Policies>::call(term.term));
        }
    };

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::float_, fusion::vector1<A0> >, Modifiers>
      : make_real_direct<float, Modifiers> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::double_, fusion::vector1<A0> >, Modifiers>
      : make_real_direct<double, Modifiers> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::long_double, fusion::vector1<A0> >, Modifiers>
      : make_real_direct<long double, Modifiers> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Policy, typename A0, typename Modifiers>
    struct make_primitive<
        terminal_ex<tag::real_tag<T, Policy>, fusion::vector1<A0> >
          , Modifiers>
      : make_real_direct<T, Modifiers, Policy> {};

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T, typename Modifiers>
        struct basic_real_literal
        {
            static bool const lower =
                has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
            static bool const upper =
                has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

            typedef literal_real_generator<
                T, real_policies<T>
              , typename spirit::detail::get_encoding<
                    Modifiers, unused_type, lower || upper>::type
              , typename detail::get_casetag<Modifiers, lower || upper>::type
              , true
            > result_type;

            template <typename T_>
            result_type operator()(T_ i, unused_type) const
            {
                return result_type(i);
            }
        };
    }

    template <typename Modifiers>
    struct make_primitive<float, Modifiers> 
      : detail::basic_real_literal<float, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<double, Modifiers> 
      : detail::basic_real_literal<double, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<long double, Modifiers> 
      : detail::basic_real_literal<long double, Modifiers> {};

}}}

#endif // defined(BOOST_SPIRIT_KARMA_REAL_FEB_26_2007_0512PM)
