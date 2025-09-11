/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_REAL_POLICIES_APRIL_17_2006_1158PM
#define BOOST_SPIRIT_X3_REAL_POLICIES_APRIL_17_2006_1158PM

#include <boost/spirit/home/x3/string/detail/string_parse.hpp>
#include <boost/spirit/home/x3/support/numeric_utils/extract_int.hpp>

#include <string_view>
#include <iterator>
#include <limits>

namespace boost::spirit::x3
{
    //  Default (unsigned) real number policies
    template <typename T>
    struct ureal_policies
    {
        // trailing dot policy suggested by Gustavo Guerra
        static constexpr bool allow_leading_dot = true;
        static constexpr bool allow_trailing_dot = true;
        static constexpr bool expect_dot = false;

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse_sign(It& /*first*/, Se const& /*last*/) noexcept
        {
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_n(It& first, Se const& last, Attribute& attr_)
            noexcept(noexcept(extract_uint<T, 10, 1, -1>::call(first, last, attr_)))
        {
            return extract_uint<T, 10, 1, -1>::call(first, last, attr_);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse_dot(It& first, Se const& last)
            noexcept(noexcept(*first) && noexcept(++first))
        {
            if (first == last || *first != '.')
            {
                return false;
            }
            ++first;
            return true;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_frac_n(It& first, Se const& last, Attribute& attr_)
            noexcept(noexcept(extract_uint<T, 10, 1, -1, true>::call(first, last, attr_)))
        {
            return extract_uint<T, 10, 1, -1, true>::call(first, last, attr_);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse_exp(It& first, Se const& last)
            noexcept(noexcept(*first) && noexcept(++first))
        {
            if (first == last || (*first != 'e' && *first != 'E'))
            {
                return false;
            }
            ++first;
            return true;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse_exp_n(It& first, Se const& last, int& attr_)
            noexcept(noexcept(extract_int<int, 10, 1, -1>::call(first, last, attr_)))
        {
            return extract_int<int, 10, 1, -1>::call(first, last, attr_);
        }

        // The parse_nan() and parse_inf() functions get called whenever
        // a number to parse does not start with a digit (after having
        // successfully parsed an optional sign).
        //
        // The functions should return true if a Nan or Inf has been found. In
        // this case the attr should be set to the matched value (NaN or
        // Inf). The optional sign will be automatically applied afterwards.
        //
        // The default implementation below recognizes representations of NaN
        // and Inf as mandated by the C99 Standard and as proposed for
        // inclusion into the C++0x Standard: nan, nan(...), inf and infinity
        // (the matching is performed case-insensitively).
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_nan(It& first, Se const& last, Attribute& attr_)
        {
            using namespace std::string_view_literals;

            if (first == last) return false;   // end of input reached
            if (*first != 'n' && *first != 'N') return false;   // not "nan"

            // nan[(...)] ?
            if (detail::string_parse("nan"sv, "NAN"sv, first, last, unused))
            {
                if (first != last && *first == '(')
                {
                    // skip trailing (...) part
                    It i = first;

                    while (++i != last && *i != ')')
                        ;
                    if (i == last)
                        return false;     // no trailing ')' found, give up

                    first = ++i;
                }
                attr_ = std::numeric_limits<T>::quiet_NaN();
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_inf(It& first, Se const& last, Attribute& attr_)
        {
            using namespace std::string_view_literals;

            if (first == last) return false;   // end of input reached
            if (*first != 'i' && *first != 'I') return false;   // not "inf"

            // inf or infinity ?
            if (detail::string_parse("inf"sv, "INF"sv, first, last, unused))
            {
                // skip allowed 'inity' part of infinity
                (void)detail::string_parse("inity"sv, "INITY"sv, first, last, unused);
                attr_ = std::numeric_limits<T>::infinity();
                return true;
            }
            return false;
        }
    };

    // Default (signed) real number policies
    template <typename T>
    struct real_policies : ureal_policies<T>
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse_sign(It& first, Se const& last)
            noexcept(noexcept(detail::extract_sign(first, last)))
        {
            return detail::extract_sign(first, last);
        }
    };

    template <typename T>
    struct strict_ureal_policies : ureal_policies<T>
    {
        static constexpr bool expect_dot = true;
    };

    template <typename T>
    struct strict_real_policies : real_policies<T>
    {
        static constexpr bool expect_dot = true;
    };
} // boost::spirit::x3

#endif
