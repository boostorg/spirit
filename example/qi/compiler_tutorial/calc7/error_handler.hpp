/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_ERROR_HANDLER_HPP)
#define BOOST_SPIRIT_CALC7_ERROR_HANDLER_HPP

#include <iostream>
#include <string>
#include <boost/spirit/include/phoenix_function.hpp>

namespace client
{
    using boost::phoenix::function;

    ///////////////////////////////////////////////////////////////////////////////
    //  The error handler
    ///////////////////////////////////////////////////////////////////////////////
    struct error_handler_
    {
        template <typename, typename, typename, typename>
        struct result { typedef void type; };

        template <typename Message, typename What, typename Iterator>
        void operator()(Message const& message, What const& what
          , Iterator err_pos, Iterator last) const
        {
            std::cout
                << message
                << what
                << " here: \""
                << get_line(err_pos, last)
                << "\""
                << std::endl
            ;
        }

        template <typename Iterator>
        std::string get_line(Iterator err_pos, Iterator last) const
        {
            Iterator i = err_pos;
            // position i to the next EOL
            while (i != last && (*i != '\r' && *i != '\n'))
                ++i;
            return std::string(err_pos, i);
        }
    };

    boost::phoenix::function<error_handler_> const
        error_handler = error_handler_();
}

#endif

