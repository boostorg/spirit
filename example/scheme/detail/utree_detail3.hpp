/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE_DETAIL3)
#define BOOST_SPIRIT_UTREE_DETAIL3

#include <boost/variant/get.hpp>    // boost::bad_get

namespace scheme 
{ 
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        struct get_utree_type;

#define SCHEME_GET_UTREE_TYPE(t, v)                                           \
    template <> struct get_utree_type<t> { enum { value = v }; }              \
    /**/

        SCHEME_GET_UTREE_TYPE(nil, utree_type::nil_type);
        SCHEME_GET_UTREE_TYPE(bool, utree_type::bool_type);
        SCHEME_GET_UTREE_TYPE(int, utree_type::int_type);
        SCHEME_GET_UTREE_TYPE(double, utree_type::double_type);
        SCHEME_GET_UTREE_TYPE(utf8_string_range, utree_type::string_type);
        SCHEME_GET_UTREE_TYPE(utf8_symbol_range, utree_type::symbol_type);
        SCHEME_GET_UTREE_TYPE(binary_range, utree_type::binary_type);
        SCHEME_GET_UTREE_TYPE(boost::iterator_range<utree::iterator>, 
            utree_type::list_type);
        SCHEME_GET_UTREE_TYPE(boost::iterator_range<utree::const_iterator>, 
            utree_type::list_type);
        SCHEME_GET_UTREE_TYPE(function_ptr, utree_type::function_type);

#undef SCHEME_GET_UTREE_TYPE

        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        struct get_impl;

        template <>
        struct get_impl<nil>
        {
            typedef nil type;
            static type call(utree const&) { return nil(); }
        };

        template <>
        struct get_impl<bool>
        {
            typedef bool type;
            static type call(utree const& x) { return x.b; }
        };

        template <>
        struct get_impl<int>
        {
            typedef int type;
            static type call(utree const& x) { return x.i; }
        };

        template <>
        struct get_impl<double>
        {
            typedef double type;
            static type call(utree const& x) { return x.d; }
        };

        template <>
        struct get_impl<boost::iterator_range<utree::iterator> >
        {
            typedef boost::iterator_range<utree::iterator> type;
            static type call(utree const& x) 
            { 
                return type(utree::iterator(x.l.first)
                  , utree::iterator(0, x.l.last)); 
            }
        };

        template <>
        struct get_impl<boost::iterator_range<utree::const_iterator> >
        {
            typedef boost::iterator_range<utree::const_iterator> type;
            static type call(utree const& x) 
            { 
                return type(utree::const_iterator(x.l.first)
                    , utree::const_iterator(0, x.l.last)); 
            }
        };

        template <>
        struct get_impl<utf8_string_range>
        {
            typedef utf8_string_range type;
            static type call(utree const& x) 
            { 
                return type(x.s.str(), x.s.size()); 
            }
        };

        template <>
        struct get_impl<utf8_symbol_range>
        {
            typedef utf8_symbol_range type;
            static type call(utree const& x) 
            { 
                return type(x.s.str(), x.s.size()); 
            }
        };

        template <>
        struct get_impl<binary_range>
        {
            typedef binary_range type;
            static type call(utree const& x) 
            { 
                return type(x.s.str(), x.s.size()); 
            }
        };

        template <>
        struct get_impl<function_ptr>
        {
            typedef function_ptr type;
            static type call(utree const& x) 
            { 
                return type(x.f); 
            }
        };
    }

    template <typename T>
    typename scheme::detail::get_impl<T>::type 
    get(scheme::utree const& x)
    {
        if (x.which() != scheme::detail::get_utree_type<T>::value)
            throw boost::bad_get();

        return scheme::detail::get_impl<T>::call(x);
    }
}

#endif
