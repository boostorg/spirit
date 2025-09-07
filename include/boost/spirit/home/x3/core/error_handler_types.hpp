/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CORE_ERROR_HANDLER_TYPES_HPP
#define BOOST_SPIRIT_X3_CORE_ERROR_HANDLER_TYPES_HPP

namespace boost::spirit::x3
{
    // Enum type used in `on_error`.
    enum class error_handler_result
    {
        fail,
        retry,
        accept,
        rethrow, // see BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE for alternative behaviors
    };

    // Note for X3 developers:
    // You must at least sync the implementation of `rule_parser` and `guard`
    // when you modify the semantics of error handlers.

} // boost::spirit::x3

#endif
