/*=============================================================================
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_DIRECTIVE_WITH_HPP
#define BOOST_SPIRIT_X3_DIRECTIVE_WITH_HPP

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject, typename ID, typename T>
    struct with_directive;

    namespace detail
    {
        template <typename Subject, typename ID, typename T>
        struct with_directive_impl
            : unary_parser<Subject, with_directive<Subject, ID, T>>
        {
            using base_type = unary_parser<Subject, with_directive<Subject, ID, T>>;
            mutable T val_;

            template <typename SubjectT, typename U>
                requires
                    std::is_constructible_v<Subject, SubjectT> &&
                    std::is_constructible_v<T, U>
            constexpr with_directive_impl(SubjectT&& subject, U&& val)
                noexcept(
                    std::is_nothrow_constructible_v<Subject, SubjectT> &&
                    std::is_nothrow_constructible_v<T, U>
                )
                : base_type(std::forward<SubjectT>(subject))
                , val_(std::forward<U>(val))
            {}
        };

        template <typename Subject, typename ID, typename T>
        struct with_directive_impl<Subject, ID, T const>
            : unary_parser<Subject, with_directive<Subject, ID, T const>>
        {
            using base_type = unary_parser<Subject, with_directive<Subject, ID, T const>>;
            /* not mutable */ T const val_;

            template <typename SubjectT, typename U>
                requires
                    std::is_constructible_v<Subject, SubjectT> &&
                    std::is_constructible_v<T const, U>
            constexpr with_directive_impl(SubjectT&& subject, U&& val)
                noexcept(
                    std::is_nothrow_constructible_v<Subject, SubjectT> &&
                    std::is_nothrow_constructible_v<T const, U>
                )
                : base_type(std::forward<SubjectT>(subject))
                , val_(std::forward<U>(val))
            {}
        };

        template <typename Subject, typename ID, typename T>
        struct with_directive_impl<Subject, ID, T&>
            : unary_parser<Subject, with_directive<Subject, ID, T&>>
        {
            using base_type = unary_parser<Subject, with_directive<Subject, ID, T&>>;
            T& val_;

            template <typename SubjectT, typename U>
                requires
                    std::is_constructible_v<Subject, SubjectT> &&
                    std::is_constructible_v<T&, U&>
            constexpr with_directive_impl(SubjectT&& subject, U& val)
                noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
                : base_type(std::forward<SubjectT>(subject))
                , val_(val)
            {}
        };
    } // detail

    // `with` directive injects a value into the context prior to parsing.
    // Holds lvalue references by reference, holds rvalue reference by value.
    template <typename Subject, typename ID, typename T>
    struct with_directive : detail::with_directive_impl<Subject, ID, T>
    {
        static_assert(
            !std::is_rvalue_reference_v<T>,
            "`x3::with`: rvalue reference is prohibited to prevent dangling reference"
        );

        static_assert(
            std::is_lvalue_reference_v<T> || std::is_move_constructible_v<T>,
            "Passing an rvalue to `x3::with` requires the type to be move-constructible "
            "so it can be stored by value."
        );

        using subject_type = Subject;
        using id_type = ID;
        using value_type = T;
        using base_type = detail::with_directive_impl<Subject, ID, T>;

        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT, typename U>
            requires std::is_constructible_v<base_type, SubjectT, U>
        constexpr with_directive(SubjectT&& subject, U&& val)
            noexcept(std::is_nothrow_constructible_v<base_type, SubjectT, U>)
            : base_type(std::forward<SubjectT>(subject), std::forward<U>(val))
        {}

        // The internal context type. This can be used to determine the composed
        // context type used in `x3::parse`/`x3::phrase_parse`. It is required for
        // the argument of `BOOST_SPIRIT_X3_INSTANTIATE`.
        template <typename Context>
        using context_t = context<ID, std::remove_reference_t<T>, Context>;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, context_t<Context>, RContext, Attribute>)
        {
            static_assert(Parsable<Subject, It, Se, context_t<Context>, RContext, Attribute>);
            return this->subject.parse(
                first, last,
                x3::make_context<ID>(this->val_, context),
                rcontext, attr
            );
        }

    private:
        using base_type::val_;
    };

    namespace detail
    {
        template <typename ID, typename T>
        struct [[nodiscard]] with_gen
        {
            static_assert(!std::is_rvalue_reference_v<T>);
            T val;

            template <X3Subject Subject>
            [[nodiscard]] constexpr with_directive<as_parser_plain_t<Subject>, ID, T>
            operator[](Subject&& subject) &&
                noexcept(
                    is_parser_nothrow_castable_v<Subject> &&
                    std::is_nothrow_constructible_v<
                        with_directive<as_parser_plain_t<Subject>, ID, T>,
                        as_parser_t<Subject>,
                        T&& // lvalue or rvalue, forwarded
                    >
                )
            {
                // with rvalue `with_gen`, the held value can always be forwarded
                return { as_parser(std::forward<Subject>(subject)), std::forward<T>(val) };
            }

            template <X3Subject Subject>
            [[nodiscard]] constexpr with_directive<as_parser_plain_t<Subject>, ID, T>
            operator[](Subject&& subject) const&
                noexcept(
                    is_parser_nothrow_castable_v<Subject> &&
                    std::is_nothrow_constructible_v<
                        with_directive<as_parser_plain_t<Subject>, ID, T>,
                        as_parser_t<Subject>,
                        T& // lvalue
                    >
                )
            {
                static_assert(
                    std::is_lvalue_reference_v<T> || std::is_copy_constructible_v<T>,
                    "When you have passed an rvalue to `x3::with` and saved the functor "
                    "as a local variable, it requires the held type to be copy-constructible. "
                    "If your type is move only, then apply `std::move` to your `x3::with<ID>(val)` "
                    "instance."
                );
                return { as_parser(std::forward<Subject>(subject)), val };
            }
        };

        template <typename ID>
        struct with_fn
        {
            template <typename T>
            static constexpr with_gen<ID, T> operator()(T&& val)
                noexcept(
                    std::is_lvalue_reference_v<T> ||
                    std::is_nothrow_move_constructible_v<T>
                )
            {
                return { std::forward<T>(val) };
            }
        };
    } // detail

    inline namespace cpos
    {
        // `with` directive injects a value into the context prior to parsing.
        // Holds lvalue references by reference, holds rvalue reference by value.
        template <typename ID>
        inline constexpr detail::with_fn<ID> with{};
    } // cpos
} // boost::spirit::x3

#endif
