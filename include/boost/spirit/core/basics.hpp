/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2001-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_BASICS_HPP)
#define BOOST_SPIRIT_BASICS_HPP

#include <boost/config.hpp>
#include <boost/spirit/core/impl/basics.ipp>

#if defined(BOOST_MSVC) || defined(BOOST_INTEL_CXX_VERSION)
#include "boost/spirit/core/impl/msvc.hpp"
#endif

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)
#define BOOST_SPIRIT_TYPENAME
#else
#define BOOST_SPIRIT_TYPENAME typename
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

    struct nil_t {};
    struct void_t {};
    struct default_t {};

    template <int N>
    struct int_t { BOOST_STATIC_CONSTANT(int, value = N); };

    template <bool C>
    struct bool_t { BOOST_STATIC_CONSTANT(bool, value = C); };
    typedef bool_t<true> true_t;
    typedef bool_t<false> false_t;

    ///////////////////////////////////////////////////////////////////////////
    //
    //  if_t selects type A or B depending on the condition C.
    //  If C is of type true_t, A is selected, otherwise B
    //
    ///////////////////////////////////////////////////////////////////////////
    #if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

    template <typename C, typename A, typename B>
    struct if_t
    {
        typedef typename impl::chooser<C>::type chooser_t;
        typedef typename chooser_t::template result<A, B>::type type;
    };

    #else
    template <typename C, typename A, typename B>
    struct if_t { typedef A type; };

    template <typename A, typename B>
    struct if_t<false_t, A, B> { typedef B type; };

    #endif

}} // namespace boost::spirit

#endif

