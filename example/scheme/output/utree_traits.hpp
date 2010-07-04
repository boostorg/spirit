//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SCHEME_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM)
#define SCHEME_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM

#include <utree/utree.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/spirit/include/karma.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost
{
    template <typename T>
    inline T get(scheme::utree const& x)
    {
        return x.get<T>();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // the specialization below tells Spirit to handle scheme::utree as if it
    // where a 'real' variant (in the context of karma)
    template <>
    struct not_is_variant<scheme::utree, karma::domain>
      : mpl::false_ {};

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit how to extract the type of the value 
    // stored in the given utree node
    template <>
    struct variant_which<scheme::utree>
    {
        static int call(scheme::utree const& u) { return u.which(); }
    };

    ///////////////////////////////////////////////////////////////////////////
    // The specializations below tell Spirit to verify whether an attribute
    // type is compatible with a given variant type
    template <>
    struct compute_compatible_component_variant<
            scheme::utree, iterator_range<scheme::utree::iterator> >
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::list_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, iterator_range<scheme::utree::const_iterator> >
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::const_iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::list_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, scheme::nil>
      : mpl::true_
    {
        typedef scheme::nil compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::nil_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, bool>
      : mpl::true_
    {
        typedef bool compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::bool_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, int>
      : mpl::true_
    {
        typedef int compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::int_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, double>
      : mpl::true_
    {
        typedef double compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::double_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::utf8_string_range>
      : mpl::true_
    {
        typedef scheme::utf8_string_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::string_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::utf8_string>
      : mpl::true_
    {
        typedef scheme::utf8_string compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::string_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::utf8_symbol_range>
      : mpl::true_
    {
        typedef scheme::utf8_symbol_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::symbol_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::utf8_symbol>
      : mpl::true_
    {
        typedef scheme::utf8_symbol compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::symbol_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::binary_range>
      : mpl::true_
    {
        typedef scheme::binary_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::binary_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, scheme::binary_string>
      : mpl::true_
    {
        typedef scheme::binary_string compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::binary_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utree compatible_type;

        static bool is_compatible(int d) 
        { 
            return d >= scheme::utree_type::nil_type &&
                   d <= scheme::utree_type::reference_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utree, std::vector<scheme::utree> >
      : mpl::true_
    {
        typedef scheme::utree compatible_type;

        static bool is_compatible(int d) 
        { 
            return d >= scheme::utree_type::nil_type &&
                   d <= scheme::utree_type::reference_type; 
        }
    };

    template <typename Sequence>
    struct compute_compatible_component_variant<scheme::utree, Sequence
          , mpl::false_
          , typename enable_if<fusion::traits::is_sequence<Sequence> >::type>
      : mpl::true_
    {
        typedef iterator_range<scheme::utree::const_iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == scheme::utree_type::list_type; 
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct symbols_lookup<scheme::utree, scheme::utf8_symbol>
    {
        typedef std::string type;

        static type call(scheme::utree const& t)
        {
            scheme::utf8_symbol_range r = boost::get<scheme::utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct symbols_lookup<scheme::utf8_symbol, scheme::utf8_symbol>
    {
        typedef std::string type;

        static type call(scheme::utf8_symbol const& t)
        {
            return t;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct extract_from_attribute<scheme::utree, scheme::utf8_symbol>
    {
        typedef std::string type;

        template <typename Context>
        static type call(scheme::utree const& t, Context&)
        {
            scheme::utf8_symbol_range r = boost::get<scheme::utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct extract_from_attribute<scheme::utree, scheme::utf8_string>
    {
        typedef std::string type;

        template <typename Context>
        static type call(scheme::utree const& t, Context&)
        {
            scheme::utf8_string_range r = boost::get<scheme::utf8_string_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<scheme::utree const, scheme::utf8_string, karma::domain>
    {
        typedef std::string type;

        static type pre(scheme::utree const& t) 
        { 
            scheme::utf8_string_range r = boost::get<scheme::utf8_string_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct transform_attribute<scheme::utree const, scheme::utf8_symbol, karma::domain>
    {
        typedef std::string type;

        static type pre(scheme::utree const& t) 
        { 
            scheme::utf8_symbol_range r = boost::get<scheme::utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };
}}}

#endif
