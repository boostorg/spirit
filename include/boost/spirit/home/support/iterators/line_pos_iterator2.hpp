/*==============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2010      Bryce Lelbach
    Copyright (c) 2014      Tomoki Imai
    Copyright (c) 2023      Tobias Loew

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(BOOST_SPIRIT_RANDOM_LINE_POS_ITERATOR2)
#define BOOST_SPIRIT_RANDOM_LINE_POS_ITERATOR2

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace boost { namespace spirit
{
    //[line_pos_iterator_class
    /*`The `line_pos_iterator2` is a lightweight line position iterator.
       This iterator adapter only stores the current line number, nothing else.
       Unlike __classic__'s `position_iterator`, it does not store the
       column number and does not need an end iterator. The current column can
       be computed, if needed. Unlike `line_pos_iterator`, this iterator requires
       a */
    //`[heading Class Reference]
    template <class Iterator>
    class line_pos_iterator2 : public boost::iterator_adaptor<
        line_pos_iterator2<Iterator>  // Derived
      , Iterator                     // Base
      , boost::use_default           // Value
      , boost::forward_traversal_tag // CategoryOrTraversal
    > {
        typedef typename std::iterator_traits<Iterator>::iterator_category category;
        BOOST_STATIC_ASSERT((boost::is_convertible<category, boost::iterators::bidirectional_traversal_tag >::value, "bidirectional iterator expected"));

    public:
        line_pos_iterator2();

        explicit line_pos_iterator2(Iterator);

        std::size_t position() const;



        inline Iterator get_line_start(Iterator lower_bound) const
        {
            if (this->base() == lower_bound) {
                return this->base();
            }
            else {
                for (auto i = std::prev(this->base()); i != lower_bound; --i) {
                    if ((*i == '\r') || (*i == '\n')) {
                        return i + 1;
                    }
                }
                return lower_bound;
            }
        }

    private:
        friend class boost::iterator_core_access;

        void increment();

        std::size_t line; // The line position.
        typename std::iterator_traits<Iterator>::value_type prev;
    };
    //]

    template <class Iterator>
    line_pos_iterator2<Iterator>::line_pos_iterator2() :
        line_pos_iterator2::iterator_adaptor_(), line(1), prev(0) { }

    template <class Iterator>
    line_pos_iterator2<Iterator>::line_pos_iterator2(Iterator base) :
        line_pos_iterator2::iterator_adaptor_(base), line(1), prev(0) { }

    template <class Iterator>
    std::size_t line_pos_iterator2<Iterator>::position() const
    {
        return line;
    }
 
    template<class Iterator>
    void line_pos_iterator2<Iterator>::increment()
    {
        typename std::iterator_traits<Iterator>::reference
          ref = *(this->base());
      
        switch (ref) {
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

    //[line_pos_iterator_utilities
    //`[heading get_line]
    template <class Iterator>
    inline std::size_t get_line(line_pos_iterator2<Iterator>);
    /*`Get the line position. */

    //`[heading get_line_start]
    template <class Iterator>
    inline Iterator get_line_start(Iterator lower_bound, line_pos_iterator2<Iterator> current);

    template <class Iterator>
    inline Iterator get_line_start(line_pos_iterator2<Iterator> lower_bound, line_pos_iterator2<Iterator> current) {
        return get_line_start(lower_bound.base(), current);
    }
    /*`Get an iterator to the beginning of the line. */



    //`[heading get_current_line]
    template <class Iterator>
    inline iterator_range<Iterator>
    get_current_line(Iterator lower_bound, line_pos_iterator2<Iterator> current,
        Iterator upper_bound);

    template <class Iterator>
    inline iterator_range<Iterator>
        get_current_line(line_pos_iterator2<Iterator> lower_bound, line_pos_iterator2<Iterator> current,
            line_pos_iterator2<Iterator> upper_bound) {
        return get_current_line(lower_bound.base(), current, upper_bound.base());
    }
    /*`Get an `iterator_range` containing the current line. */



    //`[heading get_column]
    template <class Iterator>
    inline std::size_t get_column(Iterator lower_bound, line_pos_iterator2<Iterator> current,
                                  std::size_t tabs = 4); 

    template <class Iterator>
    inline std::size_t get_column(line_pos_iterator2<Iterator> lower_bound, line_pos_iterator2<Iterator> current,
        std::size_t tabs = 4) {
        return get_column(lower_bound.base(), current, tabs);
    }
    /*`Get the current column. */
    //]



    template <class Iterator>
    inline std::size_t get_line(line_pos_iterator2<Iterator> i)
    {
        return i.position();
    }
    
    template <class Iterator>
    inline Iterator get_line_start(Iterator lower_bound, line_pos_iterator2<Iterator> current)
    {
        return current.get_line_start(lower_bound);
    }

    template <class Iterator>
    inline Iterator get_line_end(line_pos_iterator2<Iterator> current, Iterator upper_bound)
    {
        for (Iterator i = current.base(); i != upper_bound; ++i) {
            if ((*i == '\n') || (*i == '\r')) {
                return i;
            }
        }
        return upper_bound;
    }

    
    template <class Iterator>
    inline iterator_range<Iterator>
    get_current_line(Iterator lower_bound,
        line_pos_iterator2<Iterator> current,
        Iterator upper_bound)
    {
        Iterator first = get_line_start(lower_bound, current);
        Iterator last = get_line_end(current, upper_bound);
        return iterator_range<Iterator>(first, last);
    }
    
    template <class Iterator>
    inline std::size_t get_column(Iterator lower_bound,
        line_pos_iterator2<Iterator> current,
                                  std::size_t tabs)
    {
        std::size_t column = 1;
        Iterator first = get_line_start(lower_bound, current);
      
        for (Iterator i = first; i != current.base(); ++i) {
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

#endif // BOOST_SPIRIT_RANDOM_LINE_POS_ITERATOR2
