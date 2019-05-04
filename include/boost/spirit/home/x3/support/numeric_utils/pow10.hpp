/*=============================================================================
    Copyright (c) 2001-2019 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_POW10_DECEMBER_26_2008_1118AM)
#define BOOST_SPIRIT_X3_POW10_DECEMBER_26_2008_1118AM

#include <boost/config/no_tr1/cmath.hpp>
#include <boost/limits.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/traits/numeric_traits.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(push)
# pragma warning(disable: 4244)   // conversion from 'double' to 'float', possible loss of data
#endif

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    namespace detail
    {
        template <typename T, typename Enable = void>
        struct pow10_helper
        {
            static T call(unsigned dim)
            {
                using namespace std;    // allow for ADL to find the correct overload
                return pow(T(10), T(dim));
            }
        };

        template <>
        struct pow10_helper<unused_type>
        {
            static unused_type call(unused_type)
            {
                return unused;
            }
        };

        template <typename T>
        struct pow10_table
        {
            constexpr static std::size_t size =
                std::numeric_limits<T>::max_exponent10;

            constexpr pow10_table()
             : exponents()
            {
                exponents[0] = T(1);
                for (auto i = 1; i != size; ++i)
                    exponents[i] = exponents[i-1] * T(10);
            }

            T exponents[size];
        };

        template <typename T>
        struct native_pow10_helper
        {
            static T call(unsigned dim)
            {
                constexpr auto table = pow10_table<T>();
                return table.exponents[dim];
            }
        };

        template <>
        struct pow10_helper<float>
          : native_pow10_helper<float> {};

        template <>
        struct pow10_helper<double>
          : native_pow10_helper<double> {};

        template <>
        struct pow10_helper<long double>
          : native_pow10_helper<long double> {};
    }

    template <typename T>
    inline T pow10(unsigned dim)
    {
        return detail::pow10_helper<T>::call(dim);
    }
}}}}

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

#endif
