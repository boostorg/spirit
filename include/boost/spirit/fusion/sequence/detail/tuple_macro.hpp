/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_MACRO_HPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_MACRO_HPP

///////////////////////////////////////////////////////////////////////////////
//
//  Pre-processor gunk. See tuple10.hpp and detail/tuple10.hpp. The
//  following code is the preprocessor version of the code found in
//  those files, plus/minus a few specific details (specifically,
//  the tuple_access<N> and tupleN+1 classes).
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/enum.hpp>

#define FUSION_TUPLE_N_ACCESS(z, n, _)                                          \
                                                                                \
    template <>                                                                 \
    struct tuple_access<n>                                                      \
    {                                                                           \
        template <typename Tuple>                                               \
        static typename tuple_access_result<n, Tuple>::type                     \
        get(Tuple& t)                                                           \
        {                                                                       \
            FUSION_RETURN_TUPLE_MEMBER(n);                                      \
        }                                                                       \
    };

#define FUSION_TUPLE_MEMBER_DEFAULT_INIT(z, n, _)                               \
    BOOST_PP_CAT(m, n)(BOOST_PP_CAT(T, n)())

#define FUSION_TUPLE_MEMBER_INIT(z, n, _)                                       \
    BOOST_PP_CAT(m, n)(BOOST_PP_CAT(_, n))

#define FUSION_TUPLE_MEMBER(z, n, _)                                            \
    BOOST_PP_CAT(T, n) BOOST_PP_CAT(m, n);

#define FUSION_TUPLE_MEMBER_ASSIGN(z, n, _)                                     \
    this->BOOST_PP_CAT(m, n) = t.BOOST_PP_CAT(m, n);

#define FUSION_TUPLE_RESULT_OF_NEXT_TYPE(z, n, _)                               \
    typedef typename result_of_next<                                            \
        BOOST_PP_CAT(BOOST_PP_CAT(i, n), _type)>::type                          \
        BOOST_PP_CAT(BOOST_PP_CAT(i, BOOST_PP_INC(n)), _type);

#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
# define FUSION_TUPLE_RESULT_OF_NEXT(z, n, _)                                   \
    next_iter::BOOST_PP_CAT(BOOST_PP_CAT(i, BOOST_PP_INC(n)), _type)            \
        BOOST_PP_CAT(i, BOOST_PP_INC(n))(fusion::next(BOOST_PP_CAT(i, n)));
#else
# define FUSION_TUPLE_RESULT_OF_NEXT(z, n, _)                                   \
    FUSION_TUPLE_RESULT_OF_NEXT_TYPE(z, n, _)                                   \
        BOOST_PP_CAT(BOOST_PP_CAT(i, BOOST_PP_INC(n)), _type)                   \
            BOOST_PP_CAT(i, BOOST_PP_INC(n))(fusion::next(BOOST_PP_CAT(i, n)));
#endif

#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
# define FUSION_TUPLE_NEXT_ITER_N(z, n, _)                                      \
    namespace detail                                                            \
    {                                                                           \
        template <typename i0_type>                                             \
        struct BOOST_PP_CAT(next_iter, n)                                       \
        {                                                                       \
            BOOST_PP_REPEAT(                                                    \
                BOOST_PP_DEC(n), FUSION_TUPLE_RESULT_OF_NEXT_TYPE, _)           \
        };                                                                      \
    }

#else
# define FUSION_TUPLE_NEXT_ITER_N(z, n, _)
#endif

#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
# define FUSION_TUPLE_CONSTRUCT_FROM_ITER(n)                                    \
    typedef detail::BOOST_PP_CAT(next_iter, n)<i0_type> next_iter;              \
    BOOST_PP_REPEAT(BOOST_PP_DEC(n), FUSION_TUPLE_RESULT_OF_NEXT, _)
#else
# define FUSION_TUPLE_CONSTRUCT_FROM_ITER(n)                                    \
    BOOST_PP_REPEAT(BOOST_PP_DEC(n), FUSION_TUPLE_RESULT_OF_NEXT, _)
#endif

#define FUSION_TUPLE_N(z, n, _)                                                 \
                                                                                \
    FUSION_TUPLE_NEXT_ITER_N(z, n, _)                                           \
                                                                                \
    template <BOOST_PP_ENUM_PARAMS(n, typename T)>                              \
    struct BOOST_PP_CAT(tuple_data, n)                                          \
        : sequence_base<BOOST_PP_CAT(tuple_data, n)                             \
            <BOOST_PP_ENUM_PARAMS(n, T)> >                                      \
    {                                                                           \
        typedef mpl::BOOST_PP_CAT(vector, n)<BOOST_PP_ENUM_PARAMS(n, T)> types; \
        typedef tuple_tag tag;                                                  \
        typedef mpl::int_<n> size;                                              \
                                                                                \
        BOOST_PP_CAT(tuple_data, n)()                                           \
            : BOOST_PP_ENUM(n, FUSION_TUPLE_MEMBER_DEFAULT_INIT, _) {}          \
                                                                                \
        BOOST_PP_CAT(tuple_data, n)(BOOST_PP_ENUM_BINARY_PARAMS(                \
            n, typename detail::call_param<T, >::type _))                       \
            : BOOST_PP_ENUM(n, FUSION_TUPLE_MEMBER_INIT, _) {}                  \
                                                                                \
        BOOST_PP_REPEAT(n, FUSION_TUPLE_MEMBER, _)                              \
    };                                                                          \
                                                                                \
    template <BOOST_PP_ENUM_PARAMS(n, typename T)>                              \
    struct BOOST_PP_CAT(tuple, n)                                               \
        : BOOST_PP_CAT(tuple_data, n)<BOOST_PP_ENUM_PARAMS(n, T)>               \
    {                                                                           \
        typedef BOOST_PP_CAT(tuple_data, n)<                                    \
            BOOST_PP_ENUM_PARAMS(n, T)> base_type;                              \
                                                                                \
        BOOST_PP_CAT(tuple, n)()                                                \
            : base_type()                                                       \
        {}                                                                      \
                                                                                \
        BOOST_PP_CAT(tuple, n)(BOOST_PP_ENUM_BINARY_PARAMS(                     \
            n, typename detail::call_param<T, >::type _))                       \
            : base_type(BOOST_PP_ENUM_PARAMS(n, _))                             \
        {}                                                                      \
                                                                                \
        template <typename X>                                                   \
        explicit BOOST_PP_CAT(tuple, n)(X const& x)                             \
            : base_type(construct(x, &x))                                       \
        {}                                                                      \
                                                                                \
        template <BOOST_PP_ENUM_PARAMS(n, typename U)>                          \
        BOOST_PP_CAT(tuple, n)&                                                 \
        operator=(BOOST_PP_CAT(tuple, n)<BOOST_PP_ENUM_PARAMS(n, U)> const& t)  \
        {                                                                       \
            BOOST_PP_REPEAT(n, FUSION_TUPLE_MEMBER_ASSIGN, _)                   \
            return *this;                                                       \
        }                                                                       \
                                                                                \
    private:                                                                    \
                                                                                \
        template <typename i0_type>                                             \
        static base_type                                                        \
        construct(i0_type const& i0, void const*)                               \
        {                                                                       \
            FUSION_TUPLE_CONSTRUCT_FROM_ITER(n)                                 \
            return base_type(BOOST_PP_ENUM_PARAMS(n, *i));                      \
        }                                                                       \
                                                                                \
        template <typename Tuple>                                               \
        static base_type                                                        \
        construct(Tuple const& t, sequence_root const*)                         \
        {                                                                       \
            return base_type(BOOST_PP_ENUM_PARAMS(n, t.m));                     \
        }                                                                       \
    };

#endif
