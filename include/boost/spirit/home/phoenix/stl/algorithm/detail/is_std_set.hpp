///////////////////////////////////////////////////////////////////////////////
//
// is_std_set.hpp
//
/////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1000
# pragma once
#endif

#ifndef IS_STD_SET_EN_16_12_2004
#define IS_STD_SET_EN_16_12_2004

#include <boost/mpl/bool.hpp>
#include "./std_set_fwd.hpp"

namespace boost
{
    template<class T>
    struct is_std_set
        : boost::mpl::false_
    {};

    template<
        class Kty
      , class Pr
      , class Alloc
    >
    struct is_std_set< ::std::set<Kty,Pr,Alloc> >
        : boost::mpl::true_
    {};

    template<class T>
    struct is_std_multiset
        : boost::mpl::false_
    {};

    template<
        class Kty
      , class Pr
      , class Alloc
    >
    struct is_std_multiset< ::std::multiset<Kty,Pr,Alloc> >
        : boost::mpl::true_
    {};
}

#endif
