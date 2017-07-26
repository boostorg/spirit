/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_SKIP_JANUARY_26_2008_0422PM)
#define SPIRIT_SKIP_JANUARY_26_2008_0422PM

#ifndef BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
  #define BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE 1
#endif
#if BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
#else
  #pragma message "deprecated.  May cause infinite recursion when x3::skip used recursively."
  //For example, see 
  /*
    https://stackoverflow.com/questions/45282293/spirit-not-able-to-use-x3skipskippersome-recursive-rule-in-its-rule-defin
   */
  //However, it's not clear what solution is, based on:
  /*
    https://sourceforge.net/p/spirit/mailman/message/35963822/
   */
#endif//BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Subject>
    struct reskip_directive : unary_parser<Subject, reskip_directive<Subject>>
    {
        typedef unary_parser<Subject, reskip_directive<Subject>> base_type;
        static bool const is_pass_through_unary = true;
        static bool const handles_container = Subject::handles_container;

        reskip_directive(Subject const& subject)
          : base_type(subject) {}

        template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        typename disable_if<has_skipper<Context>, bool>::type
        parse(Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr) const
        {
            auto const& skipper =
                detail::get_unused_skipper(x3::get<skipper_tag>(context));

            return this->subject.parse(
                first, last
              , make_context<skipper_tag>(skipper, context)
              , rcontext
              , attr);
        }
        template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        typename enable_if<has_skipper<Context>, bool>::type
        parse(Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr) const
        {
            return this->subject.parse(
                first, last
              , context
              , rcontext
              , attr);
        }
    };

    template <typename Subject, typename Skipper>
    struct skip_directive : unary_parser<Subject, skip_directive<Subject, Skipper>>
    {
        typedef unary_parser<Subject, skip_directive<Subject, Skipper>> base_type;
        static bool const is_pass_through_unary = true;
        static bool const handles_container = Subject::handles_container;

        skip_directive(Subject const& subject, Skipper const& skipper)
          : base_type(subject)
          , skipper(skipper)
        {}

      #if BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
        
        template <typename ID, typename T, typename Next, typename FoundVal>
        static inline context<ID, T, Next>
        detail_unique_val_context(T& val, Next const& next, FoundVal&)
        //T != FoundVal; so return new context
        {
            return { val, next};
        }
        
        template <typename ID, typename T, typename Next>
        static inline Next const&
        detail_unique_val_context(T& /* val */, Next const& next, T&)
        //T == FoundVal; so return old context.
        {
            return next;
        }
        
        template <typename ID, typename T, typename Next>
        static inline context<ID, T, Next>
        detail_unique_val_context(T& val, Next const& next, unused_type)
        //ID not found in context; so append val to existing context.
        {
            return { val, next };
        }
        
        template <typename ID, typename T, typename Next>
        static inline auto
        make_unique_val_context(T& val, Next const& next)
        {
            return detail_unique_val_context<ID>(val, next, x3::get<ID>(next));
        }
      #endif 
            
       template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr) const
        {
            return this->subject.parse(
                first, last
            #if BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
              , make_unique_val_context<skipper_tag>(skipper, context)
            #else
              , make_context<skipper_tag>(skipper, context)
            #endif 
              , rcontext
              , attr);
        }

        Skipper const skipper;
    };

    struct reskip_gen
    {
        template <typename Skipper>
        struct skip_gen
        {
            skip_gen(Skipper const& skipper)
              : skipper_(skipper) {}

            template <typename Subject>
            skip_directive<typename extension::as_parser<Subject>::value_type, Skipper>
            operator[](Subject const& subject) const
            {
                return { as_parser(subject), skipper_ };
            }

            Skipper skipper_;
        };

        template <typename Skipper>
        skip_gen<Skipper> const operator()(Skipper const& skipper) const
        {
            return { skipper };
        }

        template <typename Subject>
        reskip_directive<typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return { as_parser(subject) };
        }
    };

    auto const skip = reskip_gen{};
}}}

#endif
