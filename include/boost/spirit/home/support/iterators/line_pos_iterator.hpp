/*==============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2010      Bryce Lelbach
    Copyright (c) 2014      Tomoki Imai
    Copyright (c) 2023      Kniazev Nikita
    Copyright (c) 2023      Tobias Loew

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR)
#define BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR

#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/next_prior.hpp>

namespace boost { namespace spirit
{
    //[line_pos_iterator_class
    /*`The `line_pos_iterator` is a lightweight line position iterator.
       This iterator adapter only stores the current line number, nothing else.
       Unlike __classic__'s `position_iterator`, it does not store the
       column number and does not need an end iterator. The current column can
       be computed, if needed. Please note, due to not haveing an end iterator,
       for CRLF and LFCR style line-endings the line number is increased, when
       the iterator moves from the first of the two line-ending characters.*/
    //`[heading Class Reference]
    template <class Iterator>
    class line_pos_iterator : public boost::iterator_adaptor<
        line_pos_iterator<Iterator>  // Derived
      , Iterator                     // Base
      , boost::use_default           // Value
      , boost::forward_traversal_tag // CategoryOrTraversal
    > {
    public:
        line_pos_iterator();

        explicit line_pos_iterator(Iterator, std::size_t line_start = 1);

        std::size_t position() const;

    private:
        friend class boost::iterator_core_access;

        void increment();

        std::size_t line; // The line position.
        bool prev_n;
        bool prev_r;
    };
    //]

    template <class Iterator>
    line_pos_iterator<Iterator>::line_pos_iterator() :
        line_pos_iterator::iterator_adaptor_(), line(1), prev_n(), prev_r() { }

    template <class Iterator>
    line_pos_iterator<Iterator>::line_pos_iterator(Iterator base, std::size_t line_start /* = 1 */ ) :
        line_pos_iterator::iterator_adaptor_(base), line(line_start), prev_n(), prev_r() { }

    template <class Iterator>
    std::size_t line_pos_iterator<Iterator>::position() const
    {
        return line;
    }
 
    template<class Iterator>
    void line_pos_iterator<Iterator>::increment()
    {
        typename std::iterator_traits<Iterator>::reference
          ch = *(this->base());
      
        int newline = ((ch == '\n') & !prev_r) | ((ch == '\r') & !prev_n);
        line += newline;
        prev_r = (ch == '\r') & newline;
        prev_n = (ch == '\n') & newline;

        ++this->base_reference();
    }

    //[line_pos_iterator_utilities
    //`[heading get_line]
    template <class Iterator>
    inline std::size_t get_line(Iterator);
    /*`Get the line position. Returns -1 if Iterator is not a
       `line_pos_iterator`. */

    //`[heading get_line_start]
    template <class Iterator>
    inline Iterator get_line_start(Iterator lower_bound, Iterator current); 
    /*`Get an iterator to the beginning of the line. Applicable to any
       iterator. */


    //`[heading get_line_end]
    template <class Iterator>
    inline Iterator get_line_end(Iterator current, Iterator upper_bound);
    /*`Get an iterator to the end of the line. Applicable to any
       iterator. */


    //`[heading get_current_line]
    template <class Iterator>
    inline iterator_range<Iterator>
    get_current_line(Iterator lower_bound, Iterator current,
                     Iterator upper_bound); 
    /*`Get an `iterator_range` containing the current line. Applicable to any
       iterator. */ 

    //`[heading get_column]
    template <class Iterator>
    inline std::size_t get_column(Iterator lower_bound, Iterator current,
                                  std::size_t tabs = 4); 
    /*`Get the current column. Applicable to any iterator. */ 
    //]

    template <class Iterator>
    inline std::size_t get_line(Iterator)
    {
        return -1;
    }
    
    template <class Iterator>
    inline std::size_t get_line(line_pos_iterator<Iterator> i)
    {
        return i.position();
    }


    namespace impl {

        // get_line_start for forward iterators: forward linear search starting from lower-bound
        // complexity: linear in length of [lower_bound, current)
        template <class Iterator>
        inline Iterator get_line_start(
                Iterator lower_bound,
                Iterator current, 
                std::forward_iterator_tag
            )
        {
            Iterator latest = lower_bound;
            bool prev_was_newline = false;
            for (Iterator i = lower_bound; i != current; ++i) {
                if (prev_was_newline) {
                    latest = i;
                }
                prev_was_newline = (*i == '\r') || (*i == '\n');
            }
            if (prev_was_newline) {
                latest = current;
            }
            return latest;
        }


        // get_line_start for forward iterators: backard linear search starting from current
        // complexity: linear in avarage line-length
        template <class Iterator>
        inline Iterator get_line_start(
                Iterator lower_bound,
                Iterator current,
                std::bidirectional_iterator_tag
        )
        {
            if (current == lower_bound) {
                return current;
            }
            else {
                for (Iterator i = boost::prior(current); i != lower_bound; --i) {
                    if ((*i == '\r') || (*i == '\n')) {
                        return boost::next(i);
                    }
                }
                if ((*lower_bound == '\r') || (*lower_bound == '\n')) {
                    return boost::next(lower_bound);
                }
                else {
                    return lower_bound;
                }
            }
        }
    }

    template <class Iterator>
    inline Iterator get_line_start(Iterator lower_bound, Iterator current)
    {
		return impl::get_line_start(
			lower_bound,
			current,
            typename boost::iterators::iterator_category<Iterator>::type()
		);
    }

    template <class Iterator>
    line_pos_iterator<Iterator> get_line_start(line_pos_iterator<Iterator> lower_bound,
        line_pos_iterator<Iterator> current)
    {
        // No need in line number counting because it will be the same
        Iterator it = get_line_start(lower_bound.base(), current.base());
        return line_pos_iterator<Iterator>(it, current.position());
    }


    template <class Iterator>
    inline Iterator get_line_end(Iterator current, Iterator upper_bound)
    {
        for (Iterator i = current; i != upper_bound; ++i) {
            if ((*i == '\n') || (*i == '\r')) {
                return i;
            }
        }
        return upper_bound;
    }

    template <class Iterator>
    line_pos_iterator<Iterator> get_line_end(line_pos_iterator<Iterator> current,
        line_pos_iterator<Iterator> upper_bound)
    {
        // No need in line number counting because it will be the same
        Iterator it = get_line_end(current.base(), upper_bound.base());
        return line_pos_iterator<Iterator>(it, current.position());
    }

    
    template <class Iterator>
    inline iterator_range<Iterator>
    get_current_line(Iterator lower_bound,
                     Iterator current,
                     Iterator upper_bound)
    {
		return iterator_range<Iterator>(
            get_line_start(lower_bound, current),
            get_line_end(current, upper_bound)
        );
    }


    template <class Iterator>
    inline iterator_range<line_pos_iterator<Iterator> >
        get_current_line(line_pos_iterator<Iterator> lower_bound, line_pos_iterator<Iterator> current,
            line_pos_iterator<Iterator> upper_bound)
    {
        // No need in line number counting because it will be the same
        iterator_range<Iterator> range = get_current_line(lower_bound.base(), current.base(), upper_bound.base());
        return iterator_range<line_pos_iterator<Iterator> >(
            line_pos_iterator<Iterator>(range.begin(), current.position()),
            line_pos_iterator<Iterator>(range.end(), current.position())
        );
    }

    template <class Iterator>
    inline std::size_t get_column(Iterator lower_bound,
                                  Iterator current,
                                  std::size_t tabs)
    {
        std::size_t column = 1;
        Iterator first = get_line_start(lower_bound, current);
      
        for (Iterator i = first; i != current; ++i) {
          switch (*i) {
            case '\t':
              column += tabs - (column - 1) % tabs;
              break;
            default:
              ++column;
          }
        }
      
        return column;
    }

    template <class Iterator>
    std::size_t get_column(line_pos_iterator<Iterator> lower_bound,
        line_pos_iterator<Iterator> current,
        std::size_t tabs)
    {
        // No need in line number counting because it will be the same
        Iterator it = get_column(lower_bound.base(), current.base(), tabs);
        return line_pos_iterator<Iterator>(it, current.position());
    }

}}

#endif // BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR

