/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_DETAIL_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_DETAIL_RULE_JAN_08_2012_0326PM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/error_handler_types.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/directive/expect.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>

#if defined(BOOST_SPIRIT_X3_DEBUG)
#include <boost/spirit/home/x3/nonterminal/simple_trace.hpp>
#endif

#include <iterator>
#include <type_traits>
#include <concepts>
#include <utility>

namespace boost::spirit::x3
{
    template <typename ID, typename Attribute = unused_type, bool force_attribute = false>
    struct rule;

    struct parse_pass_context_tag;

    namespace detail
    {
        template <typename ID>
        struct rule_id {};

        // Placeholder type to detect whether the default `parse_rule(...)` is called
        enum struct default_parse_rule_result : bool {};

        template <typename ID, typename Context>
        constexpr bool context_has_rule_id = !std::is_same_v<
            std::remove_cvref_t<decltype(x3::get<ID>(std::declval<Context const&>()))>,
            unused_type
        >;
    } // detail

    // The default `parse_rule` definition.
    //
    // This overload will only be selected when there exists no user-defined
    // definition for `parse_rule`.
    //
    // When a user invokes `BOOST_SPIRIT_X3_DEFINE_`, an unconstrained overload
    // is generated at the user's namespace scope. It will never conflict with
    // this (vvvvv) overload, as the generated one is never directly called with
    // a context containing `ID`.
    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        requires detail::context_has_rule_id<ID, Context>
    [[nodiscard]] constexpr detail::default_parse_rule_result
    parse_rule(
        detail::rule_id<ID>,
        It& first, Se const& last,
        Context const& context, Attribute& attr
    )
    {
        auto&& rule_ = x3::get<ID>(context);
        return static_cast<detail::default_parse_rule_result>(
            rule_.parse(first, last, context, unused, attr)
        );
    }

    // This overload is selected only when the user *declares* their `parse_rule`
    // in the user's namespace scope AND the function definition is not found.
    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        requires (!detail::context_has_rule_id<ID, Context>)
    constexpr void
    parse_rule(
        detail::rule_id<ID>,
        It&, Se const&,
        Context const&, Attribute&
    ) = delete; // BOOST_SPIRIT_X3_DEFINE undefined for this rule

} // boost::spirit::x3

namespace boost::spirit::x3::detail
{
#if defined(BOOST_SPIRIT_X3_DEBUG)
    // TODO: This should be customizable by users
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
    struct scoped_debug
    {
        scoped_debug(
            char const* rule_name,
            It const& first, Se const& last,
            Attribute const& attr,
            bool const& parse_ok
        )
            : parse_ok(parse_ok)
            , rule_name(rule_name)
            , first(first)
            , last(last)
            , attr(attr)
            , f(detail::get_simple_trace())
        {
            f(first, last, attr, pre_parse, rule_name);
        }

        ~scoped_debug()
        {
            f(first, last, attr, parse_ok ? successful_parse : failed_parse, rule_name);
        }

        bool const& parse_ok;
        char const* rule_name = nullptr;
        It const& first;
        Se const& last;
        Attribute const& attr;
        detail::simple_trace_type& f;
    };
#endif

    template <typename ID, typename It, typename Se, typename Context>
    concept HasImmutableOnErrorOverload =
        std::forward_iterator<It> &&
        std::sentinel_for<Se, It> &&
        requires(ID& id) { // Note: `ID` should be non-const
            id.on_error(
                std::declval<It const&>(),
                std::declval<Se const&>(),
                std::declval<expectation_failure<It> const&>(),
                std::declval<Context const&>()
            );
        };

    template <typename ID, typename It, typename Se, typename Context>
    concept HasMutableOnErrorOverload =
        std::forward_iterator<It> &&
        std::sentinel_for<Se, It> &&
        requires(ID& id) { // Note: `ID` should be non-const
            id.on_error(
                std::declval<It&>(),
                std::declval<Se&>(),
                std::declval<expectation_failure<It> const&>(),
                std::declval<Context const&>()
            );
        };

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
    struct has_on_error : std::false_type {};

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        requires HasImmutableOnErrorOverload<ID, It, Se, Context>
    struct has_on_error<ID, It, Se, Context> : std::true_type
    {
        // We intentionally make this hard error to prevent error-prone definition
        static_assert(
            std::convertible_to<
                decltype(std::declval<ID&>().on_error(
                    std::declval<It const&>(),
                    std::declval<Se const&>(),
                    std::declval<expectation_failure<It> const&>(),
                    std::declval<Context const&>()
                )),
                x3::error_handler_result
            >,
            "The return type of `on_error` should be convertible to `x3::error_handler_result`."
        );
    };

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        requires
            (!HasImmutableOnErrorOverload<ID, It, Se, Context>) &&
            HasMutableOnErrorOverload<ID, It, Se, Context>
    struct has_on_error<ID, It, Se, Context> : std::false_type
    {
        // Historically, Spirit has passed mutable lvalue references of the iterators *as-is*
        // to the `on_success`/`on_error` handlers. This behavior was simply a mistake, because:
        //   (1) `on_success`/`on_error` mechanism was designed to be grammar-agnostic, and
        //   (2) it does not make sense to modify the grammar-specific iterator on the
        //       grammar-agnostic callback.
        //
        // Furthermore, any modification to X3's internal iterator variables may invoke
        // undefined behavior, since we never provide any kind of guarantee on how the
        // intermediate iterator variables are processed in X3's implementation details.
        static_assert(
            false,
            "The `on_error` callback should only accept const reference or passed-by-value iterators."
        );
    };

    template <typename ID, typename It, typename Se, typename Attribute, typename Context>
    concept HasImmutableOnSuccessOverload =
        std::forward_iterator<It> &&
        std::sentinel_for<Se, It> &&
        requires(ID& id) { // Note: `ID` should be non-const
            id.on_success(
                std::declval<It const&>(),
                std::declval<Se const&>(),
                std::declval<Attribute&>(),
                std::declval<Context const&>()
            );
        };

    template <typename ID, typename It, typename Se, typename Attribute, typename Context>
    concept HasMutableOnSuccessOverload =
        std::forward_iterator<It> &&
        std::sentinel_for<Se, It> &&
        requires(ID& id) { // Note: `ID` should be non-const
            id.on_success(
                std::declval<It&>(),
                std::declval<Se&>(),
                std::declval<Attribute&>(),
                std::declval<Context const&>()
            );
        };

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
    struct has_on_success : std::false_type {};

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
        requires HasImmutableOnSuccessOverload<ID, It, Se, Attribute, Context>
    struct has_on_success<ID, It, Se, Attribute, Context> : std::true_type
    {
        // We intentionally make this hard error to prevent error-prone definition
        static_assert(
            std::is_void_v<
                decltype(std::declval<ID&>().on_success(
                    std::declval<It const&>(),
                    std::declval<Se const&>(),
                    std::declval<Attribute&>(),
                    std::declval<Context const&>()
                ))
            >,
            "The return type of `on_success` should be `void`."
        );
    };

    template <typename ID, std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename Context>
        requires
            (!HasImmutableOnSuccessOverload<ID, It, Se, Attribute, Context>) &&
            HasMutableOnSuccessOverload<ID, It, Se, Attribute, Context>
    struct has_on_success<ID, It, Se, Attribute, Context> : std::false_type
    {
        // For details, see the comments on `has_on_error`.
        static_assert(
            false,
            "The `on_success` callback should only accept const reference or passed-by-value iterators."
        );
    };

    template <typename Attribute, typename ID, bool SkipDefinitionInjection = false>
    struct rule_parser
    {
        template <
            typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
            typename Context, typename RContext, typename ActualAttribute
        >
        [[nodiscard]] static constexpr bool
        parse_rhs(
            RHS const& rhs, It& first, Se const& last,
            Context const& context, RContext& rcontext, ActualAttribute& attr
        ) // never noexcept; requires complex handling
        {
            It start = first;

            // See if the user has a BOOST_SPIRIT_DEFINE for this rule
            using parse_rule_result_type = decltype(
                parse_rule( // ADL
                    std::declval<detail::rule_id<ID>>(), first, last,
                    std::declval<decltype(x3::make_unique_context<ID>(rhs, context))>(),
                    std::declval<Attribute&>()
                )
            );
            constexpr bool is_default_parse_rule = std::is_same_v<
                parse_rule_result_type, default_parse_rule_result
            >;

            bool ok;
            if constexpr (SkipDefinitionInjection || !is_default_parse_rule)
            {
                ok = rhs.parse(first, last, context, rcontext, attr);
            }
            else
            {
                // If there is no BOOST_SPIRIT_DEFINE for this rule,
                // we'll make a context for this rule tagged by its ID
                // so we can extract the rule later on in the default
                // parse_rule function.
                auto rule_id_context = x3::make_unique_context<ID>(rhs, context);
                ok = rhs.parse(first, last, rule_id_context, rcontext, attr);
            }

            // Note: this check uses `It, It` because the value is actually iterator-iterator pair
            if constexpr (has_on_success<ID, It, It, ActualAttribute, Context>::value)
            {
                if (!ok) return false;

                x3::skip_over(start, first, context);
                bool pass = true;
                ID().on_success(
                    std::as_const(start), std::as_const(first), attr,
                    x3::make_context<parse_pass_context_tag>(pass, context)
                );
                return pass;
            }
            else
            {
                return ok;
            }
        }

        template <
            typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
            typename Context, typename RContext, typename ActualAttribute
        >
        [[nodiscard]] static constexpr bool
        parse_rhs_with_on_error(
            RHS const& rhs, It& first, Se const& last,
            Context const& context, RContext& rcontext, ActualAttribute& attr
        ) // never noexcept; requires complex handling
        {
            while (true)
            {
            #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                try
            #endif
                {
                    if (rule_parser::parse_rhs(rhs, first, last, context, rcontext, attr))
                    {
                        return true;
                    }
                }

            #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
                catch (expectation_failure<It> const& x) {
            #else
                if (x3::has_expectation_failure(context)) {
                    auto const& x = x3::get_expectation_failure(context);
            #endif
                    // X3 developer note: don't forget to sync this implementation with x3::guard
                    switch (ID{}.on_error(std::as_const(first), std::as_const(last), x, context))
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

        template <
            bool ForceAttribute,
            typename RHS, std::forward_iterator It, std::sentinel_for<It> Se,
            typename Context, typename ActualAttribute
        >
        [[nodiscard]] static constexpr bool
        call_rule_definition(
            RHS const& rhs, char const* rule_name,
            It& first, Se const& last,
            Context const& context, ActualAttribute& attr
        )
        {
            // Do down-stream transformation, provides attribute for rhs parser
            using transform = traits::transform_attribute<ActualAttribute, Attribute, parser_id>;
            using transform_attr = typename transform::type;
            transform_attr attr_ = transform::pre(attr);

            // Creates a place to hold the result of parse_rhs
            // called inside the following scope.
            bool parse_ok;
            {
            #ifdef BOOST_SPIRIT_X3_DEBUG
                // Create a scope to cause the dbg variable below (within
                // the #if...#endif) to call it's DTOR before any
                // modifications are made to the attribute, attr_ passed
                // to parse_rhs (such as might be done in
                // transform::post when, for example,
                // ActualAttribute is a recursive variant).
                scoped_debug<It, Se, transform_attr>
                dbg(rule_name, first, last, attr_, parse_ok);
            #else
                (void)rule_name;
            #endif

                // In theory, these calls can be overloaded using tag dispatches.
                // However we should get over those legacy technique already, as they
                // lead to enormous amount of call stack, generating unreadable
                // compilation errors. Even when we have a single layer of tag dispatch,
                // we should generally avoid it because the "true_type/false_type"
                // argument placed at the very last param of the overload is virtually
                // indistinguishable in messages and has serious QoL issue in debuggers.
                constexpr bool rhs_has_on_error = has_on_error<ID, It, Se, Context>::value;

                // The existence of semantic action inhibits attribute materialization
                // _unless_ it is explicitly required by the user (primarily via `%=`).
                if constexpr (RHS::has_action && !ForceAttribute)
                {
                    if constexpr (rhs_has_on_error)
                    {
                        parse_ok = rule_parser::parse_rhs_with_on_error(
                            rhs, first, last, context, attr_ /* rcontext */, unused
                        );
                    }
                    else
                    {
                        parse_ok = rule_parser::parse_rhs(
                            rhs, first, last, context, attr_ /* rcontext */, unused
                        );
                    }
                }
                else // attribute is required
                {
                    if constexpr (rhs_has_on_error)
                    {
                        parse_ok = rule_parser::parse_rhs_with_on_error(
                            rhs, first, last, context, attr_ /* rcontext */, attr_
                        );
                    }
                    else
                    {
                        parse_ok = rule_parser::parse_rhs(
                            rhs, first, last, context, attr_ /* rcontext */, attr_
                        );
                    }
                }
            }
            if (parse_ok)
            {
                // do up-stream transformation, this integrates the results
                // back into the original attribute value, if appropriate
                transform::post(attr, std::forward<transform_attr>(attr_));
            }
            return parse_ok;
        }
    };
} // boost::spirit::x3::detail

#endif
