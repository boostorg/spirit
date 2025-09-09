/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ACTION_JANUARY_07_2007_1128AM)
#define BOOST_SPIRIT_X3_ACTION_JANUARY_07_2007_1128AM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>
#include <boost/spirit/home/x3/core/call.hpp>

#ifndef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
# pragma message("Use of `boost::iterator_range` is deprecated in X3. #define BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE")
# include <boost/range/iterator_range_core.hpp>
#endif

#include <ranges>
#include <iterator>
#include <concepts>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    struct raw_attribute_type; // TODO: move this to detail

    // Ideally we should have a context-agnostic concept that can be used
    // like `X3ActionFunctor<F>`, but we technically can't.
    //
    // In order to check `std::invocable`, we need to know the actual context
    // type passed to the `.parse(...)` function but it is unknown until
    // runtime.
    //
    // Even if we make up the most trivial context type (i.e. `unused_type`),
    // such concept will be useless because a user-provided functor always
    // operates on user-specific precondition that assumes the context
    // holds exact specific type provided to the entry point (`x3::parse`).

    template <typename Subject, typename Action>
    struct action : unary_parser<Subject, action<Subject, Action>>
    {
        static_assert(
            !std::is_reference_v<Action>,
            "Reference type is disallowed for semantic action functor to prevent dangling reference"
        );

        using base_type = unary_parser<Subject, action<Subject, Action>>;
        static constexpr bool is_pass_through_unary = true;
        static constexpr bool has_action = true;

        Action f;

        template <typename SubjectT, typename ActionT>
            requires std::is_constructible_v<base_type, SubjectT> && std::is_constructible_v<Action, ActionT>
        constexpr action(SubjectT&& subject, ActionT&& f)
            noexcept(std::is_nothrow_constructible_v<base_type, SubjectT> && std::is_nothrow_constructible_v<Action, ActionT>)
            : base_type(std::forward<SubjectT>(subject))
            , f(std::forward<ActionT>(f))
        {
        }

        // attr==unused, action wants attribute
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, unused_type
        ) const noexcept(
            std::is_nothrow_default_constructible_v<traits::attribute_of_t<action<Subject, Action>, Context>> &&
            noexcept(this->parse_main(first, last, context, rcontext, std::declval<traits::attribute_of_t<action<Subject, Action>, Context>&>()))
        )
        {
            using attribute_type = traits::attribute_of_t<action<Subject, Action>, Context>;

            // Synthesize the attribute since one is not supplied
            attribute_type attribute; // default-initialize
            return this->parse_main(first, last, context, rcontext, attribute);
        }

        // Catch-all overload for non-unused_type attribute
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const noexcept(noexcept(this->parse_main(first, last, context, rcontext, attr)))
        {
            return this->parse_main(first, last, context, rcontext, attr);
        }

    private:
        // Compose attr(where(val(pass(context))))
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        using composed_context_t = x3::context<
            attr_context_tag,
            Attribute,
            x3::context<
                where_context_tag,
            #ifdef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
                std::ranges::subrange<It, Se> const,
            #else
                boost::iterator_range<It> const,
            #endif
                x3::context<
                    rule_val_context_tag,
                    RContext,
                    x3::context<
                        parse_pass_context_tag,
                        bool,
                        Context
                    >
                >
            >
        >;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires std::invocable<Action const&, composed_context_t<It, Se, Context, RContext, Attribute> const&>
        [[nodiscard]] constexpr bool
        call_action(
            It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) const noexcept(false) // construction of `subrange` is never noexcept as per the standard
        {
        #ifdef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
            using where_range_t = std::ranges::subrange<It, Se>;
        #else
            using where_range_t = boost::iterator_range<It>;
        #endif

            static_assert(
                std::is_void_v<std::invoke_result_t<Action const&, composed_context_t<It, Se, Context, RContext, Attribute> const&>>,
                "Semantic action should not return value. Check your function signature."
            );

            bool pass = true;
            auto const pass_context = x3::make_context<parse_pass_context_tag>(pass, context);

            auto const val_context = x3::make_context<rule_val_context_tag>(rcontext, pass_context);

            // TODO: Provide some trait to detect whether this is actually needed for
            // each semantic actions.
            //
            // Although this can be assumed to be eliminated in optimized code,
            // this still may introduce compile time overhead (and also runtime
            // overhead, as constructing `subrange` is never noexcept).
            where_range_t const where_rng(first, last);
            auto const where_context = x3::make_context<where_context_tag>(where_rng, val_context);

            auto const attr_context = x3::make_context<attr_context_tag>(attr, where_context);

            // Sanity check (internal check to detect implementation divergence)
            static_assert(std::same_as<
                std::remove_cvref_t<decltype(attr_context)>,
                composed_context_t<It, Se, Context, RContext, Attribute>
            >);

            this->f(attr_context);
            return pass;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!std::invocable<Action const&, composed_context_t<It, Se, Context, RContext, Attribute> const&>)
        [[nodiscard]] constexpr bool
        call_action(
            It&, Se const&,
            Context const&, RContext&, Attribute&
        ) const noexcept(std::is_nothrow_invocable_v<Action const&>)
        {
            // Explicitly make this hard error instead of emitting "no matching overload".
            // This provides much more human-friendly errors.
            static_assert(
                std::invocable<Action const&>,
                "Neither `f(ctx)` nor `f()` is well-formed for your semantic action. "
                "Check your function signature. Note that some functors might need "
                "`const` qualifier to satisfy the constraints."
            );

            static_assert(
                std::is_void_v<std::invoke_result_t<Action const&>>,
                "Semantic action should not return value. Check your function signature."
            );
            this->f();
            return true;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse_main(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const noexcept(
            std::is_copy_assignable_v<It> &&
            is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute> &&
            noexcept(this->call_action(first, last, context, rcontext, attr))
        )
        {
            It const saved_first = first;
            if (this->subject.parse(first, last, context, rcontext, attr))
            {
                if (this->call_action(first, last, context, rcontext, attr))
                {
                    return true;
                }

                // reset iterators if semantic action failed the match
                // retrospectively
                first = saved_first;
            }
            return false;
        }

        // attr==raw_attribute_type, action wants iterator_range (see raw.hpp)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse_main(
            It& first, Se const& last, Context const& context, RContext& rcontext, raw_attribute_type&
        ) const noexcept(false) // construction of `subrange` is never noexcept as per the standard
        {
        #ifdef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
            std::ranges::subrange<It, Se> rng;
        #else
            boost::iterator_range<It> rng;
        #endif
            // synthesize the attribute since one is not supplied
            return this->parse_main(first, last, context, rcontext, rng);
        }
    };

    template <X3Subject Subject, typename Action>
    [[nodiscard, deprecated(
        "Use `operator[]` instead. The symbol `/` normally means \"ordered choice\" "
        "in PEG, and is irrelevant to semantic actions. Furthermore, using C++'s "
        "`operator/` for this purpose may introduce surprising behavior when it's "
        "mixed with ordinary PEG operators, for instance, the unary `operator+`, "
        "due to precedence."
    )]]
    constexpr action<as_parser_plain_t<Subject>, std::remove_cvref_t<Action>>
    operator/(Subject&& p, Action&& f)
        noexcept(
            is_parser_nothrow_castable_v<Subject> &&
            std::is_nothrow_constructible_v<
                action<as_parser_plain_t<Subject>, std::remove_cvref_t<Action>>,
                as_parser_t<Subject>, Action
            >
        )
    {
        return { as_parser(std::forward<Subject>(p)), std::forward<Action>(f) };
    }
} // boost::spirit::x3

#endif
