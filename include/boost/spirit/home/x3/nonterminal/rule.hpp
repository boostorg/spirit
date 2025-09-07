/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#include <boost/spirit/home/x3/nonterminal/detail/rule_parser.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/config/helper_macros.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
#include <typeinfo>
#endif

namespace boost::spirit::x3
{
    template <typename ID, X3Subject RHS, typename Attribute, bool ForceAttribute, bool SkipDefinitionInjection = false>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>>
    {
        using this_type = rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>;
        using id = ID;
        using rhs_type = RHS;
        using lhs_type = rule<ID, Attribute, ForceAttribute>;
        using attribute_type = Attribute;

        static constexpr bool has_attribute = !std::is_same_v<Attribute, unused_type>;
        static constexpr bool handles_container = traits::is_container<Attribute>::value;
        static constexpr bool force_attribute = ForceAttribute;

        template <typename RHS_T>
            requires std::is_constructible_v<RHS, RHS_T>
        constexpr rule_definition(RHS_T&& rhs, char const* name)
            noexcept(std::is_nothrow_constructible_v<RHS, RHS_T>)
            : rhs(std::forward<RHS_T>(rhs))
            , name(name)
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute_>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, unused_type, Attribute_& attr) const
            // never noexcept; requires very complex implementation details
        {
            return detail::rule_parser<attribute_type, ID, SkipDefinitionInjection>
                ::template call_rule_definition<ForceAttribute>(
                    rhs, name, first, last, context, attr
                );
        }

        RHS rhs;
        char const* name = "unnamed";
    };

    template <typename ID, typename Attribute, bool ForceAttribute>
    struct rule : parser<rule<ID, Attribute, ForceAttribute>>
    {
        static_assert(!std::is_reference_v<Attribute>, "Reference qualifier on rule attribute type is meaningless");

        using id = ID;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<std::remove_const_t<Attribute>, unused_type>;
        static constexpr bool handles_container = traits::is_container<Attribute>::value;
        static constexpr bool force_attribute = ForceAttribute;

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
        rule() : name(typeid(rule).name()) {}
#else
        constexpr rule() noexcept : name("unnamed") {}
#endif

        constexpr rule(char const* name) noexcept
            : name(name)
        {}

        constexpr rule(rule const& r) noexcept
            : name(r.name)
        {
            // Assert that we are not copying an unitialized static rule. If
            // the static is in another TU, it may be initialized after we copy
            // it. If so, its name member will be nullptr.
            BOOST_ASSERT_MSG(r.name != nullptr, "uninitialized rule"); // static initialization order fiasco
        }

        template <X3Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute>
        operator=(RHS&& rhs) const&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <X3Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true>
        operator%=(RHS&& rhs) const&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        // When a rule placeholder constructed and immediately consumed it cannot be used recursively,
        // that's why the rule definition injection into a parser context can be skipped.
        // This optimization has a huge impact on compile times because immediate rules are commonly
        // used to cast an attribute like `as`/`attr_cast` does in Qi.
        template <X3Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute, true>
        operator=(RHS&& rhs) const&&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, ForceAttribute, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <X3Subject RHS>
        [[nodiscard]] constexpr rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true, true>
        operator%=(RHS&& rhs) const&&
            noexcept(
                is_parser_nothrow_castable_v<RHS> &&
                std::is_nothrow_constructible_v<
                    rule_definition<ID, as_parser_plain_t<RHS>, Attribute, true, true>,
                    as_parser_t<RHS>, char const*
                >
            )
        {
            return { as_parser(std::forward<RHS>(rhs)), name };
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Exposed>
            requires (!std::is_same_v<std::remove_const_t<Exposed>, unused_type>)
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, unused_type, Exposed& exposed_attr) const
            // never noexcept; requires very complex implementation details
        {
            static_assert(has_attribute, "A rule must have an attribute. Check your rule definition.");

            static_assert(
                traits::Transformable<Exposed, Attribute, detail::parser_id>,
                "Attribute type mismatch; the rule's attribute is not assignable to "
                "the exposed attribute, and no eligible specialization of "
                "`x3::traits::transform_attribute` was found."
            );

            using transform = traits::transform_attribute<Exposed, Attribute, detail::parser_id>;
            using transformed_type = typename transform::type;
            transformed_type transformed_attr = transform::pre(exposed_attr);

            // ADL
            if (static_cast<bool>(parse_rule(detail::rule_id<ID>{}, first, last, context, transformed_attr))) {
                transform::post(exposed_attr, std::forward<transformed_type>(transformed_attr));
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, unused_type, unused_type) const
            // never noexcept; requires very complex implementation details
        {
            // make sure we pass exactly the rule attribute type
            attribute_type no_attr; // default-initialize

            // ADL
            return static_cast<bool>(parse_rule(detail::rule_id<ID>{}, first, last, context, no_attr));
        }

        char const* name = "unnamed";
    };

    namespace traits
    {
        template <typename T, typename Enable = void>
        struct is_rule : std::false_type {};

        template <typename T>
        constexpr bool is_rule_v = is_rule<T>::value;

        template <typename ID, typename Attribute, bool ForceAttribute>
        struct is_rule<rule<ID, Attribute, ForceAttribute>> : std::true_type {};

        template <typename ID, typename Attribute, typename RHS, bool ForceAttribute, bool SkipDefinitionInjection>
        struct is_rule<rule_definition<ID, RHS, Attribute, ForceAttribute, SkipDefinitionInjection>> : std::true_type {};
    }

    template <typename T>
        requires traits::is_rule_v<T>
    struct get_info<T>
    {
        using result_type = std::string;
        [[nodiscard]] std::string operator()(T const& r) const
        {
            BOOST_ASSERT_MSG(r.name, "uninitialized rule"); // static initialization order fiasco
            return r.name? r.name : "uninitialized";
        }
    };

// -------------------------------------------------------------

#define BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN_I(x) _Pragma(#x)
#define BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN(msg) BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN_I(message(msg))

#define BOOST_SPIRIT_X3_DECLARE_(r, constexpr_, rule_type)                                       \
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>           \
    [[nodiscard]] constexpr_ bool                                                             \
    parse_rule(                                                                               \
        ::boost::spirit::x3::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>,    \
        It& first, Se const& last,                                                            \
        Context const& context, typename std::remove_cvref_t<rule_type>::attribute_type& attr \
    );

#define BOOST_SPIRIT_X3_DECLARE(rule_type) BOOST_SPIRIT_X3_DECLARE_(,, rule_type)
#define BOOST_SPIRIT_X3_DECLARE_CONSTEXPR(rule_type) BOOST_SPIRIT_X3_DECLARE_(, constexpr, rule_type)

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation. We simply
// can't drop (legit) use cases of `BOOST_SPIRIT_INSTANTIATE`, so
// this is a pure technical limitation. If you need `constexpr`
// support in your rule, use `BOOST_SPIRIT_X3_DECLARE_CONSTEXPR`.
#define BOOST_SPIRIT_DECLARE(...) \
    BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN( \
        "Use of variadic arguments with `BOOST_SPIRIT_DECLARE` is deprecated due to the heavy compile-time cost of " \
	"`BOOST_PP_SEQ_*`. Just apply `BOOST_SPIRIT_X3_DECLARE` for each of your rules." \
    ) \
    BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_X3_DECLARE_,, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))


#define BOOST_SPIRIT_X3_DEFINE_(r, constexpr_, rule_name)                                      \
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>         \
    [[nodiscard]] constexpr_ bool                                                           \
    parse_rule(                                                                             \
        ::boost::spirit::x3::detail::rule_id<std::remove_cvref_t<decltype(rule_name)>::id>, \
        It& first, Se const& last,                                                          \
        Context const& context,                                                             \
        std::remove_cvref_t<decltype(rule_name)>::attribute_type& attr                      \
    ) {                                                                                     \
        using rule_t = std::remove_cvref_t<decltype(rule_name)>;                            \
        return ::boost::spirit::x3::detail::rule_parser<                                    \
            typename rule_t::attribute_type, typename rule_t::id, true                      \
        >::call_rule_definition<rule_t::force_attribute>(                                   \
            BOOST_JOIN(rule_name, _def), rule_name.name,                                    \
            first, last, context, attr                                                      \
        );                                                                                  \
    }

#define BOOST_SPIRIT_X3_DEFINE(rule_type) BOOST_SPIRIT_X3_DEFINE_(,, rule_type)
#define BOOST_SPIRIT_X3_DEFINE_CONSTEXPR(rule_type) BOOST_SPIRIT_X3_DEFINE_(, constexpr, rule_type)

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation. We simply
// can't drop (legit) use cases of `BOOST_SPIRIT_INSTANTIATE`, so
// this is a pure technical limitation. If you need `constexpr`
// support in your rule, use `BOOST_SPIRIT_X3_DEFINE_CONSTEXPR`.
#define BOOST_SPIRIT_DEFINE(...) \
    BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN( \
        "Use of variadic arguments with `BOOST_SPIRIT_DEFINE` is deprecated due to the heavy compile-time cost of " \
	"`BOOST_PP_SEQ_*`. Just apply `BOOST_SPIRIT_X3_DEFINE` for each of your rules." \
    ) \
    BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_X3_DEFINE_,, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

    namespace detail
    {
        template <typename RuleT, std::forward_iterator It, typename A, typename B = void>
        struct instantiate_macro_helper
        {
            using rule_type = RuleT;
            using iterator_type = It;

            // Old API:
            //  A => Context
            //  B => void

            // New API:
            //  A => Se
            //  B => Context

            using sentinel_type = std::conditional_t<std::is_void_v<B>, It, A>;
            static_assert(std::sentinel_for<sentinel_type, It>);

            using context_type = std::conditional_t<std::is_void_v<B>, A, B>;
        };
    } // detail

#define BOOST_SPIRIT_X3_INSTANTIATE_(rule_type, It, Se, Context)                           \
    template bool parse_rule<It, Se, Context>(                                             \
        ::boost::spirit::x3::detail::rule_id<typename std::remove_cvref_t<rule_type>::id>, \
        It&, Se const&, Context const&,                                                    \
        typename std::remove_cvref_t<rule_type>::attribute_type&                           \
    );

#define BOOST_SPIRIT_X3_INSTANTIATE_WRAP(...) __VA_ARGS__

// NB: This can't be `constexpr`, because a constexpr declaration
// cannot be used with explicit template instantiation.
#define BOOST_SPIRIT_X3_INSTANTIATE(...) \
    BOOST_SPIRIT_X3_INSTANTIATE_( \
        BOOST_SPIRIT_X3_INSTANTIATE_WRAP(typename ::boost::spirit::x3::detail::instantiate_macro_helper<__VA_ARGS__>::rule_type), \
        BOOST_SPIRIT_X3_INSTANTIATE_WRAP(typename ::boost::spirit::x3::detail::instantiate_macro_helper<__VA_ARGS__>::iterator_type), \
        BOOST_SPIRIT_X3_INSTANTIATE_WRAP(typename ::boost::spirit::x3::detail::instantiate_macro_helper<__VA_ARGS__>::sentinel_type), \
        BOOST_SPIRIT_X3_INSTANTIATE_WRAP(typename ::boost::spirit::x3::detail::instantiate_macro_helper<__VA_ARGS__>::context_type) \
    )

#define BOOST_SPIRIT_INSTANTIATE(...) \
    BOOST_SPIRIT_X3_DEPRECATED_MACRO_WARN( \
        "Use `BOOST_SPIRIT_X3_INSTANTIATE`. `BOOST_SPIRIT_INSTANTIATE` is deprecated because " \
        "the name was not correctly prefixed with `X3`." \
    ) \
    BOOST_SPIRIT_X3_INSTANTIATE(__VA_ARGS__)

} // boost::spirit::x3

#endif
