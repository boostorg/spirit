/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CHAR_SET_OCT_12_2014_1051AM)
#define BOOST_SPIRIT_X3_CHAR_SET_OCT_12_2014_1051AM

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/x3/char/detail/cast_char.hpp>
#include <boost/spirit/home/x3/char/detail/basic_chset.hpp>
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>
#include <boost/spirit/home/x3/support/utility/utf8.hpp>
#include <boost/spirit/home/x3/support/no_case.hpp>

#include <ranges>
#include <string_view>
#include <type_traits>

namespace boost::spirit::x3
{
    // Parser for a character range
    template <typename Encoding, typename Attribute = typename Encoding::char_type>
    struct char_range : char_parser<char_range<Encoding, Attribute>>
    {
        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

        constexpr char_range(char_type from_, char_type to_) noexcept
            : from(from_), to(to_)
        {}

        template <typename Char, typename Context>
            requires (std::is_convertible_v<std::remove_cvref_t<Char>, char_type>)
        [[nodiscard]] constexpr bool test(Char ch_, Context const& context) const noexcept
        {
            char_type ch = static_cast<char_type>(ch_);  // optimize for token based parsing
            return (x3::get_case_compare<encoding>(context)(ch, from) >= 0)
                && (x3::get_case_compare<encoding>(context)(ch , to) <= 0);
        }

        char_type from, to;
    };

    // Parser for a character set
    template <typename Encoding, typename Attribute = typename Encoding::char_type>
    struct char_set : char_parser<char_set<Encoding, Attribute>>
    {
        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

        template<std::ranges::forward_range R>
        constexpr char_set(R const& str)
            noexcept(detail::cast_char_noexcept<std::ranges::range_value_t<R>, char_type>)
        {
            static_assert(detail::cast_char_viable<std::ranges::range_value_t<R>, char_type>);

            using detail::cast_char; // ADL introduction

            for (auto definition = std::ranges::begin(str); definition != std::ranges::end(str);)
            {
                auto const ch = *definition;
                auto next_definition = std::next(definition);
                if (next_definition == std::ranges::end(str))
                {
                    chset.set(cast_char<char_type>(ch));
                    break;
                }
                auto next_ch = *next_definition;
                if (next_ch == '-')
                {
                    next_definition = std::next(next_definition);
                    if (next_definition == std::ranges::end(str))
                    {
                        chset.set(cast_char<char_type>(ch));
                        chset.set('-');
                        break;
                    }
                    chset.set(
                        cast_char<char_type>(ch),
                        cast_char<char_type>(*next_definition)
                    );
                }
                else
                {
                    chset.set(cast_char<char_type>(ch));
                }
                definition = next_definition;
            }
        }

        template <typename Char, typename Context>
        [[nodiscard]] constexpr bool test(Char ch_, Context const& context) const noexcept
        {
            return x3::get_case_compare<encoding>(context).in_set(ch_, chset);
        }

        detail::basic_chset<char_type> chset;
    };

    template <typename Encoding, typename Attribute>
    struct get_info<char_set<Encoding, Attribute>>
    {
        using result_type = std::string;
        [[nodiscard]] constexpr std::string operator()(char_set<Encoding, Attribute> const& /* p */) const
        {
            return "char-set"; // TODO: make more user-friendly
        }
    };

    template <typename Encoding, typename Attribute>
    struct get_info<char_range<Encoding, Attribute>>
    {
        using result_type = std::string;
        [[nodiscard]] constexpr std::string operator()(char_range<Encoding, Attribute> const& p) const
        {
            // TODO: make more user-friendly && make the format consistent with above
            return "char_range \"" + x3::to_utf8(Encoding::toucs4(p.from)) + '-' + x3::to_utf8(Encoding::toucs4(p.to))+ '"';
        }
    };

} // boost::spirit::x3

#endif
