/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Joel de Guzman
    Copyright (c) 2002 Juan Carlos Arevalo-Baeza
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_IF_HPP
#define BOOST_SPIRIT_IF_HPP

#if !defined(BOOST_SPIRIT_PARSER_HPP)
#include "boost/spirit/core/parser.hpp"
#endif

#if !defined(BOOST_SPIRIT_COMPOSITE_HPP)
#include "boost/spirit/core/composite/composite.hpp"
#endif

#if !defined(BOOST_SPIRIT_CONDITIONS_IPP)
#include "boost/spirit/dynamic/impl/conditions.ipp"
#endif

namespace boost { namespace spirit {

    namespace impl {

    //////////////////////////////////
    // if-else-parser, holds two alternative parsers and a conditional functor
    // that selects between them.
    template <typename ParserT_true, typename ParserT_false, typename CondT>
    struct if_else_parser
        : public condition_evaluator<CondT>
        , public binary
        <
            ParserT_true,
            ParserT_false,
            parser<if_else_parser<ParserT_true, ParserT_false, CondT> >
        >
    {
        typedef if_else_parser<ParserT_true, ParserT_false, CondT>  self_t;
        typedef condition_evaluator<CondT>                          eval_t;
        typedef binary<ParserT_true, ParserT_false, parser<self_t> >base_t;
        typedef CondT                                               condition_t;

        if_else_parser
        (
            ParserT_true  const& p_true,
            ParserT_false const& p_false,
            CondT         const& cond_
        )
            : eval_t(cond_)
            , base_t(p_true, p_false)
        { }

        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result
                <ParserT_true, ScannerT>::type   then_result_t;
            typedef typename parser_result
                <ParserT_false, ScannerT>::type  else_result_t;

            typename ScannerT::iterator_t const  save(scan.first);

            int length = evaluate(scan);
            if (length >= 0)
            {
                then_result_t then_result(this->left().parse(scan));
                if (then_result)
                {
                    length += then_result.length();
                    return scan.create_match(length, nil_t(), save, scan.first);
                }
            }
            else
            {
                else_result_t else_result(this->right().parse(scan));
                if (else_result)
                {
                    length += else_result.length();
                    return scan.create_match(length, nil_t(), save, scan.first);
                }
            }
            return scan.no_match();
        }
    };

    //////////////////////////////////
    // if-else-parser generator, takes the false-parser in brackets
    // and returns the if-else-parser.
    template <typename ParserT_true, typename CondT>
    struct if_else_parser_gen
    {
        if_else_parser_gen(ParserT_true const& p_true_, CondT const& cond_)
            : p_true(p_true_)
            , cond(cond_) {}

        template <typename ParserT_false>
        if_else_parser
        <
            ParserT_true,
            typename /*::boost::spirit::*/as_parser<ParserT_false>::type,
            CondT
        >
        operator[](ParserT_false const& subject) const
        {
            typedef ::boost::spirit::as_parser<ParserT_false> as_parser_t;
            typedef typename as_parser_t::type parser_t;

            typedef char assert_argument_must_be_a_parser
                [::boost::spirit::is_parser<parser_t>::value];

            return if_else_parser<ParserT_true, parser_t, CondT>
                (
                    p_true,
                    as_parser_t::convert(subject),
                    cond
                );
        }

        //typename ParserT_true::embed_t p_true;

        // these must be copies from the source parsers for now [m]
        ParserT_true const p_true;
        CondT const cond;
    };

    //////////////////////////////////
    // if-parser, conditionally runs a parser is a functor condition is true.
    // If the condition is fales, it fails the parse.
    // It can optionally become an if-else-parser through the member else_p.
    template <typename ParserT, typename CondT>
    struct if_parser
        : public condition_evaluator<CondT>
        , public unary<ParserT, parser<if_parser<ParserT, CondT> > >
    {
        typedef if_parser<ParserT, CondT>           self_t;
        typedef condition_evaluator<CondT>          eval_t;
        typedef unary<ParserT, parser<self_t> >     base_t;

        if_parser(ParserT const& p, CondT const& cond_)
            : eval_t(cond_)
            , base_t(p)
            , else_p(p, cond_)
        {}

        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<ParserT, ScannerT>::type t_result_t;
            typename ScannerT::iterator_t const save(scan.first);

            int length = evaluate(scan);
            if (length >= 0)
            {
                t_result_t then_result(this->subject().parse(scan));
                if (then_result)
                {
                    length += then_result.length();
                    return scan.create_match(length, nil_t(), save, scan.first);
                }
            }
            return scan.empty_match();
        }

        if_else_parser_gen<ParserT, CondT> else_p;
    };

    //////////////////////////////////
    // if-parser generator, takes the true-parser in brackets and returns the
    // if-parser.
    template <typename CondT>
    struct if_parser_gen
    {
        typedef ::boost::spirit::as_parser<CondT> c_as_parser_t;
        typedef typename c_as_parser_t::type      condition_t;

        if_parser_gen(CondT const& cond_) : cond(cond_) {}

        template <typename ParserT>
        if_parser
        <
            typename /*::boost::spirit::*/as_parser<ParserT>::type,
            condition_t
        >
        operator[](ParserT const& subject) const
        {
            typedef ::boost::spirit::as_parser<ParserT> p_as_parser_t;
            typedef typename p_as_parser_t::type        parser_t;

            typedef char assert_argument_must_be_a_parser
                [::boost::spirit::is_parser<parser_t>::value];

            return if_parser<parser_t, condition_t>
                (
                    p_as_parser_t::convert(subject),
                    c_as_parser_t::convert(cond)
                );
        }

        CondT const &cond;
    };

} // namespace impl

//////////////////////////////////
// if_p function, returns "if" parser generator

template <typename CondT>
impl::if_parser_gen<CondT>
if_p(CondT const& cond)
{
    return impl::if_parser_gen<CondT>(cond);
}

}} // namespace boost::spirit

#endif // BOOST_SPIRIT_IF_HPP
