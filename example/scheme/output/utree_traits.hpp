//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SCHEME_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM)
#define SCHEME_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM

#include <utree/utree.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/spirit/include/karma.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // the specialization below tells Spirit to handle scheme::utree as if it
    // where a 'real' variant (in the context of karma)
    template <>
    struct not_is_variant<scheme::utree, karma::domain>
      : mpl::false_ {};

    // The specializations below tell Spirit to verify whether an attribute
    // type is compatible with a given variant type
    template <>
    struct compute_compatible_component_variant<
            iterator_range<scheme::utree::iterator>, scheme::utree>
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::list_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            iterator_range<scheme::utree::const_iterator>, scheme::utree>
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::const_iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::list_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<scheme::nil, scheme::utree>
      : mpl::true_
    {
        typedef scheme::nil compatible_type;
        typedef mpl::int_<scheme::utree_type::nil_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<bool, scheme::utree>
      : mpl::true_
    {
        typedef bool compatible_type;
        typedef mpl::int_<scheme::utree_type::bool_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<int, scheme::utree>
      : mpl::true_
    {
        typedef int compatible_type;
        typedef mpl::int_<scheme::utree_type::int_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<double, scheme::utree>
      : mpl::true_
    {
        typedef double compatible_type;
        typedef mpl::int_<scheme::utree_type::double_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_string_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_string_range compatible_type;
        typedef mpl::int_<scheme::utree_type::string_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_string, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_string compatible_type;
        typedef mpl::int_<scheme::utree_type::string_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_symbol_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_symbol_range compatible_type;
        typedef mpl::int_<scheme::utree_type::symbol_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_symbol, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_symbol compatible_type;
        typedef mpl::int_<scheme::utree_type::symbol_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::binary_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::binary_range compatible_type;
        typedef mpl::int_<scheme::utree_type::binary_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::binary_string, scheme::utree>
      : mpl::true_
    {
        typedef scheme::binary_string compatible_type;
        typedef mpl::int_<scheme::utree_type::binary_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, scheme::utree>
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::const_iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::reference_type> distance;

        static bool is_compatible(int d) 
        { 
            return d >= scheme::utree_type::nil_type &&
                   d <= scheme::utree_type::reference_type; 
        }
    };

    template <typename Sequence>
    struct compute_compatible_component_variant<Sequence, scheme::utree
          , mpl::false_
          , typename enable_if<fusion::traits::is_sequence<Sequence> >::type>
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::const_iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::list_type> distance;

        static bool is_compatible(int d) { return d == distance::value; }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct symbols_lookup<scheme::utf8_symbol, scheme::utree>
    {
        typedef std::string type;

        static type call(scheme::utree const& t)
        {
            return boost::get<scheme::utf8_symbol>(t);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct extract_from_attribute<scheme::utree, scheme::utf8_symbol>
    {
        typedef std::string type;

        template <typename Context>
        static type call(scheme::utree const& attr, Context&)
        {
            return boost::get<scheme::utf8_symbol>(attr);
        }
    };

    template <>
    struct extract_from_attribute<scheme::utree, scheme::utf8_string>
    {
        typedef std::string type;

        template <typename Context>
        static type call(scheme::utree const& attr, Context&)
        {
            return boost::get<scheme::utf8_string>(attr);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<scheme::utree const, scheme::utf8_string, karma::domain>
    {
        typedef std::string type;

        static type pre(scheme::utree const& val) 
        { 
            return boost::get<scheme::utf8_string>(val); 
        }
    };

    template <>
    struct transform_attribute<scheme::utree const, scheme::utf8_symbol, karma::domain>
    {
        typedef std::string type;

        static type pre(scheme::utree const& val) 
        { 
            return boost::get<scheme::utf8_symbol>(val); 
        }
    };
}}}

#endif
