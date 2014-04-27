/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_LOCALS_HPP_INCLUDED
#define BOOST_SPIRIT_X3_LOCALS_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <tuple>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/core/directive.hpp>


namespace boost { namespace spirit { namespace x3
{
    struct local_variables_tag;
    
    template <typename Context>
    inline auto
    _vars(Context const& context)
    -> decltype(x3::get<local_variables_tag>(context))
    {
        return x3::get<local_variables_tag>(context);
    }
        
    template <typename... Ts>
    struct locals_directive : directive<locals_directive<Ts...>>
    {
        static bool const is_pass_through_unary = true;
        
        template <typename Subject, typename Iterator, typename Context, typename Attribute, typename... As>
        bool parse(Subject const& subject, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, As&&... as) const
        {
            std::tuple<Ts...> locals(std::forward<As>(as)...);
            auto ctx(make_context<local_variables_tag>(locals, context));
            return subject.parse(first, last, ctx, attr);
        }
    };
    
    template <typename... Ts, typename... As>
    inline directive_caller<locals_directive<Ts...>, As...> locals(As&&... as)
    {
        return {locals_directive<Ts...>(), std::forward<As>(as)...};
    }
}}}


#endif

