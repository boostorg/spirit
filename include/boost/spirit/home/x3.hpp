/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_MARCH_04_2007_0852PM)
#define BOOST_SPIRIT_X3_MARCH_04_2007_0852PM

#if defined(_MSC_VER)
#pragma once
#endif

#if !defined BOOST_SPIRIT_X3_HIDE_CXX17_WARNING && !( true \
  && defined __cpp_if_constexpr && __cpp_if_constexpr >= 201606 \
  && defined __cpp_inline_variables && __cpp_inline_variables >= 201606 \
  && defined __cpp_fold_expressions && __cpp_fold_expressions >= 201603 \
  && defined __cpp_variadic_using && __cpp_variadic_using >= 201611 \
  && (defined __cpp_template_auto && __cpp_template_auto >= 201606 \
      || defined __cpp_nontype_template_parameter_auto && __cpp_nontype_template_parameter_auto >= 201606) \
  && defined __cpp_nontype_template_args && __cpp_nontype_template_args >= 201411 \
)
# define BOOST_SPIRIT_X3_STRINGIZE_IMPL(x) #x
# define BOOST_SPIRIT_X3_STRINGIZE(x) BOOST_SPIRIT_X3_STRINGIZE_IMPL(x)
# if defined __GNUC__ || defined __clang__
#  warning "Spirit X3 will soon use C++17 features which your compiler does not support"
#  warning "Minimal supported compiler versions: Clang 4 / GCC 7 / MSC 1915 (VS 2017 v15.8)"
#  if defined __clang__
#   pragma message "This compiler seems to be Clang " BOOST_SPIRIT_X3_STRINGIZE(__clang_major__)
#  else
#   pragma message "This compiler seems to be GCC " BOOST_SPIRIT_X3_STRINGIZE(__GNUC__)
#  endif
#  warning "Define BOOST_SPIRIT_X3_HIDE_CXX17_WARNING to hide the warning"
# elif defined _MSC_VER
#  pragma message (__FILE__ "(" BOOST_SPIRIT_X3_STRINGIZE(__LINE__) "): warning: Spirit X3 will soon use C++17 features which your compiler does not support" )
#  pragma message (__FILE__ "(" BOOST_SPIRIT_X3_STRINGIZE(__LINE__) "): warning: Minimal supported compiler versions: Clang 4 / GCC 7 / MSC 1915 (VS 2017 v15.8)" )
#  pragma message (__FILE__ "(" BOOST_SPIRIT_X3_STRINGIZE(__LINE__) "): warning: This compiler seems to be MSC " BOOST_SPIRIT_X3_STRINGIZE(_MSC_VER) )
#  pragma message (__FILE__ "(" BOOST_SPIRIT_X3_STRINGIZE(__LINE__) "): warning: Define BOOST_SPIRIT_X3_HIDE_CXX17_WARNING to hide the warning" )
# else
#  pragma message "warning: Spirit X3 will soon use C++17 features which your compiler does not support"
#  pragma message "warning: Minimal supported compiler versions: Clang 4 / GCC 7 / MSC 1915 (VS 2017 v15.8)"
#  pragma message "warning: Define BOOST_SPIRIT_X3_HIDE_CXX17_WARNING to hide the warning"
# endif
# undef BOOST_SPIRIT_X3_STRINGIZE
# undef BOOST_SPIRIT_X3_STRINGIZE_IMPL
#endif

#include <boost/spirit/home/x3/auxiliary.hpp>
#include <boost/spirit/home/x3/char.hpp>
#include <boost/spirit/home/x3/directive.hpp>
#include <boost/spirit/home/x3/nonterminal.hpp>
#include <boost/spirit/home/x3/numeric.hpp>
#include <boost/spirit/home/x3/operator.hpp>
#include <boost/spirit/home/x3/core.hpp>
#include <boost/spirit/home/x3/string.hpp>

#endif
