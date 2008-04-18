/*=============================================================================
    Copyright (c) 2001-2008 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(BOOST_SPIRIT_PRIMITIVES_APR_17_2008_0751PM)
#define BOOST_SPIRIT_PRIMITIVES_APR_17_2008_0751PM

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    //  the eol_director matches line endings
    ///////////////////////////////////////////////////////////////////////////
    struct eol_director
    {
        template <typename Component, typename Context, typename Iterator>
        struct attribute
        {
            typedef unused_type type;
        };

        template <
            typename Component
          , typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        static bool parse(
            Component const& component
          , Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr)
        {
            qi::skip(first, last, skipper);

            bool matched = false;
            if (first != last && *first == '\r')    // CR
            {
                matched = true;
                ++first;
            }
            if (first != last && *first == '\n')    // LF
            {
                matched = true;
                ++first;
            }

            return matched;
        }

        template <typename Component>
        static std::string what(Component const&)
        {
            return "eol";
        }
    };
    
    ///////////////////////////////////////////////////////////////////////////
    //  the end_director matches the end of the input
    ///////////////////////////////////////////////////////////////////////////
    struct end_director
    {
        template <typename Component, typename Context, typename Iterator>
        struct attribute
        {
            typedef unused_type type;
        };

        template <
            typename Component
          , typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        static bool parse(
            Component const& component
          , Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr)
        {
            qi::skip(first, last, skipper);
            return first == last;
        }

        template <typename Component>
        static std::string what(Component const&)
        {
            return "end";
        }
    };
    
///////////////////////////////////////////////////////////////////////////////
}}}

#endif


