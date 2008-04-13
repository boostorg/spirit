/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_ACTION_JAN_07_2007_1128AM)
#define SPIRIT_ACTION_JAN_07_2007_1128AM

#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/support/component.hpp>
#include <boost/spirit/home/support/attribute_of.hpp>
#include <boost/spirit/home/support/detail/values.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_same.hpp>
#include <vector>

namespace boost { namespace spirit { namespace qi
{
    struct action
    {
        template <typename Component, typename Context, typename Iterator>
        struct attribute
          : traits::attribute_of<
                qi::domain
              , typename result_of::left<Component>::type
              , Context
              , Iterator
            >
        {
        };

        template <
            typename Component
          , typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        static bool parse(
            Component const& component
          , Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr_)
        {
            typedef typename
                result_of::left<Component>::type::director
            director;

            typedef typename
                attribute<Component, Context, Iterator>::type
            attr_type;

            // create an attribute if one is not supplied
            typename mpl::if_<
                is_same<typename remove_const<Attribute>::type, unused_type>
              , typename remove_const<attr_type>::type
              , Attribute&>::type
            attr = spirit::detail::make_value<attr_type>::call(attr_);

            if (director::parse(
                spirit::left(component), first, last, context, skipper, attr))
            {
                // call the function, passing the attribute, the context
                // and a bool flag that the client can set to false to
                // fail parsing.
                bool pass = true;
                spirit::right(component)(
                    spirit::detail::pass_value<attr_type>::call(attr), context, pass);
                return pass;
            }
            return false;
        }

        template <typename Component>
        static std::string what(Component const& component)
        {
            typedef typename
                result_of::left<Component>::type::director
            director;

            return director::what(spirit::left(component));
        }
    };
}}}

#endif
