/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_GROUPING_HPP
#define BOOST_SPIRIT_GROUPING_HPP

#include <utility>

#include "boost/spirit/phoenix/tuples.hpp"
#include "boost/spirit/phoenix/tuple_helpers.hpp"
#include "boost/spirit/phoenix/functions.hpp"

#include "boost/spirit/core/parser.hpp"
#include "boost/spirit/attribute/impl/grouping.ipp"

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
//
//  grouping_result class
//
//      This type computer returnes the tuple type of a grouping parser.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ParserT, typename ScannerT, typename CategoryT>
struct grouping_result {

    typedef typename impl::tuple_result<CategoryT>
        ::template result<ParserT, ScannerT>::type type;
};

///////////////////////////////////////////////////////////////////////////////
//
//  grouping_parser class
//
//      The grouping parser template handles the composition of single parser
//      results of a composite parser into one overall parser result, which
//      can be used for accessing all the parser results from inside a single
//      semantic action code attached to the grouping parser. For instance
//
//          std::pair<double, double> c;
//          group_d[real_p >> ',' >> real_p]
//          [
//              c = construct<std::pair<double, double> >(arg1, arg3)
//          ]
//
//      will parse two comma separated real numbers and assigns them after
//      a successful match to the pair variable 'c'. As you can see, the
//      result of the first real_p parser is accessible through the 'arg1'
//      placeholder variable and the third parser result (the second real_p)
//      is accessible through the 'arg3' placeholder variable.
//
//      If a single parser does not return a result value (the corresponding
//      match<> result contains nil_t as its value), a std::pair of iterators
//      pointing to the begin and the end of the corresponding matched input
//      sequence will be stored as the parser result. For instance
//
//          std::string result_string;
//          rule<> esc_string = *(c_escape_ch_p - '"');
//          group_d[ch_p('"') >> esc_string >> '"']
//          [
//              result_string =
//                  construct<std::string>(first(arg2), second(arg2));
//          ]
//
//      will fill the return_string variable with the C style escaped character
//      string, which was embedded into the '"'.
//
///////////////////////////////////////////////////////////////////////////////

struct grouping_parser_gen;

template <typename ParserT>
struct grouping_parser
:   public unary<ParserT, parser<grouping_parser<ParserT> > >
{
    typedef grouping_parser<ParserT>                    self_t;
    typedef grouping_parser_gen                         parser_generator_t;
    typedef unary_parser_category                       parser_category_t;

    typedef unary<ParserT, parser<self_t> >             base_t;
    typedef typename ParserT::parser_category_t         base_category_t;

    template <typename ScannerT>
    struct result
    {
        typedef
            typename grouping_result<ParserT, ScannerT, base_category_t>::type
            tuple_t;

        typedef phoenix::unpack_tuple<tuple_t> attr_t;
        typedef typename match_result<ScannerT, attr_t>::type type;
    };

    grouping_parser()
    : base_t(ParserT()) {}

    grouping_parser(ParserT const &p)
    : base_t(p) {}

    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        impl::grouped_parser<ParserT, ScannerT, self_t, base_category_t> p;
        return p.parse(this->subject(), scan);
    }
};

struct grouping_parser_gen {

    template <typename ParserT>
    struct result {

        typedef grouping_parser<ParserT> type;
    };

    template <typename ParserT>
    static grouping_parser<ParserT>
    generate (parser<ParserT> const &p)
    {
        return grouping_parser<ParserT>(p.derived());
    }

    template <typename ParserT>
    grouping_parser<ParserT>
    operator[] (parser<ParserT> const &p) const
    {
        return grouping_parser<ParserT>(p.derived());
    }
};

const grouping_parser_gen group_d = grouping_parser_gen();

///////////////////////////////////////////////////////////////////////////////
//
//  first() and second() lazy access functions for accessing the .first and
//  .second members of a std::pair as embedded into the grouping result tuple
//  for parsers, not returning an explicit result.
//
///////////////////////////////////////////////////////////////////////////////

struct first_impl {

    template <typename IteratorT>
    struct result { typedef IteratorT type; };

    template <typename IteratorT>
    IteratorT operator()(std::pair<IteratorT, IteratorT> const &p) const
    { return p.first; }
};

struct second_impl {

    template <typename IteratorT>
    struct result { typedef IteratorT type; };

    template <typename IteratorT>
    IteratorT operator()(std::pair<IteratorT, IteratorT> const &p) const
    { return p.second; }
};

phoenix::function<first_impl> const first;
phoenix::function<second_impl> const second;

///////////////////////////////////////////////////////////////////////////////
}} // namespace boost::spirit

#endif // BOOST_SPIRIT_GROUPING_HPP
