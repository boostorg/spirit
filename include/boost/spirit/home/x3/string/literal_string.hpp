/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_LITERAL_STRING_APR_18_2006_1125PM)
#define BOOST_SPIRIT_X3_LITERAL_STRING_APR_18_2006_1125PM

#include <boost/spirit/home/x3/string/detail/string_parse.hpp>

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/no_case.hpp>
#include <boost/spirit/home/x3/support/utility/utf8.hpp>

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename String, typename Encoding, typename Attribute = std::basic_string<typename Encoding::char_type>>
    struct literal_string : parser<literal_string<String, Encoding, Attribute>>
    {
        static_assert(
            !std::is_pointer_v<std::decay_t<String>>,
            "`literal_string` for raw character pointer/array is banned; it has an undetectable risk of holding a dangling pointer."
        );
        static_assert(std::is_convertible_v<String, std::basic_string_view<typename String::value_type>>);

        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;
        static constexpr bool handles_container = has_attribute;

        template<class... Args>
            requires std::is_constructible_v<String, Args...>
        constexpr literal_string(Args&&... args)
            noexcept(std::is_nothrow_constructible_v<String, Args...>)
            : str(std::forward<Args>(args)...)
        {}

        template <typename Iterator, typename Context, typename Attribute_>
        [[nodiscard]] constexpr bool parse(
            Iterator& first, Iterator const& last,
            Context const& context, unused_type, Attribute_& attr
        ) const
        {
            x3::skip_over(first, last, context);
            return detail::string_parse(str, first, last, attr, x3::get_case_compare<encoding>(context));
        }

        String str;
    };

    template <typename String, typename Encoding, typename Attribute>
    struct get_info<literal_string<String, Encoding, Attribute>>
    {
        using result_type = std::string;
        [[nodiscard]] constexpr std::string operator()(literal_string<String, Encoding, Attribute> const& p) const
        {
            return '"' + x3::to_utf8(p.str) + '"';
        }
    };
} // boost::spirit::x3

#endif
