/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2011 Jan Frederick Eick
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_EXTRACT_INT_APRIL_17_2006_0830AM
#define BOOST_SPIRIT_X3_EXTRACT_INT_APRIL_17_2006_0830AM

#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/numeric_utils/detail/extract_int_impl.hpp>
#include <boost/assert.hpp>

#include <iterator>
#include <type_traits>

namespace boost::spirit::x3
{
    namespace detail
    {
        //  Extract the prefix sign (- or +), return true if a '-' was found
        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] constexpr bool
        extract_sign(It& first, Se const& last)
            noexcept(
                noexcept(*first) &&
                noexcept(++first)
            )
        {
            (void)last;
            BOOST_ASSERT(first != last); // precondition

            // Extract the sign
            bool const neg = *first == '-';
            if (neg || (*first == '+'))
            {
                ++first;
                return neg;
            }
            return false;
        }
    } // detail

    // Low level unsigned integer parser
    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits, bool Accumulate = false>
    struct extract_uint
    {
        // check template parameter 'Radix' for validity
        static_assert(Radix >= 2 && Radix <= 36, "Unsupported Radix");

        using extract_type = detail::extract_int<
            T, Radix, MinDigits, MaxDigits,
            detail::positive_accumulator<Radix>,
            Accumulate
        >;

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        call(It& first, Se const& last, T& attr)
            noexcept(std::is_nothrow_copy_assignable_v<It> && noexcept(extract_type::parse(first, last, attr)))
        {
            if (first == last) return false;

            It const save = first;
            if (!extract_type::parse(first, last, attr))
            {
                first = save;
                return false;
            }
            return true;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        call(It& first, Se const& last, Attribute& attr)
            noexcept(
                std::is_nothrow_default_constructible_v<T> &&
                noexcept(extract_uint::call(first, last, std::declval<T&>())) &&
                noexcept(traits::move_to(std::declval<T&&>(), attr))
            )
        {
            // this case is called when Attribute is not T
            T tmp_attr; // default initialize
            if (extract_uint::call(first, last, tmp_attr))
            {
                traits::move_to(std::move(tmp_attr), attr);
                return true;
            }
            return false;
        }
    };

    // Low level signed integer parser
    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits>
    struct extract_int
    {
        // check template parameter 'Radix' for validity
        static_assert(
            Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16,
            "Unsupported Radix"
        );

        using extract_pos_type = detail::extract_int<T, Radix, MinDigits, MaxDigits>;
        using extract_neg_type = detail::extract_int<T, Radix, MinDigits, MaxDigits, detail::negative_accumulator<Radix>>;

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        call(It& first, Se const& last, T& attr)
            noexcept(
                std::is_nothrow_copy_assignable_v<It> &&
                noexcept(detail::extract_sign(first, last)) &&
                noexcept(extract_neg_type::parse(first, last, attr)) &&
                noexcept(extract_pos_type::parse(first, last, attr))
            )
        {
            if (first == last) return false;

            It const save = first;
            bool hit = detail::extract_sign(first, last);
            if (hit)
            {
                hit = extract_neg_type::parse(first, last, attr);
            }
            else
            {
                hit = extract_pos_type::parse(first, last, attr);
            }

            if (!hit)
            {
                first = save;
                return false;
            }
            return true;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        call(It& first, Se const& last, Attribute& attr)
            noexcept(
                std::is_nothrow_default_constructible_v<T> &&
                noexcept(extract_int::call(first, last, std::declval<T&>())) &&
                noexcept(traits::move_to(std::declval<T&&>(), attr))
            )
        {
            // this case is called when Attribute is not T
            T tmp_attr; // default initialize
            if (extract_int::call(first, last, tmp_attr))
            {
                traits::move_to(std::move(tmp_attr), attr);
                return true;
            }
            return false;
        }
    };
} // boost::spirit::x3

#endif
