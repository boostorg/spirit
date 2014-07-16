/*=============================================================================
    Copyright (c) 2014 Byoung-young Lee


    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_NO_CASE_OCTOBER_25_2008_0424PM)
#define SPIRIT_NO_CASE_OCTOBER_25_2008_0424PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <type_traits>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    // no_case_directive performs case-insensitive parsing
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject>
    struct no_case_directive : unary_parser<Subject, no_case_directive<Subject>>
    {
        typedef unary_parser<Subject, no_case_directive<Subject> > base_type;
        typedef Subject subject_type;
        no_case_directive(Subject const& subject)
        : base_type(subject) {}

        template<class Iterator, class Func>
        static auto transform(const Iterator &it, const Func &func)
        {
            return boost::make_transform_iterator(it, func);
        }

        template <typename Iterator, typename Context
          , typename RContext, typename Attribute, typename Func>
        bool parse(Func func, Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute &attr) const
        {
            auto begin = transform(first, func);
            const auto ret = this->subject.parse(
                begin, transform(last, func), context, rcontext, attr);
            first = begin.base();
            return ret;
        }

        template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& ctx, RContext& rctx, Attribute &attr) const
        {
            using spirit::traits::char_encoding_from_char;
            using Char = std::remove_reference_t<decltype(*Iterator())>;
            using Enc = typename char_encoding_from_char<Char>::type;
            if (Enc::islower(*first))
                return parse(Enc::tolower, first, last, ctx, rctx, attr)
                        || parse(Enc::toupper, first, last, ctx, rctx, attr);
            else
                return parse(Enc::toupper, first, last, ctx, rctx, attr)
                        || parse(Enc::tolower, first, last, ctx, rctx, attr);
        }
    };

    struct no_case_gen
    {
        template <typename Subject>
        no_case_directive<typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return {as_parser(subject)};
        }
    };

    no_case_gen const no_case = no_case_gen();
}}}

#endif
