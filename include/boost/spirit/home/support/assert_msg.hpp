//  Copyright (c) 2001-2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_ASSERT_MSG_JUN_23_2009_0836AM)
#define BOOST_SPIRIT_ASSERT_MSG_JUN_23_2009_0836AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>

// Work around the MPL problem in BOOST_MPL_ASSERT_MSG generating
// multiple definition linker errors for certain compilers (VC++ 8).
// BOOST_SPIRIT_DONT_USE_MPL_ASSERT_MSG can also be defined by user.
#if !defined(BOOST_SPIRIT_DONT_USE_MPL_ASSERT_MSG)
# if defined(BOOST_MSVC) && BOOST_MSVC < 1500
#  define BOOST_SPIRIT_DONT_USE_MPL_ASSERT_MSG 1
# endif
#endif

#if !defined(BOOST_NO_CXX11_STATIC_ASSERT) || BOOST_SPIRIT_DONT_USE_MPL_ASSERT_MSG != 0
#include <boost/static_assert.hpp>
#define BOOST_SPIRIT_ASSERT_MSG(Cond, Msg, Types)                             \
        BOOST_STATIC_ASSERT_MSG(Cond, Msg)
#else
#include <boost/mpl/assert.hpp>
#define BOOST_SPIRIT_ASSERT_MSG(Cond, Msg, Types)                             \
        BOOST_MPL_ASSERT_MSG(Cond, Msg, Types)
#endif

#define BOOST_SPIRIT_ASSERT_MATCH(Domain, Expr)                               \
        BOOST_SPIRIT_ASSERT_MSG((                                             \
            boost::spirit::traits::matches<Domain, Expr>::value               \
        ), error_invalid_expression, (Expr))

#endif

