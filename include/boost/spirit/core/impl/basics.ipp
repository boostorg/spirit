/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 2001-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined BOOST_SPIRIT_BASICS_IPP
#define BOOST_SPIRIT_BASICS_IPP

///////////////////////////////////////////////////////////////////////////////
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
namespace boost { namespace spirit {

//////////////////////////////////
template <bool C>
struct bool_t;
typedef bool_t<false> false_t;

    namespace impl
    {
        //////////////////////////////////
        struct chooser0
        {
            template <typename A, typename B>
            struct result { typedef A type; };
        };

        //////////////////////////////////
        struct chooser1
        {
            template <typename A, typename B>
            struct result { typedef B type; };
        };

        //////////////////////////////////
        template <typename C> struct chooser
        { typedef chooser0 type; };

        template <> struct chooser<false_t>
        { typedef chooser1 type; };
    }

}} // namespace boost::spirit

#endif
#endif

