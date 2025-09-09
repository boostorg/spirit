/*=============================================================================
    Copyright (c) 2009 Chris Hoeppler
    Copyright (c) 2014 Lee Clagett
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(BOOST_SPIRIT_X3_CONFIX_MAY_30_2014_1819PM)
#define BOOST_SPIRIT_X3_CONFIX_MAY_30_2014_1819PM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template<typename Prefix, typename Subject, typename Postfix>
    struct confix_directive :
        unary_parser<Subject, confix_directive<Prefix, Subject, Postfix>>
    {
        using base_type = unary_parser<Subject, confix_directive<Prefix, Subject, Postfix>>;
        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename PrefixT, typename SubjectT, typename PostfixT>
            requires
                std::is_constructible_v<Prefix, PrefixT> &&
                std::is_constructible_v<Subject, SubjectT> &&
                std::is_constructible_v<Postfix, PostfixT>
        constexpr confix_directive(PrefixT&& prefix, SubjectT&& subject, PostfixT&& postfix)
            noexcept(
                std::is_nothrow_constructible_v<Prefix, PrefixT> &&
                std::is_nothrow_constructible_v<Subject, SubjectT> &&
                std::is_nothrow_constructible_v<Postfix, PostfixT>
            )
            : base_type(std::forward<SubjectT>(subject))
            , prefix_(std::forward<PrefixT>(prefix))
            , postfix_(std::forward<PostfixT>(postfix))
        {
        }

        template<std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const noexcept(
            std::is_nothrow_copy_assignable_v<It> &&
            is_nothrow_parsable_v<Prefix, It, Se, Context, RContext, unused_type> &&
            is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute> &&
            is_nothrow_parsable_v<Postfix, It, Se, Context, RContext, unused_type>
        )
        {
            It const saved_first = first;

            if (!(prefix_.parse(first, last, context, rcontext, unused) &&
                  this->subject.parse(first, last, context, rcontext, attr) &&
                  postfix_.parse(first, last, context, rcontext, unused))
            ) {
            #if !BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                if (x3::has_expectation_failure(context))
                {
                    // don't rollback iterator (mimicking exception-like behavior)
                    return false;
                }
            #endif

                first = saved_first;
                return false;
            }

            return true;
        }

    private:
        Prefix prefix_;
        Postfix postfix_;
    };

    namespace detail
    {
        template <X3Subject Prefix, X3Subject Postfix>
        struct [[nodiscard]] confix_gen
        {
            template <X3Subject Subject>
            [[nodiscard]] constexpr confix_directive<Prefix, as_parser_plain_t<Subject>, Postfix>
            operator[](Subject&& subject) const
                noexcept(
                    is_parser_nothrow_castable_v<Subject> &&
                    std::is_nothrow_constructible_v<
                        confix_directive<Prefix, as_parser_plain_t<Subject>, Postfix>,
                        Prefix const&,
                        as_parser_t<Subject>,
                        Postfix const&
                    >
                )
            {
                return { prefix, as_parser(std::forward<Subject>(subject)), postfix };
            }

            Prefix prefix;
            Postfix postfix;
        };

        struct confix_fn
        {
            template <X3Subject Prefix, X3Subject Postfix>
            static constexpr confix_gen<as_parser_plain_t<Prefix>, as_parser_plain_t<Postfix>>
            operator()(Prefix&& prefix, Postfix&& postfix)
                noexcept(
                    is_parser_nothrow_castable_v<Prefix> &&
                    is_parser_nothrow_castable_v<Postfix> &&
                    std::is_nothrow_constructible_v<
                        detail::confix_gen<as_parser_plain_t<Prefix>, as_parser_plain_t<Postfix>>,
                        as_parser_t<Prefix>, as_parser_t<Postfix>
                    >
                )
            {
                return { as_parser(std::forward<Prefix>(prefix)), as_parser(std::forward<Postfix>(postfix)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::confix_fn confix{};
    } // cpos
} // boost::spirit::x3

#endif
