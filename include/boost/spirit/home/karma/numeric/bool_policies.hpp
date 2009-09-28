//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_BOOL_POLICIES_SEP_28_2009_1203PM)
#define BOOST_SPIRIT_KARMA_BOOL_POLICIES_SEP_28_2009_1203PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/karma/generate.hpp>
#include <boost/spirit/home/karma/char.hpp>

namespace boost { namespace spirit { namespace karma 
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  bool_policies, if you need special handling of your boolean output
    //  just overload this policy class and use it as a template
    //  parameter to the karma::bool_generator boolean generator
    //
    //      struct special_bool_policy : karma::bool_policies<>
    //      {
    //          template <typename CharEncoding, typename Tag
    //            , typename OutputIterator>
    //          static bool generate (OutputIterator& sink, bool b)
    //          {
    //              //  we want to spell the names of true and false backwards
    //              return string_inserter<CharEncoding, Tag>::call(
    //                  sink, b ? "eurt" : "aslaf");
    //          }
    //      };
    //
    //      typedef karma::bool_generator<special_bool_policy> backwards_bool;
    //
    //      karma::generate(sink, backwards_bool(), true); // will output: eurt
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = bool>
    struct bool_policies
    {
        ///////////////////////////////////////////////////////////////////////
        // Expose the data type the generator is targeted at
        ///////////////////////////////////////////////////////////////////////
        typedef T value_type;

        ///////////////////////////////////////////////////////////////////////
        //  By default the policy doesn't require any special iterator 
        //  functionality. The boolean generator exposes its properties
        //  from here, so this needs to be updated in case other properties
        //  need to be implemented.
        ///////////////////////////////////////////////////////////////////////
        typedef mpl::int_<generator_properties::no_properties> properties;

        ///////////////////////////////////////////////////////////////////////
        //  Print the textual representations of the given boolean value
        //
        //      sink       The output iterator to use for generation
        //      b          The boolean value to convert. 
        //
        //  The CharEncoding and Tag template parameters are either of the type 
        //  unused_type or describes the character class and conversion to be 
        //  applied to any output possibly influenced by either the lower[...] 
        //  or upper[...] directives.
        //
        ///////////////////////////////////////////////////////////////////////
        template <typename CharEncoding, typename Tag, typename OutputIterator>
        static bool generate (OutputIterator& sink, T b)
        {
            return string_inserter<CharEncoding, Tag>::call(
                sink, b ? "true" : "false");
        }
    };

}}}

#endif
