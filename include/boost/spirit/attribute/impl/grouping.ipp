/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_GROUPING_IPP
#define BOOST_SPIRIT_GROUPING_IPP

#include <boost/type_traits.hpp>
#include "boost/spirit/core/meta/traverse.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
namespace impl
{

///////////////////////////////////////////////////////////////////////////////
//
//  tuple_result type computer
//
//      This template class returns the type of a tuple containing all the
//      return values of all parsers, which compose a given parser.
//
//      For all parser types except binary_category_parser type parsers the
//      tuple_result template returnes the return type of a given parser
//      wrapped into a tuple.
//      For binary_parser_category type parsers the tuple_result template
//      returnes a tuple, which containes the tuple members of the left
//      subparser concatinated with the tuple members of right subparser.
//
///////////////////////////////////////////////////////////////////////////////

template <typename CategoryT>
struct tuple_result;

template <>
struct tuple_result<plain_parser_category> {

    template <typename ParserT, typename ScannerT>
    struct result {

    //  The type 'match_t' is the type of the match<> class returned by the
    //  parser under inspection.

        typedef typename parser_result<ParserT, ScannerT>::type match_t;

    //  The type 'type' is the parser return type of the parser under
    //  inspection, converted to a corresponding tuple type.
    //  If the parser under inspection does not return any value, a std::pair
    //  containing the corresponding 'begin' and 'end' iterators pointing to
    //  the begin and the end of the matched input sequence is considered to
    //  be the parser result.

//#if defined(__BORLANDC__) && __BORLANDC__ >= 0x561
//        enum {
//            value = ::boost::is_same<nil_t, typename match_t::attr_t>::value
//        };
//#else
//        BOOST_STATIC_CONSTANT(int,
//            value = (::boost::is_same<nil_t, typename match_t::attr_t>::value));
//#endif

        typedef result self_t;
        typedef typename ScannerT::iterator_t iterator_t;
        typedef typename
                mpl::if_<is_same<nil_t, typename match_t::attr_t>,
                    std::pair<iterator_t, iterator_t>,
                    typename match_t::attr_t
                >::type
            result_t;
        typedef typename phoenix::make_tuple<result_t>::type type;
    };
};

template <>
struct tuple_result<unary_parser_category> {

    template <typename ParserT, typename ScannerT>
    struct result {

        typedef typename ParserT::subject_t             subject_t;
        typedef typename subject_t::parser_category_t   subject_category_t;

        typedef typename tuple_result<subject_category_t>
            ::template result<subject_t, ScannerT>::type subject_result_t;

    //  The type 'type' is the parser return type of the subject of the
    //  parser under inspection

        typedef subject_result_t type;
    };
};

template <>
struct tuple_result<action_parser_category> {

    template <typename ParserT, typename ScannerT>
    struct result {

        typedef typename ParserT::subject_t             subject_t;
        typedef typename subject_t::parser_category_t   subject_category_t;

        typedef typename tuple_result<subject_category_t>
            ::template result<subject_t, ScannerT>::type subject_result_t;

    //  The type 'type' is the parser return type of the subject of the
    //  parser under inspection

        typedef subject_result_t type;
    };
};

template <>
struct tuple_result<binary_parser_category> {

    template <typename ParserT, typename ScannerT>
    struct result {

        typedef typename ParserT::left_t                left_t;
        typedef typename ParserT::right_t               right_t;
        typedef typename left_t::parser_category_t      left_category_t;
        typedef typename right_t::parser_category_t     right_category_t;

        typedef typename tuple_result<left_category_t>
            ::template result<left_t, ScannerT>::type   left_result_t;
        typedef typename tuple_result<right_category_t>
            ::template result<right_t, ScannerT>::type  right_result_t;

    // The type 'type' is the concatenation of the two tuple types, returned
    // by the left and right subparsers of the binary parser under inspection.

        typedef
            typename phoenix::concat_tuples<left_result_t, right_result_t>::type
            type;
    };
};

///////////////////////////////////////////////////////////////////////////////
//
//  grouped_parser parse helper template
//
//      The grouping should be done for binary_category type parsers only.
//      All other parsers are left without any change.
//
///////////////////////////////////////////////////////////////////////////////

template <
    typename ParserT, typename ScannerT, typename GroupingT,
    typename CategoryT
>
struct grouped_parser
:   public parser<grouped_parser<ParserT, ScannerT, GroupingT, CategoryT> > {

    typedef CategoryT parser_category_t;

    static typename parser_result<GroupingT, ScannerT>::type
    parse (ParserT const &p, ScannerT const &scan)
    {
        return p.parse(scan);
    }
};

///////////////////////////////////////////////////////////////////////////////
template <typename TupleT, int N>
class group_actor {

public:
    group_actor (TupleT &tuple_) : t(tuple_) {}

    template <typename T>
    void operator() (T const &val) const
    {
        t[phoenix::tuple_index<N>()] = val;
    }

    template <typename IteratorT>
    void operator()(IteratorT const &begin, IteratorT const &end) const
    {
        t[phoenix::tuple_index<N>()] = std::pair<IteratorT, IteratorT>(begin, end);
    }

private:
    mutable TupleT &t;
};

///////////////////////////////////////////////////////////////////////////////
//
//  group_transform template
//
//      The group_transform template is a metafunction, which is used during
//      parser traversal of the composite parser to be grouped. It attaches an
//      action parser to all leaf parsers (all plain_parser_category parsers),
//      which in turn will assign the result of the leaf parser to the
//      corresponding tuple member of the grouped parser result tuple.
//
///////////////////////////////////////////////////////////////////////////////

template <typename TransformT, typename TupleT>
class group_transform_plain_policy {

public:

    group_transform_plain_policy(TupleT &tuple_) : t(tuple_) {}

    template <typename ParserT, typename EnvT>
    struct plain_result {

        typedef group_actor<TupleT, (EnvT::index)> actor_t;
        typedef action<ParserT, actor_t> type;
    };

    template <typename ParserT, typename EnvT>
    typename parser_traversal_plain_result<TransformT, ParserT, EnvT>::type
    generate_plain(ParserT const &parser_, EnvT const &env) const
    {
        typedef group_actor<TupleT, (EnvT::index)> actor_t;
        return parser_[actor_t(t)];
    }

private:
    TupleT &t;
};

//////////////////////////////////
template <typename TransformT, typename TupleT>
struct group_transform_policies
:   public group_transform_plain_policy<TransformT, TupleT>,
    public unary_identity_policy<TransformT>,
    public action_identity_policy<TransformT>,
    public binary_identity_policy<TransformT>
{
    group_transform_policies(TupleT &tuple_)
    :   group_transform_plain_policy<TransformT, TupleT>(tuple_) {}
};

//////////////////////////////////
template <typename TupleT>
struct group_transform
:   public group_transform_policies<group_transform<TupleT>, TupleT> {

    group_transform(TupleT &tuple_)
    :   group_transform_policies<group_transform<TupleT>, TupleT>(tuple_) {}
};

///////////////////////////////////////////////////////////////////////////////
//
//  This template is the real workhorse behind the group_d generated parsers,
//  if the parser to group is a binary parser.
//
//  The main work this template does, is to attach a special actor as a
//  semantic action recursivly to all leaf parsers of the original binary
//  parser. This is done by the help of the post_order parser traversal
//  algorithm. So during the parsing process every leaf parser calls the
//  corresponding operator() functions of its respective actor, providing it
//  with its parser result.
//
//  The attached actors are constructed such, that they assign the parser
//  result values to the corresponding member of the overall result tuple.
//
//  The correct tuple member number is computed during the post_order parser
//  traversal process such, that the leaf parsers are numbered from left to
//  right. Such the most left leaf parser result is accessible as 'arg1' inside
//  the sematic action code attached to the grouped parser, the second leaf
//  parser result is accessible 'arg2' and so on.
//
//  After a successful match this tuple value is feeded into the usual
//  Spirit semantic action mechanics and the tuple members are available from
//  inside the semantic action code attached to the grouped parser.
//
///////////////////////////////////////////////////////////////////////////////

template <typename ParserT, typename ScannerT, typename GroupingT>
struct grouped_parser<ParserT, ScannerT, GroupingT, binary_parser_category>
:   public parser<
        grouped_parser<ParserT, ScannerT, GroupingT, binary_parser_category> >
{
    typedef typename parser_result<GroupingT, ScannerT>::type result_t;
    typedef typename result_t::attr_t::tuple_t tuple_t;
    typedef binary_parser_category parser_category_t;
    typedef
        grouped_parser<ParserT, ScannerT, GroupingT, binary_parser_category>
        self_t;

    typedef typename parser_result<ParserT, ScannerT>::type embedded_result_t;

    // the resulting tuple should contain at least 2 elements
    BOOST_STATIC_ASSERT(tuple_t::length >= 2);

    result_t
    parse (ParserT const &p, ScannerT const &scan) const
    {
        typename ScannerT::iterator_t save = scan.first;
        embedded_result_t hit =
            post_order::traverse(group_transform<tuple_t>(t), p).parse(scan);

        if (hit) {
            return scan.create_match(hit.length(),
                phoenix::unpack_tuple<tuple_t>(t), save, scan.first);
        }
        return scan.no_match();
    }

private:
    mutable tuple_t t;
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
}} // namespace boost::spirit

#endif // BOOST_SPIRIT_GROUPING_IPP
