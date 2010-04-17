/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SEXPR_ERROR_HANDLER)
#define BOOST_SPIRIT_SEXPR_ERROR_HANDLER

#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <iostream>

namespace scheme { namespace input
{
    template <typename Iterator>
    struct error_handler
    {
        template <typename, typename, typename, typename>
        struct result { typedef void type; };

        void operator()(
            Iterator first, Iterator last,
            Iterator err_pos, boost::spirit::info const& what) const
        {
            // Default handler
            Iterator eol = err_pos;
            while (eol != last && *eol != '\n' && *eol != '\r')
                ++eol;

            std::cerr
                << "Error! Expecting "
                << what
                << " here: \""
                << std::string(err_pos, eol)
                << "\""
                << std::endl
            ;
        }
    };
}}

#endif
