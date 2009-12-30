//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_AUTO_NOV_29_2009_0339PM)
#define BOOST_SPIRIT_KARMA_AUTO_NOV_29_2009_0339PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/assert_msg.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/auto/create_generator.hpp>
#include <boost/mpl/bool.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<karma::domain, tag::auto_>     // enables auto_
      : mpl::true_ {};
}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    using spirit::auto_;

    ///////////////////////////////////////////////////////////////////////////
    template <typename Modifiers>
    struct auto_generator
      : generator<auto_generator<Modifiers> >
    {
        typedef mpl::int_<generator_properties::all_properties> properties;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef spirit::hold_any type;
        };

        auto_generator(Modifiers const& modifiers)
          : modifiers_(modifiers) {}

        // auto_generator has an attached attribute 
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& context
          , Delimiter const& d, Attribute const& attr) const
        {
            return compile<karma::domain>(create_generator<Attribute>(), modifiers_)
                      .generate(sink, context, d, attr);
        }

        // this auto_generator has no attribute attached, it needs to have been
        // initialized from a value/variable
        template <typename OutputIterator, typename Context
          , typename Delimiter>
        static bool
        generate(OutputIterator&, Context&, Delimiter const&, unused_type)
        {
            // It is not possible (doesn't make sense) to use auto_ generators 
            // without providing any attribute, as the generator doesn't 'know' 
            // what to output. The following assertion fires if this situation
            // is detected in your code.
            BOOST_SPIRIT_ASSERT_MSG(false, auto_not_usable_without_attribute, ());
            return false;
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info("auto_");
        }

        Modifiers modifiers_;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Modifiers>
    struct make_primitive<tag::auto_, Modifiers> 
    {
        typedef auto_generator<Modifiers> result_type;

        result_type operator()(unused_type, Modifiers const& modifiers) const
        {
            return result_type(modifiers);
        }
    };
}}}

#endif
