/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_ERROR_HANDLER_APRIL_29_2007_1042PM)
#define BOOST_SPIRIT_ERROR_HANDLER_APRIL_29_2007_1042PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/operator/expect.hpp>
#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/function.hpp>

namespace boost { namespace spirit { namespace qi
{
    enum error_handler_result
    {
        fail
      , retry
      , accept
      , rethrow
    };

    template <
        typename Iterator, typename Context
      , typename Skipper, typename F, error_handler_result action
    >
    struct error_handler
    {
        typedef function<
            bool(Iterator& first, Iterator const& last
              , Context& context
              , Skipper const& skipper
            )>
        function_type;

        error_handler(function_type subject, F f)
          : subject(subject)
          , f(f)
        {
        }

        bool operator()(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper) const
        {
            while (true)
            {
                try
                {
                    Iterator i = first;
                    bool r = subject(i, last, context, skipper);
                    if (r)
                        first = i;
                    return r;
                }
                catch (expectation_failure<Iterator> const& x)
                {
                    typedef
                        fusion::vector<
                            Iterator&
                          , Iterator const&
                          , Iterator const&
                          , info const&>
                    params;
                    error_handler_result r = action;
                    params args(first, last, x.first, x.what);
                    f(args, context, r);

                    switch (r)
                    {
                        case fail: return false;
                        case retry: continue;
                        case accept: return true;
                        case rethrow: throw x;
                    }
                }
            }
            return false;
        }

        function_type subject;
        F f;
    };

    template <
        error_handler_result action
      , typename Iterator, typename T0, typename T1, typename T2
      , typename F>
    void on_error(rule<Iterator, T0, T1, T2>& r, F f)
    {
        typedef rule<Iterator, T0, T1, T2> rule_type;

        typedef
            error_handler<
                Iterator
              , typename rule_type::context_type
              , typename rule_type::skipper_type
              , F
              , action>
        error_handler;
        r.f = error_handler(r.f, f);
    }

    // Error handling support when <action> is not
    // specified. We will default to <fail>.
    template <typename Iterator, typename T0, typename T1
      , typename T2, typename F>
    void on_error(rule<Iterator, T0, T1, T2>& r, F f)
    {
        on_error<fail>(r, f);
    }
}}}

#endif
