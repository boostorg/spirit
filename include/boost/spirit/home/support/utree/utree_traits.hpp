//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM)
#define BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM

#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/utree.hpp>
#include <boost/spirit/home/karma/domain.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/variant.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost
{
    template <typename T>
    inline T get(boost::spirit::utree const& x)
    {
        return x.get<T>();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit.Qi to allow assignment to an utree from 
    // a variant
    namespace detail
    {
        struct assign_to_utree_visitor : static_visitor<>
        {
            assign_to_utree_visitor(utree& ut) : ut_(ut) {}

            template <typename T>
            void operator()(T& val) const
            {
                ut_ = val;
            }

            utree& ut_;
        };
    }

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct assign_to_attribute_from_value<utree, variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
    {
        static void 
        call(variant<BOOST_VARIANT_ENUM_PARAMS(T)> const& val, utree& attr)
        {
            apply_visitor(detail::assign_to_utree_visitor(attr), val);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit.Qi to allow assignment to an utree from 
    // a STL container
    template <typename Container>
    struct assign_to_attribute_from_value<utree, Container
      , typename enable_if<is_container<Container> >::type>
    {
        static void 
        call(Container const& val, utree& attr)
        {
            attr = make_iterator_range(traits::begin(val), traits::end(val));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization is required to disambiguate the specialization 
    template <>
    struct assign_to_attribute_from_value<utree, utree>
    {
        static void 
        call(utree const& val, utree& attr)
        {
            attr = val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // debug support for utree
    template <typename Out>
    struct print_attribute_debug<Out, utree>
    {
        static void call(Out& out, utree const& val)
        {
            out << val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // the specialization below tells Spirit to handle utree as if it
    // where a 'real' variant (in the context of karma)
    template <>
    struct not_is_variant<utree, karma::domain>
      : mpl::false_ {};

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit how to extract the type of the value 
    // stored in the given utree node
    template <>
    struct variant_which<utree>
    {
        static int call(utree const& u) { return u.which(); }
    };

    ///////////////////////////////////////////////////////////////////////////
    // The specializations below tell Spirit to verify whether an attribute
    // type is compatible with a given variant type
    template <>
    struct compute_compatible_component_variant<
            utree, iterator_range<utree::iterator> >
      : mpl::true_
    {
        typedef iterator_range<utree::iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::list_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, iterator_range<utree::const_iterator> >
      : mpl::true_
    {
        typedef iterator_range<utree::const_iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::list_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, nil>
      : mpl::true_
    {
        typedef nil compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::nil_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, bool>
      : mpl::true_
    {
        typedef bool compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::bool_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, int>
      : mpl::true_
    {
        typedef int compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::int_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, double>
      : mpl::true_
    {
        typedef double compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::double_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_string_range>
      : mpl::true_
    {
        typedef utf8_string_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::string_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_string_type>
      : mpl::true_
    {
        typedef utf8_string_type compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::string_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_symbol_range>
      : mpl::true_
    {
        typedef utf8_symbol_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::symbol_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_symbol>
      : mpl::true_
    {
        typedef utf8_symbol compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::symbol_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, binary_range>
      : mpl::true_
    {
        typedef binary_range compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::binary_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, binary_string>
      : mpl::true_
    {
        typedef binary_string compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::binary_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, utree>
      : mpl::true_
    {
        typedef utree compatible_type;

        static bool is_compatible(int d) 
        { 
            return d >= utree_type::nil_type &&
                   d <= utree_type::reference_type; 
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, std::vector<utree> >
      : mpl::true_
    {
        typedef utree compatible_type;

        static bool is_compatible(int d) 
        { 
            return d >= utree_type::nil_type &&
                   d <= utree_type::reference_type; 
        }
    };

    template <typename Sequence>
    struct compute_compatible_component_variant<utree, Sequence
          , mpl::false_
          , typename enable_if<fusion::traits::is_sequence<Sequence> >::type>
      : mpl::true_
    {
        typedef iterator_range<utree::const_iterator> compatible_type;

        static bool is_compatible(int d) 
        { 
            return d == utree_type::list_type; 
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct symbols_lookup<utree, utf8_symbol>
    {
        typedef std::string type;

        static type call(utree const& t)
        {
            utf8_symbol_range r = boost::get<utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct symbols_lookup<utf8_symbol, utf8_symbol>
    {
        typedef std::string type;

        static type call(utf8_symbol const& t)
        {
            return t;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct extract_from_attribute<utree, utf8_symbol>
    {
        typedef std::string type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_symbol_range r = boost::get<utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct extract_from_attribute<utree, utf8_string>
    {
        typedef std::string type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_string_range r = boost::get<utf8_string_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<utree const, utf8_string, karma::domain>
    {
        typedef std::string type;

        static type pre(utree const& t) 
        { 
            utf8_string_range r = boost::get<utf8_string_range>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct transform_attribute<utree const, utf8_symbol, karma::domain>
    {
        typedef std::string type;

        static type pre(utree const& t) 
        { 
            utf8_symbol_range r = boost::get<utf8_symbol_range>(t);
            return std::string(r.begin(), r.end());
        }
    };
}}}

#endif
