/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_DETAIL_CONFIG_HPP)
#define FUSION_DETAIL_CONFIG_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/preprocessor/cat.hpp>

///////////////////////////////////////////////////////////////////////////////
//
//  Compiler check:
//
//  Yes, we try to code in pure ANSI/ISO C++. However, in reality, we really
//  can't --we always struggle with compiler quirks. We regularly have to
//  deal with compiler and platform specific code. The problem is that we
//  can't fix all problems for our code to work with all compilers out there.
//  This gives us library implementers a bad reputation when someone attempts
//  to compile the code on some compiler we haven't tested with. Chances are,
//  her compiler is not fully C++ conforming and she'll be confronted with
//  tons of compiler errors that will somehow make less informed users to
//  conclude that the code is poorly written.
//
//  We'll still code with pure ANSI/ISO C++, plus some workarounds here and
//  there for known deficient compilers. However, to be on the safe side, we
//  shall disallow the use of this library for compilers we are not aware of
//  or we haven't tested with yet. It's better for the user to see a message
//  "sorry, this code has not been ported to your compiler yet", than to see
//  pages and pages of compiler eror messages.
//
//  If you got here somehow, it does not really mean that the code will not
//  work on your compiler. Please contact the author(s) for updates. Probably
//  a port is already available. If not, then you might want to request a port
//  to your compiler. Unless your compiler is truly broken, chances are, the
//  code here can be made to work with your compiler.
//
///////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__) && (__BORLANDC__ != 0x551)
# error "Compiler not supported yet. See note above."

#elif defined(BOOST_MSVC) && ((BOOST_MSVC < 1200) || (BOOST_MSVC > 1310))
# error "Compiler not supported yet. See note above."

#elif defined(__GNUC__) &&                                                      \
    (__GNUC__ < 2) || (__GNUC__ > 3) ||                                         \
    (__GNUC__ == 2 && __GNUC_MINOR__ < 95) ||                                   \
    (__GNUC__ == 3 && __GNUC_MINOR__ > 2)
# error "Compiler not supported yet. See note above."

#elif defined(__EDG_VERSION__) && (__EDG_VERSION__ < 244)
# error "Compiler not supported yet. See note above."

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Before including MPL, we define these dummy template functions. Borland
//  complains when a template class has the same name as a template function,
//  regardless if they are in different namespaces. This is a workaround to
//  this Borland quirk.
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
namespace boost { namespace fusion { namespace borland_only {

    template <typename T> void begin(T) {}
    template <typename T> void end(T) {}
    template <typename T> void next(T) {}
    template <typename T> void prior(T) {}
    template <typename T> void find(T) {}
    template <typename T> void find_if(T) {}

}}}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  MSVC, even with VC7.1 has problems with returning a default constructed
//  value of a given type: return type(); This only happens on debug builds.
//  It seems to be a return value optimization bug.
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1301) && !defined(NDEBUG)
# define FUSION_RETURN_DEFAULT_CONSTRUCTED type r; return r
#else
# define FUSION_RETURN_DEFAULT_CONSTRUCTED return type()
#endif

namespace boost { namespace fusion { namespace detail
{
    template <typename T>
    struct get_value
    {
        BOOST_STATIC_CONSTANT(int, value = T::value);
    };

#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)

    // VC6 ETI (early template instantiation) bug workaround.

    template <>
    struct get_value<int>
    {
        BOOST_STATIC_CONSTANT(int, value = 0);
    };
#endif

}}}

///////////////////////////////////////////////////////////////////////////////
//
//  Borland does not like the T::value syntax. Instead, we use a metafunction
//  get_value<T>::value. The explicit qualification (::boost::fusion::detail::)
//  also makes Borland happy.
//
//  VC6 on the other hand chokes with ETI (early instantiation bug). So we
//  forward the call to get_value<T>::value and fix the ETI bug there (see
//  get_value above).
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)                                    \
    || BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
# define FUSION_GET_VALUE(T) ::boost::fusion::detail::get_value<T>::value
#else
# define FUSION_GET_VALUE(T) T::value
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Borland does not like returning a const reference from a tuple member.
//  We do the cast explicitly.
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
# define FUSION_RETURN_TUPLE_MEMBER(n)                                          \
    typedef typename tuple_access_result<n, Tuple>::type type;                  \
    return type(t.BOOST_PP_CAT(m, n))
#else
# define FUSION_RETURN_TUPLE_MEMBER(n)                                          \
    return t.BOOST_PP_CAT(m, n)
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  See get.hpp. In function get<N>(t), mpl::int_<N>* = 0 is a function
//  parameter that defaults to 0. This is a hack to make VC6 happy, otherwise,
//  VC6 will return the wrong result from a wrong index!
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
# define FUSION_GET_MSVC_WORKAROUND , mpl::int_<N>* = 0
#else
# define FUSION_GET_MSVC_WORKAROUND
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  FUSION_MSVC_ETI_WRAPPER (VC6 only)
//
//  VC6 chokes with ETI (early instantiation bug) with typename T::name. So,
//  we forward the call to get_name<T>::type and fix the ETI bug by providing
//  a specialization for <int>.
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
#define FUSION_MSVC_ETI_WRAPPER(name)                                           \
namespace boost { namespace fusion { namespace detail                           \
{                                                                               \
    template <typename T>                                                       \
    struct BOOST_PP_CAT(get_, name)                                             \
    {                                                                           \
        typedef typename T::name type;                                          \
    };                                                                          \
                                                                                \
    /* VC6 ETI (early template instantiation) bug workaround. */                \
    template <>                                                                 \
    struct BOOST_PP_CAT(get_, name)<int>                                        \
    {                                                                           \
        typedef int type;                                                       \
    };                                                                          \
}}}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::tag wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(tag)
# define FUSION_GET_TAG(T) ::boost::fusion::detail::get_tag<T>::type
#else
# define FUSION_GET_TAG(T) typename T::tag
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::type wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(type)
# define FUSION_GET_TYPE(T) ::boost::fusion::detail::get_type<T>::type
#else
# define FUSION_GET_TYPE(T) typename T::type
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::types wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(types)
# define FUSION_GET_TYPES(T) ::boost::fusion::detail::get_types<T>::type
#else
# define FUSION_GET_TYPES(T) typename T::types
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::index wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(index)
# define FUSION_GET_INDEX(T) ::boost::fusion::detail::get_index<T>::type
#else
# define FUSION_GET_INDEX(T) typename T::index
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::tuple wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(tuple)
# define FUSION_GET_TUPLE(T) ::boost::fusion::detail::get_tuple<T>::type
#else
# define FUSION_GET_TUPLE(T) typename T::tuple
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::size wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(size)
# define FUSION_GET_SIZE(T) ::boost::fusion::detail::get_size<T>::type
#else
# define FUSION_GET_SIZE(T) typename T::size
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  T::value_type wrapper
//
///////////////////////////////////////////////////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(value_type)
# define FUSION_GET_VALUE_TYPE(T) ::boost::fusion::detail::get_value_type<T>::type
#else
# define FUSION_GET_VALUE_TYPE(T) typename T::value_type
#endif

///////////////////////////////////////////////////////////////////////////////
//
//   Borland is so flaky with const correctness of iterators. It's getting
//   confused with tuple_iterator<N, T> where T is a const tuple. We cast
//   what Borland thinks is a const reference to a true reference.
//
///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace fusion { namespace detail
{
#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)

    template <typename T>
    T& ref(T const& r)
    {
        return const_cast<T&>(r);
    }

#else

    template <typename T>
    T& ref(T& r)
    {
        return r;
    }

#endif

}}}

#endif


