//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_META_CREATE_NOV_21_2009_0425PM)
#define BOOST_SPIRIT_KARMA_META_CREATE_NOV_21_2009_0425PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>

#include <boost/spirit/home/support/auto/meta_create.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    ///////////////////////////////////////////////////////////////////////////
    // compatible STL containers
    template <typename Container> 
    struct meta_create_container 
    {
        struct make_dereference
        {
            template <typename RT, typename T_>
            static RT call(T_ const& t)
            {
                // we map STL containers to the Kleene star
                return *t;
            }
        };

        typedef spirit::detail::make_unary_proto_expr<
            typename Container::value_type, proto::tag::dereference
          , make_dereference, karma::domain
        > make_proto_expr;

        typedef typename make_proto_expr::type type;

        static type call()
        {
            return make_proto_expr::call();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // String types
    template <typename String> 
    struct meta_create_string 
    {
        typedef spirit::standard::string_type const& type; 
        static type call() { return spirit::standard::string; }
    };

    template <> 
    struct meta_create_string<wchar_t*>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <> 
    struct meta_create_string<wchar_t const*>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <int N> 
    struct meta_create_string<wchar_t[N]>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <int N> 
    struct meta_create_string<wchar_t const[N]>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <int N> 
    struct meta_create_string<wchar_t(&)[N]>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <int N> 
    struct meta_create_string<wchar_t const(&)[N]>
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    template <typename Traits, typename Allocator> 
    struct meta_create_string<std::basic_string<wchar_t, Traits, Allocator> >
    {
        typedef spirit::standard_wide::string_type const& type; 
        static type call() { return spirit::standard_wide::string; }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Fusion sequences
    template <typename Sequence> 
    struct meta_create_sequence 
    {
        struct make_shift_left
        {
            template <typename RT, typename T1, typename T2>
            static RT call(T1 const& t1, T2 const& t2) 
            {
                return t1 << t2;
            }
        };

        // create a mpl sequence from the given fusion sequence
        typedef typename mpl::fold<
            typename fusion::result_of::as_vector<Sequence>::type
          , mpl::vector<>, mpl::push_back<mpl::_, mpl::_> 
        >::type sequence_type;

        typedef spirit::detail::make_nary_proto_expr<
            sequence_type, proto::tag::shift_left, make_shift_left, karma::domain
        > make_proto_expr;

        typedef typename make_proto_expr::type type;

        static type call()
        {
            return make_proto_expr::call();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // the default is to use the standard streaming operator unless it's a 
    // STL container or a fusion sequence
    template <typename T, typename Enable = void>
    struct meta_create_impl;

    template <typename T>
    struct meta_create_impl<T
          , typename enable_if_c<
                traits::is_container<T>::value && !traits::is_string<T>::value
            >::type>
      : meta_create_container<T> {};

    template <typename T>
    struct meta_create_impl<T
          , typename enable_if<
                traits::is_string<T> 
            >::type>
      : meta_create_string<T> {};

    template <typename T>
    struct meta_create_impl<T
          , typename enable_if<
                fusion::traits::is_sequence<T> 
            >::type>
      : meta_create_sequence<T> {};

    template <typename T, typename Enable = void>
    struct meta_create : meta_create_impl<T> {};

    ///////////////////////////////////////////////////////////////////////////
    // optional
    template <typename T> 
    struct meta_create<boost::optional<T> > 
    {
        struct make_negate
        {
            template <typename RT, typename T_>
            static RT call(T_ const& t)
            {
                return -t;
            }
        };

        typedef spirit::detail::make_unary_proto_expr<
            T, proto::tag::negate, make_negate, karma::domain
        > make_proto_expr;

        typedef typename make_proto_expr::type type;

        static type call()
        {
            return make_proto_expr::call();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // alternatives
    template <BOOST_VARIANT_ENUM_PARAMS(typename T)> 
    struct meta_create<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > 
    {
        struct make_bitwise_or
        {
            template <typename RT, typename T1_, typename T2_>
            static RT call(T1_ const& t1, T2_ const& t2)
            {
                return t1 | t2;
            }
        };

        typedef spirit::detail::make_nary_proto_expr<
            typename boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types
          , proto::tag::bitwise_or, make_bitwise_or, karma::domain
        > make_proto_expr;

        typedef typename make_proto_expr::type type;

        static type call()
        {
            return make_proto_expr::call();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // predefined specializations for primitive components

    // character generator
    template <> 
    struct meta_create<char> 
    { 
        typedef spirit::standard::char_type const& type; 
        static type call() { return spirit::standard::char_; }
    };
    template <> 
    struct meta_create<wchar_t> 
    { 
        typedef spirit::standard_wide::char_type const& type; 
        static type call() { return spirit::standard_wide::char_; }
    };

    // boolean generator
    template <> 
    struct meta_create<bool> 
    { 
        typedef spirit::bool__type const& type; 
        static type call() { return spirit::bool_; }
    };

    // integral generators
    template <> 
    struct meta_create<int> 
    { 
        typedef spirit::int__type const& type; 
        static type call() { return spirit::int_; }
    };
    template <> 
    struct meta_create<short> 
    { 
        typedef spirit::short__type const& type; 
        static type call() { return spirit::short_; }
    };
    template <> 
    struct meta_create<long> 
    {
        typedef spirit::long__type const& type; 
        static type call() { return spirit::long_; }
    };
    template <> 
    struct meta_create<unsigned int> 
    { 
        typedef spirit::uint__type const& type; 
        static type call() { return spirit::uint_; }
    };
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    template <> 
    struct meta_create<unsigned short> 
    { 
        typedef spirit::ushort__type const& type; 
        static type call() { return spirit::ushort_; }
    };
#endif
    template <> 
    struct meta_create<unsigned long> 
    { 
        typedef spirit::ulong__type const& type; 
        static type call() { return spirit::ulong_; }
    };

#ifdef BOOST_HAS_LONG_LONG
    template <> 
    struct meta_create<boost::long_long_type> 
    { 
        typedef spirit::long_long_type const& type; 
        static type call() { return spirit::long_long; }
    };
    template <> 
    struct meta_create<boost::ulong_long_type> 
    { 
        typedef spirit::ulong_long_type const& type; 
        static type call() { return spirit::ulong_long; }
    };
#endif

    // floating point generators
    template <> 
    struct meta_create<float> 
    { 
        typedef spirit::float__type const& type; 
        static type call() { return spirit::float_; }
    };
    template <> 
    struct meta_create<double> 
    { 
        typedef spirit::double__type const& type; 
        static type call() { return spirit::double_; }
    };
    template <> 
    struct meta_create<long double> 
    { 
        typedef spirit::long_double_type const& type; 
        static type call() { return spirit::long_double; }
    };

}}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit 
{
    // dispatch this to the karma related specializations
    template <typename T>
    struct meta_create<karma::domain, T>
    {
        typedef typename spirit::detail::remove_const_ref<T>::type data_type;
        typedef typename karma::meta_create<data_type>::type type;

        static type call()
        {
            return karma::meta_create<data_type>::call();
        }
    };

}}

#endif
