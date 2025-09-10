/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CORE_CONFIG_HPP
#define BOOST_SPIRIT_X3_CORE_CONFIG_HPP

#include <boost/config.hpp>

#include <cstddef>

#if _MSC_VER
# include <CppCoreCheck/Warnings.h>
# pragma warning(default: CPPCORECHECK_LIFETIME_WARNINGS)
#endif

// <https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#c++20-[[no_unique_address]]>

#if _MSC_VER && _MSC_VER < 1929 // VS 2019 v16.9 or before
# error "Too old MSVC version; we don't support this because it leads to ODR violation regarding the existence of [[(msvc::)no_unique_address]]"
#endif

#if _MSC_VER && __INTELLISENSE__ // Memory Layout view shows wrong layout without this workaround
# define BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS [[msvc::no_unique_address, no_unique_address]]

#elif _MSC_VER // normal MSVC
# define BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]

#else // other compilers
# define BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ifndef BOOST_SPIRIT_X3_LIFETIMEBOUND
# if defined(__clang__)
#  define BOOST_SPIRIT_X3_LIFETIMEBOUND [[clang::lifetimebound]]
# elif defined(_MSC_VER)
#  define BOOST_SPIRIT_X3_LIFETIMEBOUND [[msvc::lifetimebound]]
# else
#  define BOOST_SPIRIT_X3_LIFETIMEBOUND
# endif
#endif

#endif
