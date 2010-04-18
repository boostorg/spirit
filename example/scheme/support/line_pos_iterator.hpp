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
    // computed, if needed. Some line oriented utilities are provided
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
          : line_pos_iterator::iterator_adaptor_(),
            line(1), prev(0) {}

        explicit line_pos_iterator(Iterator base)
          : line_pos_iterator::iterator_adaptor_(base),
            line(1), prev(0) {}

        std::size_t position() const
        {
            return line;
        }

    private:

        friend class boost::iterator_core_access;
        void increment()
        {
            typename std::iterator_traits<Iterator>::reference
                ref = *(this->base());
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
            ++this->base_reference();
        }

        std::size_t line;
        typename std::iterator_traits<Iterator>::value_type prev;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Utilities
    ///////////////////////////////////////////////////////////////////////////

    // Get the line position. Returns -1 if Iterator is not a line_pos_iterator.
    template <typename Iterator>
    inline int get_line(Iterator i)
    {
        return -1;
    }

    template <typename Iterator>
    inline int get_line(line_pos_iterator<Iterator> i)
    {
        return i.position();
    }

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
    inline boost::iterator_range<Iterator>
    get_current_line(
        Iterator lower_bound, Iterator current, Iterator upper_bound)
    {
        Iterator first = get_line_start(lower_bound, current);
        Iterator last = get_line_start(current, upper_bound);
        if (last == current)
            last = upper_bound;
        return boost::iterator_range<Iterator>(first, last);
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
