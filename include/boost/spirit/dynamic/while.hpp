/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2002-2003 Joel de Guzman
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_WHILE_HPP
#define BOOST_SPIRIT_WHILE_HPP

#if !defined(BOOST_SPIRIT_PARSER_HPP)
#include "boost/spirit/core/parser.hpp"
#endif

#if !defined(BOOST_SPIRIT_COMPOSITE_HPP)
#include "boost/spirit/core/composite/composite.hpp"
#endif

#if !defined(BOOST_SPIRIT_CONDITIONS_IPP)
#include "boost/spirit/dynamic/impl/conditions.ipp"
#endif

////////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

    namespace impl {

    //////////////////////////////////
    // while parser
    // object are created by while_parser_gen and do_parser_gen
    template <typename ParserT, typename CondT, bool is_do_parser>
    struct while_parser
        : public condition_evaluator<CondT>
        , public unary // the parent stores a copy of the body parser
        <
            ParserT,
            parser<while_parser<ParserT, CondT, is_do_parser> >
        >
    {
        typedef unary
        <
            ParserT,
            parser<while_parser<ParserT, CondT, is_do_parser> >
        > parent_t;

        typedef while_parser<ParserT, CondT, is_do_parser> self_t;
        typedef condition_evaluator<CondT>                 cond_t;
        typedef CondT                                      condition_t;


        //////////////////////////////
        // constructor, saves condition and body parser
        while_parser(ParserT const &p_body, CondT const &cond_)
            : cond_t(cond_)
            , parent_t(p_body)
        {}

        //////////////////////////////
        // result type computer.
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result
                <ScannerT, nil_t>::type type;
        };

        //////////////////////////////
        // parse member function
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<ParserT, ScannerT>::type sresult_t;
            typedef typename ScannerT::iterator_t                   iterator_t;

            iterator_t save(scan.first);
            int length = 0;
            int eval_length = 0;

            bool dont_check_condition = is_do_parser;

            while (dont_check_condition || (eval_length=evaluate(scan))>=0)
            {
                dont_check_condition = false;
                length += eval_length;
                sresult_t tmp(this->subject().parse(scan));
                if (tmp)
                {
                    length+=tmp.length();
                }
                else
                {
                    return scan.no_match();
                }
            }
            return scan.create_match(length, nil_t(), save, scan.first);
        }
    };

    //////////////////////////////////
    // while-parser generator, takes the body-parser in brackets
    // and returns the actual while-parser.
    template <typename CondT>
    struct while_parser_gen
    {
        typedef typename ::boost::spirit::as_parser<CondT>::type condition_t;

        //////////////////////////////
        // constructor, saves the condition for use by operator[]
        while_parser_gen(CondT const& cond_) : cond(cond_) {}

        //////////////////////////////
        // operator[] returns the actual while-parser object
        template <typename ParserT>
        while_parser
        <
            typename ::boost::spirit::as_parser<ParserT>::type,
            condition_t,
            false
        >
        operator[](ParserT const &subject) const
        {
            typedef typename ::boost::spirit::as_parser<ParserT>::type parser_t;

            typedef char assert_argument_must_be_a_parser
                [::boost::spirit::is_parser<parser_t>::value];

            return while_parser<parser_t, condition_t, false>
            (
                ::boost::spirit::as_parser<ParserT>::convert(subject),
                ::boost::spirit::as_parser<CondT>::convert(cond)
            );
        }
    private:

        //////////////////////////////
        // the condition is stored by reference here.
        // this should not cause any harm since object of type
        // while_parser_gen<> are only used as temporaries
        // the while-parser object constructed by the operator[]
        // stores a copy of the condition.
        CondT const &cond;
    };

    //////////////////////////////////
    // do-while-parser generator, takes the condition as
    // parameter to while_p member function and returns the
    // actual do-while-parser.
    template <typename ParserT>
    struct do_while_parser_gen
    {
        //////////////////////////////
        // constructor. saves the body parser for use by while_p.
        explicit do_while_parser_gen(ParserT const &body_parser)
            : body(body_parser)
        {}

        //////////////////////////////
        // while_p returns the actual while-parser object
        template <typename CondT>
        while_parser
        <
            ParserT,
            typename ::boost::spirit::as_parser<CondT>::type,
            true
        >
        while_p(CondT cond) const
        {
            typedef typename ::boost::spirit::as_parser<CondT> cond_as_parser_t;
            typedef typename cond_as_parser_t::type condition_t;

            return while_parser<ParserT, condition_t, true>
                (body, ::boost::spirit::as_parser<CondT>::convert(cond));
        }
    private:

        //////////////////////////////
        // the body is stored by reference here
        // this should not cause any harm since object of type
        // do_while_parser_gen<> are only used as temporaries
        // the while-parser object constructed by the while_p
        // member function stores a copy of the body parser.
        ParserT const &body;
    };

    struct do_parser_gen
    {
        inline do_parser_gen() {}

        template <typename ParserT>
        impl::do_while_parser_gen
        <
            typename ::boost::spirit::as_parser<ParserT>::type
        >
        operator[](ParserT const& body) const
        {
            typedef typename ::boost::spirit::as_parser<ParserT>::type parser_t;

            typedef char assert_argument_must_be_a_parser
                [::boost::spirit::is_parser<parser_t>::value];

            return impl::do_while_parser_gen<parser_t>
                (::boost::spirit::as_parser<ParserT>::convert(body));
        }
    };
} // namespace impl

//////////////////////////////////
// while_p function, while-parser generator
// Usage: spirit::while_p(Condition)[Body]
template <typename CondT>
impl::while_parser_gen<CondT>
while_p(CondT const& cond)
{
    return impl::while_parser_gen<CondT>(cond);
}

//////////////////////////////////
// do_p functor, do-while-parser generator
// Usage: spirit::do_p[Body].while_p(Condition)
impl::do_parser_gen const do_p = impl::do_parser_gen();

}} // namespace boost::spirit

#endif // BOOST_SPIRIT_WHILE_HPP
