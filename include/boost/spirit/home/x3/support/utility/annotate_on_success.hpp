/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3__ANNOTATE_ON_SUCCESS_HPP)
#define BOOST_SPIRIT_X3__ANNOTATE_ON_SUCCESS_HPP

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/utility/lambda_visitor.hpp>

#include <iterator>
#include <type_traits>

namespace boost::spirit::x3
{
    struct error_handler_tag;

    //  The on_success handler tags the AST with the iterator position
    //  for error handling.
    //
    //  The on_success handler also ties the AST to a vector of iterator
    //  positions for the purpose of subsequent semantic error handling
    //  when the program is being compiled. See x3::position_cache in
    //  x3/support/ast.
    //
    //  We'll ask the X3's error_handler utility to do these.

    struct annotate_on_success
    {
        // Catch-all default overload
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename T, typename Context>
        constexpr void
        on_success(It const& first, Se const& last, T& ast, Context const& context)
        {
            auto&& error_handler_ref = x3::get<error_handler_tag>(context);
            static_assert(
                !std::is_same_v<std::remove_cvref_t<decltype(error_handler_ref)>, unused_type>,
                "This rule is derived from `x3::annotate_on_success`, but no reference was bound to "
                "`x3::error_handler_tag`. You must provide a viable error handler via `x3::with`."
            );

            // unwrap `reference_wrapper` if neccessary
            if constexpr (requires {
                error_handler_ref.get().tag(ast, first, last);
            })
            {
                error_handler_ref.get().tag(ast, first, last);
            }
            else
            {
                error_handler_ref.tag(ast, first, last);
            }
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename... Types, typename Context>
        constexpr void
        on_success(It const& first, Se const& last, x3::variant<Types...>& ast, Context const& context)
        {
            ast.apply_visitor(x3::make_lambda_visitor<void>([&](auto& node)
            {
                this->on_success(first, last, node, context);
            }));
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename T, typename Context>
        constexpr void
        on_success(It const& first, Se const& last, forward_ast<T>& ast, Context const& context)
        {
            this->on_success(first, last, ast.get(), context);
        }
    };
} // boost::spirit::x3

#endif
