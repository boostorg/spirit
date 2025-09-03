/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013-2014 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_AUXILIARY_ANY_PARSER_APR_09_2014_1145PM)
#define BOOST_SPIRIT_X3_AUXILIARY_ANY_PARSER_APR_09_2014_1145PM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>

#include <boost/assert.hpp>

#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    // Type-erased parser that holds any underlying parser which models `X3Parser<It, It>`.
    template <
        std::forward_iterator It, // used only for polymorphism
        typename Attribute = unused_type,
        typename Context = unused_type // used only for polymorphism
    >
    struct [[deprecated(
        "Prefer static grammar (e.g. `a_prefix >> a_body | b_prefix >> b_body`) or "
        "static branching (e.g. `x3::eps[lazy_cond_func] >> a | b`)."
    )]]
    any_parser : parser<any_parser<It, Attribute, Context>>
    {
        // `any_parser` existed historically because writing a type-erased parser in C++
        // was cumbersome before modern language features. Its use is now discouraged for
        // technical reasons:
        //
        //  1) Performance: type erasure places dynamic indirection on the parsing hot path.
        //     Every token step must operate through type-erased iterators/contexts,
        //     inhibiting inlining and adding dispatch overhead.
        //
        //  2) Formal grounds: PEGs are deterministic (ordered choice) and do not require
        //     runtime reification of the grammar. The same behaviors can be expressed as
        //     a static composition of parsers with semantic predicates.
        //
        //  3) Empirical evidence: a GitHub full-text search indicates that `any_parser`
        //     is rarely used in practice.
        //
        // Reaching for `any_parser` means attempting to *define* the right-hand side of the
        // grammar dynamically from the left-hand side of the input. That indirection is
        // unnecessary in PEG; you can always structure the grammar statically. If more
        // complex branching is required, use the symbols parser or design a fully featured
        // parser class for your specific grammar.
        //
        // Note for X3 developers: if we allow the existence of `any_parser`, it would become
        // a very undesirable technical burden which prohibits the future enhancements.
        // Imagine that we're implementing an out-of-the-box packrat memoization support in
        // the future. The existence of `any_parser` makes the situation undesirably complex,
        // as we need to consider the *dynamic* properties of stateful parsers.

        using attribute_type = Attribute;

        static constexpr bool has_attribute = !std::is_same_v<Attribute, unused_type>;
        static constexpr bool handles_container = traits::is_container<Attribute>::value;

    public:
        constexpr any_parser() = default;

        template <typename Parser>
            requires
                (!std::is_same_v<std::remove_cvref_t<Parser>, any_parser>) &&
                X3Parser<Parser, It, It>
        constexpr any_parser(Parser&& parser)
            : parser_(std::make_unique<holder<as_parser_plain_t<Parser>>>(
                std::forward<Parser>(parser)
            ))
        {}

        constexpr any_parser(any_parser const& other)
            : parser_(other.parser_ ? other.parser_->clone() : nullptr)
        {}

        constexpr any_parser(any_parser&& other) = default;

        constexpr any_parser& operator=(any_parser const& other)
        {
            parser_.reset(other.parser_ ? other.parser_->clone() : nullptr);
            return *this;
        }

        any_parser& operator=(any_parser&& other) = default;

        // Runtime attribute is the same type as the predefined attribute type
        template <std::forward_iterator It_, std::sentinel_for<It_> Se_, typename Context_>
        [[nodiscard]] constexpr bool
        parse(
            It_& first, Se_ const& last,
            Context_ const& context, unused_type, Attribute& attr
        ) const
        {
            static_assert(std::is_same_v<It_, It>, "Runtime iterator type must match `any_parser`'s iterator type");
            BOOST_ASSERT_MSG(parser_ != nullptr, "Invalid use of uninitialized any_parser");

            return parser_->parse(first, last, context, attr);
        }

        // Runtime attribute is NOT the same type as the predefined attribute type
        template <std::forward_iterator It_, std::sentinel_for<It_> Se_, typename Context_, typename Attribute_>
        [[nodiscard]] constexpr bool
        parse(
            It_& first, Se_ const& last,
            Context_ const& context, unused_type, Attribute_& attr_
        ) const
        {
            Attribute attr; // default-initialize
            if (this->parse(first, last, context, unused, attr))
            {
                traits::move_to(std::move(attr), attr_);
                return true;
            }
            return false;
        }

        [[nodiscard]] constexpr std::string get_x3_info() const
        {
            return parser_ ? parser_->get_x3_info() : "(uninitialized `any_parser`)";
        }

    private:
        struct placeholder
        {
            constexpr virtual ~placeholder() = default;

            [[nodiscard]]
            constexpr virtual std::unique_ptr<placeholder> clone() const = 0;

            [[nodiscard]]
            constexpr virtual bool parse(
                It& first, It const& last, Context const& context, Attribute& attr
            ) const = 0;

            [[nodiscard]]
            constexpr virtual std::string get_x3_info() const = 0;
        };

        template <X3Parser<It, It> Parser>
        struct holder : placeholder
        {
            template <X3Subject Subject>
                requires is_parser_constructible_v<Parser, Subject>
            constexpr explicit holder(Subject&& subject)
                noexcept(is_parser_nothrow_constructible_v<Parser, Subject>)
                : parser_(as_parser(std::forward<Subject>(subject)))
            {}

            [[nodiscard]]
            constexpr std::unique_ptr<placeholder> clone() const override
            {
                return std::make_unique<holder>(*this);
            }

            [[nodiscard]]
            constexpr bool parse(
                It& first, It const& last,
                Context const& context, Attribute& attr
            ) const override
            {
                static_assert(Parsable<Parser, It, It, Context, unused_type, Attribute>);
                return parser_.parse(first, last, context, unused, attr);
            }

            [[nodiscard]]
            constexpr std::string get_x3_info() const override
            {
                return x3::what(parser_);
            }

        private:
            Parser parser_;
        };

        std::unique_ptr<placeholder> parser_;
    };

} // boost::spirit::x3

#endif
