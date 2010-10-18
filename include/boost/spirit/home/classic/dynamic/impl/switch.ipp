/*=============================================================================
    Copyright (c) 2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_SWITCH_IPP
#define BOOST_SPIRIT_SWITCH_IPP

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/repeat_from_to.hpp>

#include <boost/spirit/home/classic/core/parser.hpp>
#include <boost/spirit/home/classic/core/primitives/primitives.hpp>
#include <boost/spirit/home/classic/core/composite/composite.hpp>
#include <boost/spirit/home/classic/meta/as_parser.hpp>

#include <boost/spirit/home/classic/phoenix/actor.hpp>
#include <boost/spirit/home/classic/phoenix/tuples.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN

// forward declaration
template <int N, typename ParserT, bool IsDefault> struct case_parser;

///////////////////////////////////////////////////////////////////////////////
namespace impl {

///////////////////////////////////////////////////////////////////////////////
//  parse helper functions
template <typename ParserT, typename ScannerT>
inline typename parser_result<ParserT, ScannerT>::type
delegate_parse(ParserT const &p, ScannerT const &scan,
    typename ScannerT::iterator_t const save)
{
    typedef typename parser_result<ParserT, ScannerT>::type result_t;

    result_t result (p.subject().parse(scan));
    if (!result)
        scan.first = save;
    return result;
}

///////////////////////////////////////////////////////////////////////////////
//  General default case handling (no default_p case branch given).
//  First try to match the current parser node (if the condition value is
//  matched) and, if that fails, return a no_match
template <int N, bool IsDefault, bool HasDefault>
struct default_delegate_parse {

    template <
        typename ParserT, typename DefaultT,
        typename ValueT, typename ScannerT
    >
    static typename parser_result<ParserT, ScannerT>::type
    parse (ValueT const &value, ParserT const &p, DefaultT const &,
        ScannerT const &scan, typename ScannerT::iterator_t const save)
    {
        if (value == N)
            return delegate_parse(p, scan, save);
        return scan.no_match();
    }
};

//  The current case parser node is the default parser.
//  Ignore the given case value and try to match the given default parser.
template <int N, bool HasDefault>
struct default_delegate_parse<N, true, HasDefault> {

    template <
        typename ParserT, typename DefaultT,
        typename ValueT, typename ScannerT
    >
    static typename parser_result<ParserT, ScannerT>::type
    parse (ValueT const& /*value*/, ParserT const &, DefaultT const &d,
        ScannerT const &scan, typename ScannerT::iterator_t const save)
    {
        //  Since there is a default_p case branch defined, the corresponding
        //  parser shouldn't be the nothing_parser
        BOOST_STATIC_ASSERT((!boost::is_same<DefaultT, nothing_parser>::value));
        return delegate_parse(d, scan, save);
    }
};

//  The current case parser node is not the default parser, but there is a
//  default_p branch given inside the switch_p parser.
//  First try to match the current parser node (if the condition value is
//  matched) and, if that fails, match the given default_p parser.
template <int N>
struct default_delegate_parse<N, false, true> {

    template <
        typename ParserT, typename DefaultT,
        typename ValueT, typename ScannerT
               >::left(p), scan, save);                                \
                                                                            \
            BOOST_PP_REPEAT_FROM_TO_ ## z(1, BOOST_PP_INC(N),               \
                BOOST_SPIRIT_PARSE_SWITCH_CASES, _)                         \
                                                                            \
            case (long)(left_t0::value):                                    \
            default:                                                        \
                typedef default_case<ParserT> default_t;                    \
                typedef                                                     \
                    default_delegate_parse<Value, IsDefault, default_t::value> \
                    default_parse_t;                                        \
                                                                            \
                return default_parse_t::parse(cond_value, p.right(),        \
                    default_t::get(p), scan, save);                         \
            }                                                               \
        }                                                                   \
    };                                                                      \
    /**/

BOOST_PP_REPEAT(BOOST_PP_DEC(BOOST_SPIRIT_SWITCH_CASE_LIMIT),
    BOOST_SPIRIT_PARSE_SWITCHES, _)

#undef BOOST_SPIRIT_PARSE_SWITCH_TYPEDEFS
#undef BOOST_SPIRIT_PARSE_SWITCH_CASES
#undef BOOST_SPIRIT_PARSE_SWITCHES
///////////////////////////////////////////////////////////////////////////////

template <typename LeftT, typename RightT, bool IsDefault>
template <typename ScannerT, typename CondT>
inline typename parser_result<
    compound_case_parser<LeftT, RightT, IsDefault>, ScannerT
>::type
compound_case_parser<LeftT, RightT, IsDefault>::
    parse(ScannerT const& scan, CondT const &cond) const
{
    scan.at_end();    // allow skipper to take effect
    return parse_switch<value, case_chain<self_t>::depth, is_default>::
        do_(*this, scan, cond(scan), scan.first);
}

///////////////////////////////////////////////////////////////////////////////
//  The switch condition is to be evaluated from a parser result value.
template <typename ParserT>
struct cond_functor {

    typedef cond_functor<ParserT> self_t;

    cond_functor(ParserT const &p_)
    :   p(p_)
    {}

    template <typename ScannerT>
    struct result
    {
        typedef typename parser_result<ParserT, ScannerT>::type::attr_t type;
    };

    template <typename ScannerT>
    typename condition_result<self_t, ScannerT>::type
    operator()(ScannerT const &scan) const
    {
        typedef typename parser_result<ParserT, ScannerT>::type result_t;
        typedef typename result_t::attr_t attr_t;

        result_t result(p.parse(scan));
        return !result ? attr_t() : result.value();
    }

    typename ParserT::embed_t p;
};

template <typename ParserT>
struct make_cond_functor {

    typedef as_parser<ParserT> as_parser_t;

    static cond_functor<typename as_parser_t::type>
    do_(ParserT const &cond)
    {
        return cond_functor<typename as_parser_t::type>(
            as_parser_t::convert(cond));
    }
};

///////////////////////////////////////////////////////////////////////////////
//  The switch condition is to be evaluated from a phoenix actor
template <typename ActorT>
struct cond_actor {

    typedef cond_actor<ActorT> self_t;

    cond_actor(ActorT const &actor_)
    :   actor(actor_)
    {}

    template <typename ScannerT>
    struct result
    {
        typedef typename ::phoenix::actor_result<ActorT, ::phoenix::tuple<> >::type
            type;
    };

    template <typename ScannerT>
    typename condition_result<self_t, ScannerT>::type
    operator()(ScannerT const& /*scan*/) const
    {
        return actor();
    }

    ActorT const &actor;
};

template <typename ActorT>
struct make_cond_functor< ::phoenix::actor<ActorT> > {

    static cond_actor< ::phoenix::actor<ActorT> >
    do_(::phoenix::actor<ActorT> const &actor)
    {
        return cond_actor< ::phoenix::actor<ActorT> >(actor);
    }
};

///////////////////////////////////////////////////////////////////////////////
//  The switch condition is to be taken directly from the input stream
struct get_next_token_cond {

    typedef get_next_token_cond self_t;

    template <typename ScannerT>
    struct result
    {
        typedef typename ScannerT::value_t type;
    };

    template <typename ScannerT>
    typename condition_result<self_t, ScannerT>::type
    operator()(ScannerT const &scan) const
    {
        typename ScannerT::value_t val(*scan);
        ++scan.first;
        return val;
    }
};

template <>
struct make_cond_functor<get_next_token_cond> {

    static get_next_token_cond
    do_(get_next_token_cond const &cond)
    {
        return cond;
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

BOOST_SPIRIT_CLASSIC_NAMESPACE_END

}}  // namespace boost::spirit

#endif  // BOOST_SPIRIT_SWITCH_IPP
