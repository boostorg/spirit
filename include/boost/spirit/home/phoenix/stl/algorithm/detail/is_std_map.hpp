///////////////////////////////////////////////////////////////////////////////
//
// is_std_map.hpp
//
/////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1000
# pragma once
#endif

#ifndef IS_STD_MAP_EN_16_12_2004
#define IS_STD_MAP_EN_16_12_2004

#include <boost/mpl/bool.hpp>
#include "./std_map_fwd.hpp"

namespace boost
{
    template<class T>
    struct is_std_map
        : boost::mpl::false_
    {};

    template<
        class Kty
      , class Ty
      , class Pr
      , class Alloc
    >
    struct is_std_map< ::std::map<Kty,Ty,Pr,Alloc> >
        : boost::mpl::true_
    {};

    template<class T>
    struct is_std_multimap
        : boost::mpl::false_
    {};

    template<
        class Kty
      , class Ty
      , class Pr
      , class Alloc
    >
    struct is_std_multimap< ::std::multimap<Kty,Ty,Pr,Alloc> >
        : boost::mpl::true_
    {};
}

#endif
