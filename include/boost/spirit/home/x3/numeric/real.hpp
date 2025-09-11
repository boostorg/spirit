/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_REAL_APRIL_18_2006_0850AM
#define BOOST_SPIRIT_X3_REAL_APRIL_18_2006_0850AM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/numeric/real_policies.hpp>
#include <boost/spirit/home/x3/support/numeric_utils/extract_real.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename T, typename RealPolicies = real_policies<T>>
    struct real_parser : parser<real_parser<T, RealPolicies>>
    {
        using attribute_type = T;
        using policies_type = RealPolicies;

        static constexpr bool has_attribute = true;

        constexpr real_parser() = default;

        template <typename RealPoliciesT>
            requires
                (!std::is_same_v<std::remove_cvref_t<RealPoliciesT>, real_parser>) &&
                std::is_constructible_v<RealPolicies, RealPoliciesT>
        constexpr real_parser(RealPoliciesT&& policies)
            noexcept(std::is_nothrow_constructible_v<RealPolicies, RealPoliciesT>)
            : policies_(std::forward<RealPoliciesT>(policies))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last,
            Context const& context, unused_type, T& attr_
        ) const noexcept(
            noexcept(x3::skip_over(first, last, context)) &&
            noexcept(extract_real<T, RealPolicies>::parse(first, last, attr_, policies_))
        )
        {
            x3::skip_over(first, last, context);
            return extract_real<T, RealPolicies>::parse(first, last, attr_, policies_);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last,
            Context const& context, unused_type, Attribute& attr_param
        ) const noexcept(
            std::is_nothrow_default_constructible_v<T> &&
            noexcept(this->parse(first, last, context, unused, std::declval<T&>())) &&
            noexcept(traits::move_to(std::declval<T&&>(), attr_param))
        )
        {
            // this case is called when Attribute is not T
            T attr_;
            if (this->parse(first, last, context, unused, attr_))
            {
                traits::move_to(std::move(attr_), attr_param);
                return true;
            }
            return false;
        }

    private:
        RealPolicies policies_{};
    };

    inline namespace cpos
    {
        using float_type = real_parser<float>;
        inline constexpr float_type float_{};

        using double_type = real_parser<double>;
        inline constexpr double_type double_{};

        using long_double_type = real_parser<long double>;
        inline constexpr long_double_type long_double{};
    } // cpos
} // boost::spirit::x3

#endif
