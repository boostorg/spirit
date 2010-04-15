//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SCHEME_OUTPUT_SEXPR_MAR_8_2010_829AM)
#define SCHEME_OUTPUT_SEXPR_MAR_8_2010_829AM

#include <utree/utree.hpp>
#include <utree/detail/utree_detail3.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/spirit/include/karma.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    // the specialization below tells Spirit to handle scheme::utree as if it
    // where a 'real' variant (in the context of karma)
    template <>
    struct not_is_variant<scheme::utree, karma::domain>
      : mpl::false_ {};

    // The specializations below tell Spirit to verify whether an attribute
    // type is compatible with a given variant type
    template <>
    struct compute_compatible_component_variant<
            boost::iterator_range<scheme::utree::iterator>, scheme::utree>
      : mpl::true_
    {
        typedef boost::iterator_range<scheme::utree::iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::list_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            boost::iterator_range<scheme::utree::const_iterator>, scheme::utree>
      : mpl::true_
    {
        typedef boost::iterator_range<scheme::utree::const_iterator> compatible_type;
        typedef mpl::int_<scheme::utree_type::list_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<scheme::nil, scheme::utree>
      : mpl::true_
    {
        typedef scheme::nil compatible_type;
        typedef mpl::int_<scheme::utree_type::nil_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<bool, scheme::utree>
      : mpl::true_
    {
        typedef bool compatible_type;
        typedef mpl::int_<scheme::utree_type::bool_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<int, scheme::utree>
      : mpl::true_
    {
        typedef int compatible_type;
        typedef mpl::int_<scheme::utree_type::int_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<double, scheme::utree>
      : mpl::true_
    {
        typedef double compatible_type;
        typedef mpl::int_<scheme::utree_type::double_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_string_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_string_range compatible_type;
        typedef mpl::int_<scheme::utree_type::string_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_string, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_string compatible_type;
        typedef mpl::int_<scheme::utree_type::string_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_symbol_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_symbol_range compatible_type;
        typedef mpl::int_<scheme::utree_type::symbol_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::utf8_symbol, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utf8_symbol compatible_type;
        typedef mpl::int_<scheme::utree_type::symbol_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::binary_range, scheme::utree>
      : mpl::true_
    {
        typedef scheme::binary_range compatible_type;
        typedef mpl::int_<scheme::utree_type::binary_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<
            scheme::binary_string, scheme::utree>
      : mpl::true_
    {
        typedef scheme::binary_string compatible_type;
        typedef mpl::int_<scheme::utree_type::binary_type> distance;
    };

    template <>
    struct compute_compatible_component_variant<scheme::utree, scheme::utree>
      : mpl::true_
    {
        typedef scheme::utree compatible_type;
        typedef mpl::int_<scheme::utree_type::reference_type> distance;
    };
}}}

///////////////////////////////////////////////////////////////////////////////
namespace scheme { namespace output
{
    using boost::spirit::karma::grammar;
    using boost::spirit::karma::space_type;
    using boost::spirit::karma::rule;
    using boost::spirit::karma::double_;
    using boost::spirit::karma::int_;
    using boost::spirit::karma::char_;
    using boost::spirit::karma::string;
    using boost::spirit::karma::bool_;
    using boost::spirit::karma::eps;
    using boost::spirit::karma::space_type;
    using boost::spirit::karma::uint_generator;
    using boost::spirit::karma::verbatim;
    using boost::spirit::karma::delimit;
    using boost::spirit::karma::hex;
    using boost::spirit::karma::right_align;

    template <typename OutputIterator>
    struct sexpr : grammar<OutputIterator, space_type, utree()>
    {
        sexpr() : sexpr::base_type(start)
        {
            uint_generator<unsigned char, 16> hex2;

            start     = double_
                      | int_
                      | bool_
                      | string_
                      | symbol
                      | byte_str
                      | list
                      | nil_
                      | ref_
                      ;

            list      = '(' << *start << ')';

            string_   = '"' << string << '"';
            symbol    = string;
            byte_str  = 'b' << *right_align(2, '0')[hex2];
            nil_      = eps << "<nil>";
            ref_      = start;
        }

        typedef boost::iterator_range<utree::const_iterator> utree_list;

        rule<OutputIterator, space_type, utree()> start;
        rule<OutputIterator, space_type, utree_list()> list;
        rule<OutputIterator, utf8_symbol_range()> symbol;
        rule<OutputIterator, utf8_string_range()> string_;
        rule<OutputIterator, binary_range()> byte_str;
        rule<OutputIterator, nil()> nil_;
        rule<OutputIterator, space_type, utree()> ref_;
    };
}}

#endif
