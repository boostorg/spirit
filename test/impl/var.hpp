/*=============================================================================
  Spirit V1.6.1
  Copyright (c) 2003 Martin Wille
  http://spirit.sourceforge.net/

  Permission to copy, use, modify, sell and distribute this software
  is granted provided this copyright notice appears in all copies.
  This software is provided "as is" without express or implied
  warranty, and with no claim as to its suitability for any purpose.
 =============================================================================*/
#ifndef BOOST_SPIRIT_TEST_IMPL_VAR_HPP
#define BOOST_SPIRIT_TEST_IMPL_VAR_HPP

#include <boost/ref.hpp>

namespace test
{
    template <typename T>
    struct var_wrapper
        : public ::boost::reference_wrapper<T>
    {
        typedef ::boost::reference_wrapper<T> parent;

        explicit inline var_wrapper(T& t) : parent(t) {}

        inline T& operator()() const { return parent::get(); }
    };

    template <typename T>
    inline var_wrapper<T>
    var(T& t)
    {
        return var_wrapper<T>(t);
    }
}
#endif // BOOST_SPIRIT_TEST_IMPL_VAR_HPP
