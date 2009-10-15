/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

//[customize_karma_use_as_container_includes
#include <boost/spirit/include/karma.hpp>
#include <iostream>
#include <string>
#include <vector>
//]

///////////////////////////////////////////////////////////////////////////////
//[customize_karma_use_as_container_data
// All specializations of attribute customization points have to be placed into
// the namespace boost::spirit::traits.
namespace client
{
    struct use_as_container
    {
        struct iterator
        {
            iterator(use_as_container const* container, int const* current)
              : container_(container), current_(current)
            {}

            use_as_container const* container_;
            int const* current_;
        };
        typedef int type;

        use_as_container() 
          : value1_(1), value2_(2), value3_(3)
        {}

        int value1_;
        std::string dummy1_;    // insert some unrelated data
        int value2_;
        std::string dummy2_;    // insert some more unrelated data
        int value3_;
    };
}
//]

//[customize_karma_use_as_container_traits
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct is_container<client::use_as_container const>
      : mpl::true_
    {};

    template <>
    struct container_iterator<client::use_as_container const>
    {
        typedef client::use_as_container::iterator type;
    };

    template <>
    struct begin_container<client::use_as_container const>
    {
        static client::use_as_container::iterator 
        call(client::use_as_container const& c)
        {
            return client::use_as_container::iterator(&c, &c.value1_);
        }
    };

    template <>
    struct end_container<client::use_as_container const>
    {
        static client::use_as_container::iterator 
        call(client::use_as_container const& c)
        {
            return client::use_as_container::iterator(&c, (int const*)0);
        }
    };
}}}
//]

//[customize_karma_use_as_container_iterator_traits
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct deref_iterator<client::use_as_container::iterator>
    {
        typedef client::use_as_container::type type;

        static type call(client::use_as_container::iterator const& it)
        {
            return *it.current_;
        }
    };

    template <>
    struct next_iterator<client::use_as_container::iterator>
    {
        static void call(client::use_as_container::iterator& it)
        {
            if (it.current_ == &it.container_->value1_)
                it.current_ = &it.container_->value2_;
            else if (it.current_ == &it.container_->value2_)
                it.current_ = &it.container_->value3_;
            else
                it.current_ = 0;
        }
    };

    template <>
    struct compare_iterators<client::use_as_container::iterator>
    {
        static bool call(client::use_as_container::iterator const& it1
          , client::use_as_container::iterator const& it2)
        {
            return it1.current_ == it2.current_ && 
                   it1.container_ == it2.container_;
        }
    };
}}}
//]

///////////////////////////////////////////////////////////////////////////////
namespace karma = boost::spirit::karma;

int main()
{
    //[customize_karma_use_as_container
    client::use_as_container d2;
    // use the instance of a 'client::use_as_container' instead of a STL vector
    std::cout << karma::format(karma::int_ % ", ", d2) << std::endl;   // prints: '1, 2, 3'
    //]
    return 0;
}

