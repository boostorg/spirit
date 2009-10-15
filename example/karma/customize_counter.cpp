/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

//[customize_karma_counter_includes
#include <boost/spirit/include/karma.hpp>
#include <iostream>
#include <string>
#include <vector>
//]

///////////////////////////////////////////////////////////////////////////////
//[customize_karma_counter_data
namespace client
{
    struct counter
    {
        typedef int iterator;
        typedef int type;

        counter(int max_count) 
          : counter_(0), max_count_(max_count)
        {}

        int counter_;
        int max_count_;
    };
}
//]

//[customize_karma_counter_traits
// All specializations of attribute customization points have to be placed into
// the namespace boost::spirit::traits.
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct is_container<client::counter const>
      : mpl::true_
    {};

    template <>
    struct container_iterator<client::counter const>
    {
        typedef client::counter::iterator type;
    };

    template <>
    struct begin_container<client::counter const>
    {
        static client::counter::iterator 
        call(client::counter const& c)
        {
            return c.counter_;
        }
    };

    template <>
    struct end_container<client::counter const>
    {
        static client::counter::iterator 
        call(client::counter const& c)
        {
            return c.max_count_;
        }
    };
}}}
//]

//[customize_karma_counter_iterator_traits
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct deref_iterator<client::counter::iterator>
    {
        typedef client::counter::type type;

        static type call(client::counter::iterator const& it)
        {
            return it;
        }
    };
}}}

///////////////////////////////////////////////////////////////////////////////
namespace karma = boost::spirit::karma;

int main()
{
    //[customize_karma_counter
    // use the instance of a 'client::counter' instead of a STL vector
    client::counter count(4);
    std::cout << karma::format(karma::int_ % ", ", count) << std::endl;   // prints: '0, 1, 2, 3'
    //]
    return 0;
}

