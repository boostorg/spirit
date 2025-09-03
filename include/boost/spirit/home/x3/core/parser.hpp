/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_PARSER_OCTOBER_16_2008_0254PM)
#define BOOST_SPIRIT_X3_PARSER_OCTOBER_16_2008_0254PM

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
#include <boost/assert.hpp>

#include <iterator>
#include <string>
#include <type_traits>
#include <concepts>
#include <utility>

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
#include <typeinfo>
#endif

namespace boost::spirit::x3
{
    template <typename Subject, typename Action>
    struct action;

    template <typename Subject, typename Handler>
    struct guard;

    namespace detail
    {
        struct parser_base {};
        struct parser_id;
    } // detail

    template <typename Derived>
    struct parser : detail::parser_base
    {
        static_assert(!std::is_reference_v<Derived>);
        using derived_type = Derived;

        static constexpr bool handles_container = false;
        static constexpr bool is_pass_through_unary = false;
        static constexpr bool has_action = false;

        [[nodiscard]] constexpr Derived& derived() & noexcept
        {
            return static_cast<Derived&>(*this);
        }

        [[nodiscard]] constexpr Derived const& derived() const& noexcept
        {
            return static_cast<Derived const&>(*this);
        }

        [[nodiscard]] constexpr Derived&& derived() && noexcept
        {
            return static_cast<Derived&&>(*this);
        }

        [[nodiscard]] constexpr Derived const&& derived() const&& noexcept
        {
            return static_cast<Derived const&&>(*this);
        }

        template <typename Self, typename Action>
            requires std::is_constructible_v<
                action<Derived, std::remove_cvref_t<Action>>,
                decltype(std::declval<Self>().derived()),
                Action
            >
        [[nodiscard]] constexpr action<Derived, std::remove_cvref_t<Action>>
        operator[](this Self&& self, Action&& f)
            noexcept(std::is_nothrow_constructible_v<
                action<Derived, std::remove_cvref_t<Action>>,
                decltype(std::forward<Self>(self).derived()),
                Action
            >)
        {
            return { std::forward<Self>(self).derived(), std::forward<Action>(f) };
        }

        template <typename Self, typename Handler>
            requires std::is_constructible_v<
                guard<Derived, std::remove_cvref_t<Handler>>,
                decltype(std::declval<Self>().derived()),
                Handler
            >
        [[nodiscard]] constexpr guard<Derived, std::remove_cvref_t<Handler>>
        on_error(this Self&& self, Handler&& f)
            noexcept(std::is_nothrow_constructible_v<
                guard<Derived, std::remove_cvref_t<Handler>>,
                decltype(std::forward<Self>(self).derived()),
                Handler
            >)
        {
            return { std::forward<Self>(self).derived(), std::forward<Handler>(f) };
        }
    };

    struct unary_category;
    struct binary_category;

    template <typename Subject, typename Derived>
    struct unary_parser : parser<Derived>
    {
        using category = unary_category;
        using subject_type = Subject;
        static constexpr bool has_action = Subject::has_action;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr unary_parser(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : subject(std::forward<SubjectT>(subject))
        {}

        [[nodiscard]] constexpr unary_parser const& get_unary() const noexcept { return *this; }

        Subject subject;
    };

    template <typename Left, typename Right, typename Derived>
    struct binary_parser : parser<Derived>
    {
        using category = binary_category;
        using left_type = Left;
        using right_type = Right;
        static constexpr bool has_action = left_type::has_action || right_type::has_action;

        template <typename LeftT, typename RightT>
            requires std::is_constructible_v<Left, LeftT> && std::is_constructible_v<Right, RightT>
        constexpr binary_parser(LeftT&& left, RightT&& right)
            noexcept(std::is_nothrow_constructible_v<Left, LeftT> && std::is_nothrow_constructible_v<Right, RightT>)
            : left(std::forward<LeftT>(left))
            , right(std::forward<RightT>(right))
        {}

        [[nodiscard]] constexpr binary_parser const& get_binary() const noexcept { return *this; }

        Left left;
        Right right;
    };

    // as_parser: convert a type, T, into a parser.
    namespace extension
    {
        // In short: if you want to customize the `as_parser(p)` behavior, just
        // specialize `x3::extension::as_parser` for your class.
        //
        // Older versions of X3 specified the signature `as_spirit_parser(p)` to be
        // used for the ADL adaptation of user-defined types. However, many parts in
        // X3 were instead using ADL of the form `as_parser(p)` to dispatch any types
        // in the first place. This makes it impossible for the control to reach the
        // `as_spirit_parser(p)` call if any user-defined `as_parser(p)` is *more
        // constrained* than `as_spirit_parser(p)`.
        //
        // In other words, the old signature `as_spirit_parser(p)` has never been
        // implemented correctly since its very first introduction to X3.
        //
        // Additionally, GitHub fulltext search shows that there exists zero usage of
        // `as_spirit_parser` except for the unmaintained blog post written by the
        // original inventor. Therefore, we simply removed the feature.
        //
        // Furthermore, the current maintainer of X3 believes there are no practical
        // use cases of ADL adaptation on user-defined types. As such, we make the
        // `x3::as_parser` (not to be confused with `x3::extension::as_parser`) to
        // model a C++20-ish CPO to inhibit undesired ADL in the first place.

        template <typename T>
        struct as_parser; // not defined

        template <>
        struct as_parser<unused_type>
        {
            using value_type [[deprecated("Use x3::as_parser_plain_t")]] = unused_type;

            template <typename T>
            [[nodiscard]] static constexpr auto&& call(T&& unused_) noexcept
            {
                return std::forward<T>(unused_);
            }
        };

        template <typename Derived>
            requires std::is_base_of_v<detail::parser_base, std::remove_cvref_t<Derived>>
        struct as_parser<Derived>
        {
            using value_type [[deprecated("Use x3::as_parser_plain_t")]] = std::remove_cvref_t<Derived>;

            template <typename T>
            [[nodiscard]] static constexpr auto&& call(T&& p) noexcept
            {
                return std::forward<T>(p);
            }
        };

        template <typename Derived>
        struct as_parser<parser<Derived>>
        {
            using value_type [[deprecated("Use x3::as_parser_plain_t")]] = Derived;

            template <typename T>
            [[nodiscard]] static constexpr auto&& call(T&& p) noexcept
            {
                return std::forward<T>(p).derived();
            }
        };
    }

    namespace detail
    {
        struct as_parser_fn
        {
            template <typename T>
            static void operator()(T&&) = delete; // If you reach here, your specialization of `x3::extension::as_parser` has a wrong signature, or the type is simply incompatible.

            // catch-all default fallback
            template <typename T>
                requires std::is_base_of_v<
                    parser_base,
                    std::remove_cvref_t<decltype(extension::as_parser<std::remove_cvref_t<T>>::call(std::declval<T>()))>
                >
            [[nodiscard]] static constexpr decltype(auto)
            operator()(T&& x) noexcept(noexcept(extension::as_parser<std::remove_cvref_t<T>>::call(std::forward<T>(x))))
            {
                return extension::as_parser<std::remove_cvref_t<T>>::call(std::forward<T>(x));
            }

            template <typename Derived>
            [[nodiscard]] static constexpr auto&&
            operator()(parser<Derived>& p) noexcept
            {
                return p.derived();
            }

            template <typename Derived>
            [[nodiscard]] static constexpr auto&&
            operator()(parser<Derived> const& p) noexcept
            {
                return p.derived();
            }

            template <typename Derived>
            [[nodiscard]] static constexpr auto&&
            operator()(parser<Derived>&& p) noexcept
            {
                return std::move(p).derived();
            }

            template <typename Derived>
            [[nodiscard]] static constexpr auto&&
            operator()(parser<Derived> const&& p) noexcept
            {
                return std::move(p).derived();
            }
        }; // as_parser_fn
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::as_parser_fn as_parser{};
    } // cpos

    template <typename T>
    using as_parser_t = decltype(as_parser(std::declval<T>())); // no ADL

    template <typename T>
    using as_parser_plain_t = std::remove_cvref_t<as_parser_t<T>>;


    // This is a very low level API provided for consistency with
    // `is_parser_nothrow_castable`. Most users should use `X3Subject`
    // instead.
    template <typename T>
    struct is_parser_castable : std::bool_constant<
        requires {
            { x3::as_parser(std::declval<T>()) };
        }
    > {};

    template <typename T>
    constexpr bool is_parser_castable_v = is_parser_castable<T>::value;

    // This trait can be used primarily for multi-parameter constructors.
    // For example, `a op b` normally requires the below condition to
    // become fully noexcept:
    //    is_parser_nothrow_castable_v<A> &&
    //    is_parser_nothrow_castable_v<B> &&
    //    std::is_nothrow_constructible_v<op_parser, as_parser_t<A>, as_parser_t<B>>
    template <typename T>
    struct is_parser_nothrow_castable : std::bool_constant<
        requires {
            { x3::as_parser(std::declval<T>()) } noexcept;
        }
    > {};

    template <typename T>
    constexpr bool is_parser_nothrow_castable_v = is_parser_nothrow_castable<T>::value;

    // A type that models `X3Subject` can be used in generic directives
    // and operators. Note that this concept is iterator-agnostic.
    //
    // For example, let `p` denote an object of `T`. Then, `!p` is a
    // well-formed NOT predicate in X3's domain (with "NOT predicate"
    // referring to that of the PEG semantics) if and only if `T`
    // models `X3Subject`.
    template <typename T>
    concept X3Subject =
        is_parser_castable_v<T> && // `as_parser(t)` is valid?
        std::is_base_of_v<detail::parser_base, as_parser_plain_t<T>>;

    // Checks whether `Parser(as_parser(t))` is valid.
    //
    // This trait can be used for checking whether a "Parser" is constructible
    // with some arbitrary argument `T`. In our (X3 core) use cases, the `Parser`
    // is usually a concrete type (e.g. `some_parser<as_parser_plain_t<Subject>>`)
    // whereas the `T` is some user-provided arbitrary type (e.g. `X3Subject Subject`).
    //
    // This interface can only be used to check whether `Parser`'s single-parameter
    // constructor is available. For multi-parameter construction, manually combine
    // `is_parser_castable` with `std::is_constructible`.
    template <X3Subject Parser, X3Subject T>
    struct is_parser_constructible : std::false_type {};

    template <X3Subject Parser, X3Subject T>
        requires std::is_constructible_v<Parser, as_parser_t<T>>
    struct is_parser_constructible<Parser, T> : std::true_type {};

    template <X3Subject Parser, X3Subject T>
    constexpr bool is_parser_constructible_v = is_parser_constructible<Parser, T>::value;

    // Checks whether `Parser(as_parser(t))` is noexcept.
    //
    // This interface can only be used to check whether `Parser`'s single-parameter
    // constructor is available. For multi-parameter construction, manually combine
    // `is_parser_nothrow_castable` with `std::is_nothrow_constructible`.
    template <X3Subject Parser, X3Subject T>
    struct is_parser_nothrow_constructible : std::false_type {};

    template <X3Subject Parser, X3Subject T>
        requires
            is_parser_nothrow_castable_v<T> &&
            std::is_nothrow_constructible_v<Parser, as_parser_t<T>>
    struct is_parser_nothrow_constructible<Parser, T> : std::true_type {};

    template <X3Subject Parser, X3Subject T>
    constexpr bool is_parser_nothrow_constructible_v = is_parser_nothrow_constructible<Parser, T>::value;


    template <
        typename Parser,
        typename It, // Don't constrain these; just let `static_assert` be engaged for friendly errors
        typename Se,
        typename Context,
        typename RContext,
        typename Attribute
    >
    struct is_parsable : std::bool_constant<
        requires(std::remove_cvref_t<as_parser_t<Parser>> const& p) // mutable parser use case is currently unknown
        {
            {
                p.parse(
                    std::declval<It&>(), // first
                    std::declval<Se>(), // last
                    std::declval<Context const&>(), // context
                    std::declval<RContext&>(), // rcontext
                    std::declval<Attribute&>() // attr
                )
            } -> std::convertible_to<bool>;
        }
    > {
        static_assert(X3Subject<Parser>);
        static_assert(!std::is_reference_v<It>);
        static_assert(std::forward_iterator<It>);
        static_assert(std::sentinel_for<Se, It>);
        static_assert(!std::is_reference_v<Context>);
        static_assert(!std::is_reference_v<RContext>);
        static_assert(!std::is_reference_v<Attribute>);
    };

    template <typename Parser, typename It, typename Se, typename Context, typename RContext, typename Attribute>
    constexpr bool is_parsable_v = is_parsable<Parser, It, Se, Context, RContext, Attribute>::value;

    // TODO: handle `parse_into_container` case
    template <typename Parser, typename It, typename Se, typename Context, typename RContext, typename Attribute>
    concept Parsable = is_parsable<Parser, It, Se, Context, RContext, Attribute>::value;
    // ^^^ this must be concept in order to provide better diagnostics (e.g. on MSVC)

    template <
        typename Parser,
        typename It, // Don't constrain these; just let `static_assert` be engaged for friendly errors
        typename Se,
        typename Context,
        typename RContext,
        typename Attribute
    >
    struct is_nothrow_parsable : std::bool_constant<
        requires(std::remove_cvref_t<as_parser_t<Parser>> const& p) // mutable parser use case is currently unknown
        {
            {
                p.parse(
                    std::declval<It&>(), // first
                    std::declval<Se>(), // last
                    std::declval<Context const&>(), // context
                    std::declval<RContext&>(), // rcontext
                    std::declval<Attribute&>() // attr
                )
            } noexcept -> std::convertible_to<bool>;
        }
    > {};

    template <typename Parser, typename It, typename Se, typename Context, typename RContext, typename Attribute>
    constexpr bool is_nothrow_parsable_v = is_nothrow_parsable<Parser, It, Se, Context, RContext, Attribute>::value;

    // The primary "parser" concept of X3, applicable in iterator-aware contexts.
    //
    // Let `it` denote an lvalue reference of `It`, and let `se` denote a prvalue of `Se`.
    // Let `p` denote an lvalue reference of `Parser`.
    //
    // For `Parser` to model `X3Parser`, the following conditions must be satisfied:
    //   -- the expression `x3::as_parser(p)` is well-formed in unevaluated context, and
    //   -- the expression `cp.parse(it, se, x3::unused, x3::unused, x3::unused)`
    //      is well-formed and the return type is convertible to `bool`, where `cp` denotes a
    //      const lvalue reference to the result of the expression `x3::as_parser(p)`.
    //
    // Although some exotic user-defined parser could be designed to operate on the very
    // specific context type and/or attribute type, we require the parser to at least
    // accept `unused_type` for `RContext`, `Context` and `Attribute`. This is because
    // core parsers of Spirit have historically been assuming natural use of `unused_type`
    // in many locations.
    template <typename Parser, class It, class Se>
    concept X3Parser =
        X3Subject<Parser> &&
        is_parsable_v<Parser, It, Se, unused_type, unused_type, unused_type>;


    // The runtime type info that can be obtained via `x3::what(p)`.
    template <X3Subject Subject, typename Enabled = void>
    struct get_info
    {
        [[nodiscard]] static constexpr std::string operator()(Subject const& subject)
        {
            if constexpr (requires {
                { subject.get_x3_info() } -> std::convertible_to<std::string>;
            })
            {
                return subject.get_x3_info();
            }
            else
            {
                (void)subject;
        #ifndef BOOST_SPIRIT_X3_NO_RTTI
                return typeid(Subject).name();
        #else
                return "(get_info undefined)";
        #endif
            }
        }
    };

    namespace detail
    {
        // "what" is an extremely common identifier that can be defined in many user-specific
        // namespaces. We should avoid ADL usage for such generic names in the first place.
        // (Note: CPO inhibits ADL in general.)
        struct what_fn
        {
            template <X3Subject Subject>
            [[nodiscard]] static constexpr std::string operator()(Subject const& p)
            {
                return get_info<Subject>{}(p);
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::what_fn what{}; // no ADL
    } // cpos

} // boost::spirit::x3

namespace boost::spirit::x3::traits
{
    template <typename Subject, typename Derived, typename Context>
    struct has_attribute<x3::unary_parser<Subject, Derived>, Context>
        : has_attribute<Subject, Context> {};

    template <typename Left, typename Right, typename Derived, typename Context>
    struct has_attribute<x3::binary_parser<Left, Right, Derived>, Context>
        : std::disjunction<has_attribute<Left, Context>, has_attribute<Right, Context>> {};
} // boost::spirit::x3::traits

#endif
