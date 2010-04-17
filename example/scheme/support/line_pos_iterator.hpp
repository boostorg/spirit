/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_LINE_POS_ITERATOR)
#define BOOST_SPIRIT_LINE_POS_ITERATOR

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/iterator_range.hpp>

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // line_pos_iterator: a lighweight line position iterator. This iterator
    // adapter only stores the current line number, nothing else. Unlike
    // spirit classic's position_iterator, it does not store the column
    // number and does not need an end iterator. The current column can be
    // computed, if needed. Some utilities line oriented are provided
    // including computation of the current column.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    class line_pos_iterator
      : public boost::iterator_adaptor<
            line_pos_iterator<Iterator>     // Derived
          , Iterator                        // Base
          , boost::use_default              // Value
          , boost::forward_traversal_tag    // CategoryOrTraversal
        >
    {
    public:

        line_pos_iterator()
          : line_pos_iterator::iterator_adaptor_()
            line(0), prev(0) {}

        explicit line_pos_iterator(Iterator base)
          : node_iter::iterator_adaptor_(base)
            line(0), prev(0) {}

        std::size_t position() const
        {
            return line;
        }

    private:

        friend class boost::iterator_core_access;
        void increment()
        {
            typename Iterator::reference ref = *(this->base());
            switch (ref)
            {
                case '\r':
                    if (prev != '\n')
                       ++line;
                    break;
                case '\n':
                    if (prev != '\r')
                       ++line;
                    break;
                default:
                    break;
            }
            prev = ref;
        }

        std::size_t line;
        typename Iterator::value_type prev;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Utilities
    ///////////////////////////////////////////////////////////////////////////

    // Get an iterator to the beginning of the line. Applicable to any
    // iterator.
    template <typename Iterator>
    inline Iterator
    get_line_start(Iterator lower_bound, Iterator current)
    {
        Iterator latest = lower_bound;
        for (Iterator i = lower_bound; i != current; ++i)
        {
            switch (*i)
            {
                case '\r':
                case '\n':
                    latest = i;
            }
        }
        return latest;
    }

    // Get the iterator range containing the current line. Applicable to
    // any iterator.
    template <typename Iterator>
    inline iterator_range<Iterator>
    get_current_line(
        Iterator lower_bound, Iterator current, Iterator upper_bound)
    {
        Iterator first = get_line_start(lower_bound, current);
        Iterator last = get_line_start(current, upper_bound);
        if (last == current)
            last = upper_bound;
        return iterator_range<Iterator>(first, last);
    }

    // Get the current column. Applicable to any iterator.
    template <typename Iterator>
    inline std::size_t
    get_column(
        Iterator lower_bound, Iterator current, int tabs = 4)
    {
        std::size_t column = 1;
        Iterator first = get_line_start(lower_bound, current);
        for (Iterator i = first; i != current; ++i)
        {
            switch (*i)
            {
                case '\t':
                    column += tabs - (column - 1) % tabs;
                    break;
                default:
                    ++column;
            }
        }
        return column;
    }
}

#endif
