/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CONTEXT_JAN_4_2012_1215PM)
#define BOOST_SPIRIT_X3_CONTEXT_JAN_4_2012_1215PM

#ifndef BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE 1
#endif
#if BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE
#else
  #pragma message "deprecated.  May cause infinite templaterecursion when using x3::skip(skipper)[some recursive rule]."
  //For example, see 
  /*
    https://stackoverflow.com/questions/45282293/spirit-not-able-to-use-x3skipskippersome-recursive-rule-in-its-rule-defin
   */
  //However, it's not clear what solution is, based on:
  /*
    https://sourceforge.net/p/spirit/mailman/message/35963822/
   */
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/mpl/identity.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename ID, typename T, typename Next = unused_type>
    struct context
    {
        context(T& val, Next const& next)
            : val(val), next(next) {}

        T& get(mpl::identity<ID>) const
        {
            return val;
        }

        template <typename ID_>
        decltype(auto) get(ID_ id) const
        {
            return next.get(id);
        }

        T& val;
        Next const& next;
    };

    template <typename ID, typename T>
    struct context<ID, T, unused_type>
    {
        context(T& val)
            : val(val) {}

        context(T& val, unused_type)
            : val(val) {}

        T& get(mpl::identity<ID>) const
        {
            return val;
        }

        template <typename ID_>
        unused_type get(ID_) const
        {
            return {};
        }

        T& val;
    };

    template <typename Tag, typename Context>
    inline decltype(auto) get(Context const& context)
    {
        return context.get(mpl::identity<Tag>());
    }

    template <typename ID, typename T, typename Next>
    inline context<ID, T, Next> make_context(T& val, Next const& next)
    {
        return { val, next };
    }

    template <typename ID, typename T>
    inline context<ID, T> make_context(T& val)
    {
        return { val };
    }

    namespace detail
    {
      #if BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE
        template <typename ID, typename T, typename Next, typename FoundVal>
        inline context<ID, T, Next>
        make_unique_context(T& val, Next const& next, FoundVal&)
        //T != FoundVal; so append val to existing context.
        {
            return { val, next};
        }
        template <typename ID, typename T, typename Next>
        inline Next const&
        make_unique_context(T& /* val */, Next const& next, T&)
        //T == decltype(x3::get<ID>(next)); so return old context.
        {
            return next;
        }
      #else
        template <typename ID, typename T, typename Next, typename FoundVal>
        inline Next const&
        make_unique_context(T& /* val */, Next const& next, FoundVal&fv)
        //x3::get<ID>(next)==fv; so return old context
        {
            return next;
        }
      #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_SKIP_MAKE_UNIQUE  
        template <typename ID, typename T, typename Next>
        inline context<ID, T, Next>
        make_unique_context(T& val, Next const& next, unused_type)
        //ID not found in next; so append val to existing context.
        {
            return { val, next };
        }
    }

    template <typename ID, typename T, typename Next>
    inline auto
    make_unique_context(T& val, Next const& next)
    {
        return detail::make_unique_context<ID>(val, next, x3::get<ID>(next));
    }
}}}

#endif
