/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM)
#define BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace x3
{
    struct no_case_tag {};

    struct case_compare
    {
        template <typename Encoding>
        bool equal(typename Encoding::char_type const lc, typename Encoding::char_type const rc) const
        {
            return lc == rc;
        }
    };

    struct no_case_compare
    {
        template <typename Encoding>
        bool equal(typename Encoding::char_type const lc, typename Encoding::char_type const rc) const
        {
            return lc == rc || ((Encoding::islower(lc) ? Encoding::toupper(lc) : Encoding::tolower(lc)) == rc);
        }
    };

    case_compare const case_compare_ = case_compare();
    no_case_compare const no_case_compare_ = no_case_compare();

}}}

#endif
