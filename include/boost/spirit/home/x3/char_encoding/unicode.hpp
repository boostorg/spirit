/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CHAR_ENCODING_UNICODE_HPP
#define BOOST_SPIRIT_X3_CHAR_ENCODING_UNICODE_HPP

#include <boost/spirit/home/x3/char_encoding/unicode/classification.hpp>

#include <cstdint>

namespace boost::spirit::x3::char_encoding
{
    struct unicode
    {
        using char_type = char32_t;
        using classify_type = std::uint32_t;

        [[nodiscard]] static constexpr bool
        isascii_(char_type ch) noexcept
        {
            return 0 == (ch & ~0x7f);
        }

        [[nodiscard]] static constexpr bool
        ischar(char_type ch) noexcept
        {
            // unicode code points in the range 0x00 to 0x10FFFF
            return ch <= 0x10FFFF;
        }

        [[nodiscard]] static constexpr bool
        isalnum(char_type ch) noexcept
        {
            return x3::unicode::is_alphanumeric(ch);
        }

        [[nodiscard]] static constexpr bool
        isalpha(char_type ch) noexcept
        {
            return x3::unicode::is_alphabetic(ch);
        }

        [[nodiscard]] static constexpr bool
        isdigit(char_type ch) noexcept
        {
            return x3::unicode::is_decimal_number(ch);
        }

        [[nodiscard]] static constexpr bool
        isxdigit(char_type ch) noexcept
        {
            return x3::unicode::is_hex_digit(ch);
        }

        [[nodiscard]] static constexpr bool
        iscntrl(char_type ch) noexcept
        {
            return x3::unicode::is_control(ch);
        }

        [[nodiscard]] static constexpr bool
        isgraph(char_type ch) noexcept
        {
            return x3::unicode::is_graph(ch);
        }

        [[nodiscard]] static constexpr bool
        islower(char_type ch) noexcept
        {
            return x3::unicode::is_lowercase(ch);
        }

        [[nodiscard]] static constexpr bool
        isprint(char_type ch) noexcept
        {
            return x3::unicode::is_print(ch);
        }

        [[nodiscard]] static constexpr bool
        ispunct(char_type ch) noexcept
        {
            return x3::unicode::is_punctuation(ch);
        }

        [[nodiscard]] static constexpr bool
        isspace(char_type ch) noexcept
        {
            return x3::unicode::is_white_space(ch);
        }

        [[nodiscard]] static constexpr bool
        (isblank)(char_type ch) noexcept
        {
            return x3::unicode::is_blank(ch);
        }

        [[nodiscard]] static constexpr bool
        isupper(char_type ch) noexcept
        {
            return x3::unicode::is_uppercase(ch);
        }

        // Mixing character encodings is semantically wrong
        static constexpr void isascii_(char) = delete;
        static constexpr void isascii_(wchar_t) = delete;
        static constexpr void ischar(char) = delete;
        static constexpr void ischar(wchar_t) = delete;
        static constexpr void isalnum(char) = delete;
        static constexpr void isalnum(wchar_t) = delete;
        static constexpr void isalpha(char) = delete;
        static constexpr void isalpha(wchar_t) = delete;
        static constexpr void isdigit(char) = delete;
        static constexpr void isdigit(wchar_t) = delete;
        static constexpr void isxdigit(char) = delete;
        static constexpr void isxdigit(wchar_t) = delete;
        static constexpr void iscntrl(char) = delete;
        static constexpr void iscntrl(wchar_t) = delete;
        static constexpr void isgraph(char) = delete;
        static constexpr void isgraph(wchar_t) = delete;
        static constexpr void islower(char) = delete;
        static constexpr void islower(wchar_t) = delete;
        static constexpr void isprint(char) = delete;
        static constexpr void isprint(wchar_t) = delete;
        static constexpr void ispunct(char) = delete;
        static constexpr void ispunct(wchar_t) = delete;
        static constexpr void isspace(char) = delete;
        static constexpr void isspace(wchar_t) = delete;
        static constexpr void isblank(char) = delete;
        static constexpr void isblank(wchar_t) = delete;
        static constexpr void isupper(char) = delete;
        static constexpr void isupper(wchar_t) = delete;

        // Simple character conversions

        [[nodiscard]] static constexpr char_type
        tolower(char_type ch) noexcept
        {
            return x3::unicode::to_lowercase(ch);
        }

        [[nodiscard]] static constexpr char_type
        toupper(char_type ch) noexcept
        {
            return x3::unicode::to_uppercase(ch);
        }

        [[nodiscard]] static constexpr std::uint32_t
        toucs4(char_type ch) noexcept
        {
            return ch;
        }

        static constexpr void tolower(char) = delete;
        static constexpr void tolower(wchar_t) = delete;
        static constexpr void toupper(char) = delete;
        static constexpr void toupper(wchar_t) = delete;
        static constexpr void toucs4(char) = delete;
        static constexpr void toucs4(wchar_t) = delete;

        // Major Categories
#define BOOST_SPIRIT_X3_MAJOR_CATEGORY(name)                                       \
        [[nodiscard]] static constexpr bool                                     \
        is_##name(char_type ch) noexcept                                        \
        {                                                                       \
            return x3::unicode::get_major_category(ch) == x3::unicode::properties::name;        \
        }                                                                       \
        static constexpr void is_##name(char) = delete;                         \
        static constexpr void is_##name(wchar_t) = delete;

        BOOST_SPIRIT_X3_MAJOR_CATEGORY(letter)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(mark)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(number)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(separator)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(other)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(punctuation)
        BOOST_SPIRIT_X3_MAJOR_CATEGORY(symbol)

#undef BOOST_SPIRIT_X3_MAJOR_CATEGORY

        // General Categories
#define BOOST_SPIRIT_X3_CATEGORY(name)                                             \
        [[nodiscard]] static constexpr bool                                     \
        is_##name(char_type ch) noexcept                                        \
        {                                                                       \
            return x3::unicode::get_category(ch) == x3::unicode::properties::name;              \
        }                                                                       \
        static constexpr void is_##name(char) = delete;                         \
        static constexpr void is_##name(wchar_t) = delete;

        BOOST_SPIRIT_X3_CATEGORY(uppercase_letter)
        BOOST_SPIRIT_X3_CATEGORY(lowercase_letter)
        BOOST_SPIRIT_X3_CATEGORY(titlecase_letter)
        BOOST_SPIRIT_X3_CATEGORY(modifier_letter)
        BOOST_SPIRIT_X3_CATEGORY(other_letter)

        BOOST_SPIRIT_X3_CATEGORY(nonspacing_mark)
        BOOST_SPIRIT_X3_CATEGORY(enclosing_mark)
        BOOST_SPIRIT_X3_CATEGORY(spacing_mark)

        BOOST_SPIRIT_X3_CATEGORY(decimal_number)
        BOOST_SPIRIT_X3_CATEGORY(letter_number)
        BOOST_SPIRIT_X3_CATEGORY(other_number)

        BOOST_SPIRIT_X3_CATEGORY(space_separator)
        BOOST_SPIRIT_X3_CATEGORY(line_separator)
        BOOST_SPIRIT_X3_CATEGORY(paragraph_separator)

        BOOST_SPIRIT_X3_CATEGORY(control)
        BOOST_SPIRIT_X3_CATEGORY(format)
        BOOST_SPIRIT_X3_CATEGORY(private_use)
        BOOST_SPIRIT_X3_CATEGORY(surrogate)
        BOOST_SPIRIT_X3_CATEGORY(unassigned)

        BOOST_SPIRIT_X3_CATEGORY(dash_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(open_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(close_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(connector_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(other_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(initial_punctuation)
        BOOST_SPIRIT_X3_CATEGORY(final_punctuation)

        BOOST_SPIRIT_X3_CATEGORY(math_symbol)
        BOOST_SPIRIT_X3_CATEGORY(currency_symbol)
        BOOST_SPIRIT_X3_CATEGORY(modifier_symbol)
        BOOST_SPIRIT_X3_CATEGORY(other_symbol)

#undef BOOST_SPIRIT_X3_CATEGORY

        // Derived Categories
#define BOOST_SPIRIT_X3_DERIVED_CATEGORY(name)                                     \
        [[nodiscard]] static constexpr bool                                     \
        is_##name(char_type ch) noexcept                                        \
        {                                                                       \
            return x3::unicode::is_##name(ch);                                          \
        }                                                                       \
        static constexpr void is_##name(char) = delete;                         \
        static constexpr void is_##name(wchar_t) = delete;

        BOOST_SPIRIT_X3_DERIVED_CATEGORY(alphabetic)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(uppercase)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(lowercase)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(white_space)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(hex_digit)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(noncharacter_code_point)
        BOOST_SPIRIT_X3_DERIVED_CATEGORY(default_ignorable_code_point)

#undef BOOST_SPIRIT_X3_DERIVED_CATEGORY

        // Scripts
#define BOOST_SPIRIT_X3_SCRIPT(name)                                               \
        [[nodiscard]] static constexpr bool                                     \
        is_##name(char_type ch) noexcept                                        \
        {                                                                       \
            return x3::unicode::get_script(ch) == x3::unicode::properties::name;                \
        }                                                                       \
        static constexpr void is_##name(char) = delete;                         \
        static constexpr void is_##name(wchar_t) = delete;

        BOOST_SPIRIT_X3_SCRIPT(adlam)
        BOOST_SPIRIT_X3_SCRIPT(caucasian_albanian)
        BOOST_SPIRIT_X3_SCRIPT(ahom)
        BOOST_SPIRIT_X3_SCRIPT(arabic)
        BOOST_SPIRIT_X3_SCRIPT(imperial_aramaic)
        BOOST_SPIRIT_X3_SCRIPT(armenian)
        BOOST_SPIRIT_X3_SCRIPT(avestan)
        BOOST_SPIRIT_X3_SCRIPT(balinese)
        BOOST_SPIRIT_X3_SCRIPT(bamum)
        BOOST_SPIRIT_X3_SCRIPT(bassa_vah)
        BOOST_SPIRIT_X3_SCRIPT(batak)
        BOOST_SPIRIT_X3_SCRIPT(bengali)
        BOOST_SPIRIT_X3_SCRIPT(bhaiksuki)
        BOOST_SPIRIT_X3_SCRIPT(bopomofo)
        BOOST_SPIRIT_X3_SCRIPT(brahmi)
        BOOST_SPIRIT_X3_SCRIPT(braille)
        BOOST_SPIRIT_X3_SCRIPT(buginese)
        BOOST_SPIRIT_X3_SCRIPT(buhid)
        BOOST_SPIRIT_X3_SCRIPT(chakma)
        BOOST_SPIRIT_X3_SCRIPT(canadian_aboriginal)
        BOOST_SPIRIT_X3_SCRIPT(carian)
        BOOST_SPIRIT_X3_SCRIPT(cham)
        BOOST_SPIRIT_X3_SCRIPT(cherokee)
        BOOST_SPIRIT_X3_SCRIPT(chorasmian)
        BOOST_SPIRIT_X3_SCRIPT(coptic)
        BOOST_SPIRIT_X3_SCRIPT(cypro_minoan)
        BOOST_SPIRIT_X3_SCRIPT(cypriot)
        BOOST_SPIRIT_X3_SCRIPT(cyrillic)
        BOOST_SPIRIT_X3_SCRIPT(devanagari)
        BOOST_SPIRIT_X3_SCRIPT(dives_akuru)
        BOOST_SPIRIT_X3_SCRIPT(dogra)
        BOOST_SPIRIT_X3_SCRIPT(deseret)
        BOOST_SPIRIT_X3_SCRIPT(duployan)
        BOOST_SPIRIT_X3_SCRIPT(egyptian_hieroglyphs)
        BOOST_SPIRIT_X3_SCRIPT(elbasan)
        BOOST_SPIRIT_X3_SCRIPT(elymaic)
        BOOST_SPIRIT_X3_SCRIPT(ethiopic)
        BOOST_SPIRIT_X3_SCRIPT(georgian)
        BOOST_SPIRIT_X3_SCRIPT(glagolitic)
        BOOST_SPIRIT_X3_SCRIPT(gunjala_gondi)
        BOOST_SPIRIT_X3_SCRIPT(masaram_gondi)
        BOOST_SPIRIT_X3_SCRIPT(gothic)
        BOOST_SPIRIT_X3_SCRIPT(grantha)
        BOOST_SPIRIT_X3_SCRIPT(greek)
        BOOST_SPIRIT_X3_SCRIPT(gujarati)
        BOOST_SPIRIT_X3_SCRIPT(gurmukhi)
        BOOST_SPIRIT_X3_SCRIPT(hangul)
        BOOST_SPIRIT_X3_SCRIPT(han)
        BOOST_SPIRIT_X3_SCRIPT(hanunoo)
        BOOST_SPIRIT_X3_SCRIPT(hatran)
        BOOST_SPIRIT_X3_SCRIPT(hebrew)
        BOOST_SPIRIT_X3_SCRIPT(hiragana)
        BOOST_SPIRIT_X3_SCRIPT(anatolian_hieroglyphs)
        BOOST_SPIRIT_X3_SCRIPT(pahawh_hmong)
        BOOST_SPIRIT_X3_SCRIPT(nyiakeng_puachue_hmong)
        BOOST_SPIRIT_X3_SCRIPT(katakana_or_hiragana)
        BOOST_SPIRIT_X3_SCRIPT(old_hungarian)
        BOOST_SPIRIT_X3_SCRIPT(old_italic)
        BOOST_SPIRIT_X3_SCRIPT(javanese)
        BOOST_SPIRIT_X3_SCRIPT(kayah_li)
        BOOST_SPIRIT_X3_SCRIPT(katakana)
        BOOST_SPIRIT_X3_SCRIPT(kawi)
        BOOST_SPIRIT_X3_SCRIPT(kharoshthi)
        BOOST_SPIRIT_X3_SCRIPT(khmer)
        BOOST_SPIRIT_X3_SCRIPT(khojki)
        BOOST_SPIRIT_X3_SCRIPT(khitan_small_script)
        BOOST_SPIRIT_X3_SCRIPT(kannada)
        BOOST_SPIRIT_X3_SCRIPT(kaithi)
        BOOST_SPIRIT_X3_SCRIPT(tai_tham)
        BOOST_SPIRIT_X3_SCRIPT(lao)
        BOOST_SPIRIT_X3_SCRIPT(latin)
        BOOST_SPIRIT_X3_SCRIPT(lepcha)
        BOOST_SPIRIT_X3_SCRIPT(limbu)
        BOOST_SPIRIT_X3_SCRIPT(linear_a)
        BOOST_SPIRIT_X3_SCRIPT(linear_b)
        BOOST_SPIRIT_X3_SCRIPT(lisu)
        BOOST_SPIRIT_X3_SCRIPT(lycian)
        BOOST_SPIRIT_X3_SCRIPT(lydian)
        BOOST_SPIRIT_X3_SCRIPT(mahajani)
        BOOST_SPIRIT_X3_SCRIPT(makasar)
        BOOST_SPIRIT_X3_SCRIPT(mandaic)
        BOOST_SPIRIT_X3_SCRIPT(manichaean)
        BOOST_SPIRIT_X3_SCRIPT(marchen)
        BOOST_SPIRIT_X3_SCRIPT(medefaidrin)
        BOOST_SPIRIT_X3_SCRIPT(mende_kikakui)
        BOOST_SPIRIT_X3_SCRIPT(meroitic_cursive)
        BOOST_SPIRIT_X3_SCRIPT(meroitic_hieroglyphs)
        BOOST_SPIRIT_X3_SCRIPT(malayalam)
        BOOST_SPIRIT_X3_SCRIPT(modi)
        BOOST_SPIRIT_X3_SCRIPT(mongolian)
        BOOST_SPIRIT_X3_SCRIPT(mro)
        BOOST_SPIRIT_X3_SCRIPT(meetei_mayek)
        BOOST_SPIRIT_X3_SCRIPT(multani)
        BOOST_SPIRIT_X3_SCRIPT(myanmar)
        BOOST_SPIRIT_X3_SCRIPT(nag_mundari)
        BOOST_SPIRIT_X3_SCRIPT(nandinagari)
        BOOST_SPIRIT_X3_SCRIPT(old_north_arabian)
        BOOST_SPIRIT_X3_SCRIPT(nabataean)
        BOOST_SPIRIT_X3_SCRIPT(newa)
        BOOST_SPIRIT_X3_SCRIPT(nko)
        BOOST_SPIRIT_X3_SCRIPT(nushu)
        BOOST_SPIRIT_X3_SCRIPT(ogham)
        BOOST_SPIRIT_X3_SCRIPT(ol_chiki)
        BOOST_SPIRIT_X3_SCRIPT(old_turkic)
        BOOST_SPIRIT_X3_SCRIPT(oriya)
        BOOST_SPIRIT_X3_SCRIPT(osage)
        BOOST_SPIRIT_X3_SCRIPT(osmanya)
        BOOST_SPIRIT_X3_SCRIPT(old_uyghur)
        BOOST_SPIRIT_X3_SCRIPT(palmyrene)
        BOOST_SPIRIT_X3_SCRIPT(pau_cin_hau)
        BOOST_SPIRIT_X3_SCRIPT(old_permic)
        BOOST_SPIRIT_X3_SCRIPT(phags_pa)
        BOOST_SPIRIT_X3_SCRIPT(inscriptional_pahlavi)
        BOOST_SPIRIT_X3_SCRIPT(psalter_pahlavi)
        BOOST_SPIRIT_X3_SCRIPT(phoenician)
        BOOST_SPIRIT_X3_SCRIPT(miao)
        BOOST_SPIRIT_X3_SCRIPT(inscriptional_parthian)
        BOOST_SPIRIT_X3_SCRIPT(rejang)
        BOOST_SPIRIT_X3_SCRIPT(hanifi_rohingya)
        BOOST_SPIRIT_X3_SCRIPT(runic)
        BOOST_SPIRIT_X3_SCRIPT(samaritan)
        BOOST_SPIRIT_X3_SCRIPT(old_south_arabian)
        BOOST_SPIRIT_X3_SCRIPT(saurashtra)
        BOOST_SPIRIT_X3_SCRIPT(signwriting)
        BOOST_SPIRIT_X3_SCRIPT(shavian)
        BOOST_SPIRIT_X3_SCRIPT(sharada)
        BOOST_SPIRIT_X3_SCRIPT(siddham)
        BOOST_SPIRIT_X3_SCRIPT(khudawadi)
        BOOST_SPIRIT_X3_SCRIPT(sinhala)
        BOOST_SPIRIT_X3_SCRIPT(sogdian)
        BOOST_SPIRIT_X3_SCRIPT(old_sogdian)
        BOOST_SPIRIT_X3_SCRIPT(sora_sompeng)
        BOOST_SPIRIT_X3_SCRIPT(soyombo)
        BOOST_SPIRIT_X3_SCRIPT(sundanese)
        BOOST_SPIRIT_X3_SCRIPT(syloti_nagri)
        BOOST_SPIRIT_X3_SCRIPT(syriac)
        BOOST_SPIRIT_X3_SCRIPT(tagbanwa)
        BOOST_SPIRIT_X3_SCRIPT(takri)
        BOOST_SPIRIT_X3_SCRIPT(tai_le)
        BOOST_SPIRIT_X3_SCRIPT(new_tai_lue)
        BOOST_SPIRIT_X3_SCRIPT(tamil)
        BOOST_SPIRIT_X3_SCRIPT(tangut)
        BOOST_SPIRIT_X3_SCRIPT(tai_viet)
        BOOST_SPIRIT_X3_SCRIPT(telugu)
        BOOST_SPIRIT_X3_SCRIPT(tifinagh)
        BOOST_SPIRIT_X3_SCRIPT(tagalog)
        BOOST_SPIRIT_X3_SCRIPT(thaana)
        BOOST_SPIRIT_X3_SCRIPT(thai)
        BOOST_SPIRIT_X3_SCRIPT(tibetan)
        BOOST_SPIRIT_X3_SCRIPT(tirhuta)
        BOOST_SPIRIT_X3_SCRIPT(tangsa)
        BOOST_SPIRIT_X3_SCRIPT(toto)
        BOOST_SPIRIT_X3_SCRIPT(ugaritic)
        BOOST_SPIRIT_X3_SCRIPT(vai)
        BOOST_SPIRIT_X3_SCRIPT(vithkuqi)
        BOOST_SPIRIT_X3_SCRIPT(warang_citi)
        BOOST_SPIRIT_X3_SCRIPT(wancho)
        BOOST_SPIRIT_X3_SCRIPT(old_persian)
        BOOST_SPIRIT_X3_SCRIPT(cuneiform)
        BOOST_SPIRIT_X3_SCRIPT(yezidi)
        BOOST_SPIRIT_X3_SCRIPT(yi)
        BOOST_SPIRIT_X3_SCRIPT(zanabazar_square)
        BOOST_SPIRIT_X3_SCRIPT(inherited)
        BOOST_SPIRIT_X3_SCRIPT(common)
        BOOST_SPIRIT_X3_SCRIPT(unknown)

#undef BOOST_SPIRIT_X3_SCRIPT
    };

} // boost::spirit::x3::char_encoding

#endif
