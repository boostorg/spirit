/*=============================================================================
    Copyright (c) 2006 Tobias Schwinger
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_POSITION_ITERATOR_FWD_HPP)
#define BOOST_SPIRIT_POSITION_ITERATOR_FWD_HPP

#include <boost/spirit/core/nil.hpp>

namespace boost { namespace spirit {

    struct file_position;
    struct file_position_without_column;

    template <
        typename ForwardIteratorT,
        typename PositionT = file_position,
        typename SelfT = nil_t
    >
    class position_iterator;

    template
    <
        typename ForwardIteratorT,
        typename PositionT = file_position
    >
    class position_iterator2;

    template <typename PositionT> class position_policy;

}} // namespace boost::spirit

#endif

