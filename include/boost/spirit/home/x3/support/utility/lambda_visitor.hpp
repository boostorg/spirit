/*=============================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_LAMBDA_VISITOR_MAY_19_2014_1116AM)
#define BOOST_SPIRIT_X3_LAMBDA_VISITOR_MAY_19_2014_1116AM

#include <boost/variant/static_visitor.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename RT, typename... F>
    struct lambda_visitor : static_visitor<RT>, F...
    {
        lambda_visitor(F... f) : F(f)... {}
    };

    template <typename RT, typename... Lambdas>
    lambda_visitor<RT, Lambdas...> make_lambda_visitor(Lambdas... lambdas)
    {
        return { lambdas... };
    }
}}}

#endif
