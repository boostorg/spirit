/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_GUARD_FERBRUARY_02_2013_0649PM)
#define BOOST_SPIRIT_X3_GUARD_FERBRUARY_02_2013_0649PM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/error_handler_types.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject, typename Handler>
    struct guard : unary_parser<Subject, guard<Subject, Handler>>
    {
        static_assert(
            !std::is_reference_v<Handler>,
            "Reference type is disallowed for `Handler` to prevent dangling reference"
        );

        using base_type = unary_parser<Subject, guard<Subject, Handler>>;
        static constexpr bool is_pass_through_unary = true;

        Handler handler;

        template <typename SubjectT, typename HandlerT>
            requires std::is_constructible_v<Subject, SubjectT> && std::is_constructible_v<Handler, HandlerT>
        constexpr guard(SubjectT&& subject, HandlerT&& handler)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT> && std::is_nothrow_constructible_v<Handler, HandlerT>)
            : base_type(std::forward<SubjectT>(subject))
            , handler(std::forward<HandlerT>(handler))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            // never noexcept; requires complex implementation details
        {
            static_assert(Parsable<Subject, It, Se, Context, RContext, Attribute>);

            while (true)
            {
                It saved_it = first;

            #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                try
            #endif
                {
                    if (this->subject.parse(saved_it, last, context, rcontext, attr))
                    {
                        first = saved_it;
                        return true;
                    }
                }

            #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                catch (expectation_failure<It> const& x) {
            #else
                if (x3::has_expectation_failure(context)) {
                    auto const& x = x3::get_expectation_failure(context);
            #endif
                    static_assert(
                        std::is_invocable_r_v<
                            error_handler_result,
                            Handler const&,
                            It const&, Se const&,
                            std::remove_cvref_t<decltype(x)> const&,
                            Context const&
                        >,
                        "x3::guard: `Handler`'s signature is wrong"
                    );

                    // X3 developer note: don't forget to sync this implementation with x3::detail::rule_parser
                    switch (handler(std::as_const(first), std::as_const(last), x, context))
                    {
                        case error_handler_result::fail:
                            x3::clear_expectation_failure(context);
                            return false;

                        case error_handler_result::retry:
                            continue;

                        case error_handler_result::accept:
                            return true;

                        case error_handler_result::rethrow:
                        #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                            throw;
                        #else
                            return false; // TODO: design decision required
                        #endif
                    }
                }
                return false;
            }
        }
    };
} // boost::spirit::x3

#endif
