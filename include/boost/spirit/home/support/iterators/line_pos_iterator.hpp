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
       be computed, if needed. */
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

        explicit line_pos_iterator(Iterator);

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
    line_pos_iterator<Iterator>::line_pos_iterator(Iterator base) :
        line_pos_iterator::iterator_adaptor_(base), line(1), prev_n(), prev_r() { }

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


    template<typename Iterator>
    struct line_pos_base_iterator_type {
        typedef Iterator type;
    };

    template<typename Iterator>
    struct line_pos_base_iterator_type< line_pos_iterator<Iterator> > {
        typedef Iterator type;
    };



    //[line_pos_iterator_utilities
    //`[heading get_line]
    template <class Iterator>
    inline std::size_t get_line(Iterator);

    template <class Iterator>
    inline std::size_t get_line(line_pos_iterator<Iterator>);
    /*`Get the line position. Returns -1 if Iterator is not a
       `line_pos_iterator`. */

    //`[heading get_line_start]
    template <class Iterator, class IteratorBound>
    inline typename line_pos_base_iterator_type<Iterator>::type get_line_start(IteratorBound lower_bound, Iterator current);
    /*`Get an iterator to the beginning of the line. Applicable to any
       iterator. */

    //`[heading get_current_line]
    template <class Iterator, class IteratorBound>
    inline iterator_range<typename line_pos_base_iterator_type<Iterator>::type>
    get_current_line(IteratorBound lower_bound, Iterator current,
        IteratorBound upper_bound);
    /*`Get an `iterator_range` containing the current line. Applicable to any
       iterator. */ 

    //`[heading get_column]
    template <class Iterator, class IteratorBound>
    inline std::size_t get_column(IteratorBound lower_bound, Iterator current,
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

        template<typename Iterator>
        struct line_pos_base_iterator_category {
            typedef typename boost::iterators::iterator_category<
                typename line_pos_base_iterator_type<Iterator>::type
            >::type type;
        };


        template<typename Iterator>
        inline typename line_pos_base_iterator_type<Iterator>::type
            get_line_pos_base_iterator(Iterator it)
        {
            return it;
        }

        template<typename Iterator>
        inline typename line_pos_base_iterator_type<Iterator>::type
            get_line_pos_base_iterator(line_pos_iterator<Iterator> it)
        {
            return it.base();
        }


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
                for (typename line_pos_base_iterator_type<Iterator>::type i = boost::prior(current); i != lower_bound; --i) {
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




    template <class Iterator, class IteratorBound>
    inline typename line_pos_base_iterator_type<Iterator>::type get_line_start(IteratorBound lower_bound, Iterator current)
    {
        return impl::get_line_start(
            impl::get_line_pos_base_iterator(lower_bound),
            impl::get_line_pos_base_iterator(current),
            typename impl::line_pos_base_iterator_category<Iterator>::type()
        );
    }



    template <class Iterator, class IteratorBound>
    inline typename line_pos_base_iterator_type<Iterator>::type get_line_end(Iterator current, IteratorBound upper_bound)
    {
        typename line_pos_base_iterator_type<Iterator>::type upper_bound_base = impl::get_line_pos_base_iterator(upper_bound);
        for (typename line_pos_base_iterator_type<Iterator>::type i = impl::get_line_pos_base_iterator(current);
            i != upper_bound_base; ++i) {
            if ((*i == '\n') || (*i == '\r')) {
                return i;
            }
        }
        return upper_bound_base;
    }

    
    template <class Iterator, class IteratorBound>
    inline iterator_range<typename line_pos_base_iterator_type<Iterator>::type>
        get_current_line(IteratorBound lower_bound, Iterator current,
            IteratorBound upper_bound)
    {
        typename line_pos_base_iterator_type<Iterator>::type current_base = impl::get_line_pos_base_iterator(current);

        typename line_pos_base_iterator_type<Iterator>::type first = get_line_start(impl::get_line_pos_base_iterator(lower_bound), current_base);
        typename line_pos_base_iterator_type<Iterator>::type last = get_line_end(current_base, impl::get_line_pos_base_iterator(upper_bound));
        return iterator_range<typename line_pos_base_iterator_type<Iterator>::type>(first, last);
    }
    
    template <class Iterator, class IteratorBound>
    inline std::size_t get_column(IteratorBound lower_bound,
                                  Iterator current,
                                  std::size_t tabs)
    {
        typename line_pos_base_iterator_type<Iterator>::type current_base = impl::get_line_pos_base_iterator(current);

        std::size_t column = 1;
        typename line_pos_base_iterator_type<Iterator>::type first = get_line_start(lower_bound, current_base);
      
        for (typename line_pos_base_iterator_type<Iterator>::type i = first; i != current_base; ++i) {
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

}}

#endif // BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR

