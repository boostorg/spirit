//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_OUTPUT_ITERATOR_MAY_26_2007_0506PM)
#define BOOST_SPIRIT_KARMA_OUTPUT_ITERATOR_MAY_26_2007_0506PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <iterator>
#include <vector>
#include <algorithm>

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/if.hpp>

#include <boost/spirit/home/karma/detail/ostream_iterator.hpp>
#include <boost/spirit/home/support/unused.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail 
{
    ///////////////////////////////////////////////////////////////////////////
    //  This class is used to keep track of the current position in the output.
    ///////////////////////////////////////////////////////////////////////////
    class position_sink 
    {
    public:
        position_sink() : count(0), line(1), column(1) {}
        void tidy() { count = 0; line = 1; column = 1; }

        template <typename T>
        void output(T const& value) 
        {
            ++count; 
            if (value == '\n') {
                ++line;
                column = 1;
            }
            else {
                ++column;
            }
        }
        std::size_t get_count() const { return count; }
        std::size_t get_line() const { return line; }
        std::size_t get_column() const { return column; }

    private:
        std::size_t count;
        std::size_t line;
        std::size_t column;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  This class is used to count the number of characters streamed into the 
    //  output.
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    class counting_sink : boost::noncopyable
    {
    public:
        counting_sink(OutputIterator& sink_, std::size_t count_ = 0
              , bool enabled = true) 
          : sink(sink_), count(count_) 
        {
            prev_count = sink.chain_counting(enabled ? this : NULL);
        }
        ~counting_sink() 
        {
            sink.chain_counting(prev_count);
        }

        void output() 
        { 
            ++count; 
            if (prev_count)           // propagate count 
                prev_count->output();
        }
        std::size_t get_count() const { return count; }

    private:
        std::size_t count;
        counting_sink* prev_count;                // previous counter in chain
        OutputIterator& sink;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The following classes are used to intercept the output into a buffer
    //  allowing to do things like alignment, character escaping etc.
    //
    //  We need to use virtual functions because output_iterators do not have
    //  an associated value_type. The type of the buffer elements is available
    //  at insertion time only (and not at buffer creation time).
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    struct abstract_container
    {
        virtual ~abstract_container() {}
        virtual void output(void const *item) = 0;
        virtual bool copy(OutputIterator& sink, std::size_t maxwidth) const = 0;
        virtual std::size_t buffer_size() const = 0;
    };

    template <typename OutputIterator, typename T>
    class concrete_container : public abstract_container<OutputIterator>
    {
    public:
        concrete_container(std::size_t size)
        { 
            buffer.reserve(size); 
        }
        ~concrete_container() {}

    private:
        void output(void const *item)
        {
            buffer.push_back(*static_cast<T const*>(item));
        }
        bool copy(OutputIterator& sink, std::size_t maxwidth) const
        {
#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable: 4267)
#endif
            typename std::basic_string<T>::const_iterator end = 
                buffer.begin() + (std::min)(buffer.size(), maxwidth);

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

            std::copy(buffer.begin(), end, sink);
            return true;
        }
//         template <typename RestIterator>
//         bool copy_rest(RestIterator& restsink, std::size_t start_at) const
//         {
//             typename std::basic_string<T>::const_iterator begin = 
//                 buffer.begin() + (std::min)(buffer.size(), start_at);
// 
//             std::copy(begin, buffer.end(), restsink);
//             return true;
//         }
        std::size_t buffer_size() const
        {
            return buffer.size();
        }

    private:
        std::basic_string<T> buffer;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    class buffer_sink : boost::noncopyable
    {
    public:
        buffer_sink()
          : width(0), buffer(0) {}

        ~buffer_sink() 
        { 
            tidy(); 
        }

        void enable(std::size_t width_) 
        { 
            tidy();             // release existing buffer
            width = width_; 
        }

        void tidy() 
        { 
            delete buffer; buffer = 0; width = 0; 
        }

        template <typename T>
        void output(T const& value)
        {
            if (0 == buffer)
            {
                typedef concrete_container<OutputIterator, T> container;
                buffer = new container(width);
            }
            buffer->output(&value);
        }

        bool copy(OutputIterator& sink, std::size_t maxwidth) const 
        { 
            return buffer ? buffer->copy(sink, maxwidth) : false; 
        }
//         template <typename RestIterator>
//         bool copy_rest(RestIterator& sink, std::size_t start_at) const 
//         { 
//             return buffer ? buffer->copy_rest(sink, start_at) : false; 
//         }

        std::size_t buffer_size() const 
        { 
            return buffer ? buffer->buffer_size() : 0; 
        }

    private:
        std::size_t width;
        abstract_container<OutputIterator> *buffer;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  forward declaration only
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator> struct enable_buffering;

    ///////////////////////////////////////////////////////////////////////////
    //  Karma uses an output iterator wrapper for all output operations. This
    //  is necessary to avoid the dreaded 'scanner business' problem, i.e. the
    //  dependency of rules and grammars on the used output iterator. 
    //
    //  By default the user supplied output iterator is wrapped inside an 
    //  instance of this internal output_iterator class. 
    //
    //  This output_iterator class normally just forwards to the embedded user
    //  supplied iterator. But it is possible to enable additional functionality
    //  on demand, such as counting, buffering, and position tracking.
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator, typename Derived = unused_type>
    class output_iterator : boost::noncopyable
    {
    private:
        struct output_proxy 
        {
            output_proxy(output_iterator& parent) 
              : parent(parent) 
            {}

            template <typename T> 
            output_proxy& operator=(T const& value) 
            {
                parent.output(value);
                return *this; 
            }

        private:
            output_iterator& parent;

            // suppress warning about assignment operator not being generated
            output_proxy& operator=(output_proxy const&);
        };

        // get the most derived type of this class
        typedef typename mpl::if_<
            traits::is_not_unused<Derived>, Derived, output_iterator
        >::type most_derived_type;

#if !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
    private:
        friend struct counting_sink<output_iterator<OutputIterator, Derived> >;
        friend struct enable_buffering<output_iterator<OutputIterator, Derived> >;
#else
    public:
#endif
        // functions related to counting
        counting_sink<most_derived_type>* chain_counting(
            counting_sink<most_derived_type>* count_data)
        {
            counting_sink<most_derived_type>* prev_count = count;
            count = count_data;
            return prev_count;
        }

        // functions related to buffering
        buffer_sink<most_derived_type>* chain_buffering(
            buffer_sink<most_derived_type>* buffer_data)
        {
            buffer_sink<most_derived_type>* prev_buffer = buffer;
            buffer = buffer_data;
            return prev_buffer;
        }

    public:
        typedef std::output_iterator_tag iterator_category;
        typedef void value_type;
        typedef void difference_type;
        typedef void pointer;
        typedef void reference;

        explicit output_iterator(OutputIterator& sink_)
          : sink(sink_), count(NULL), buffer(NULL)
        {}
        output_iterator(output_iterator const& rhs)
          : sink(rhs.sink), count(rhs.count), buffer(rhs.buffer)
          , track_position_data(rhs.track_position_data)
        {}

        output_proxy operator*() { return output_proxy(*this); }
        output_iterator& operator++() { ++sink; return *this; } 
        output_iterator operator++(int) 
        {
            output_iterator t(*this);
            ++sink; 
            return t; 
        }

        template <typename T> 
        void output(T const& value) 
        { 
            if (NULL != count)          // count characters, if appropriate
                count->output();

            // always track position in the output (this is needed by different 
            // generators, such as indent, pad, etc.)
            track_position_data.output(value);

            if (NULL != buffer)         // buffer output, if appropriate
                buffer->output(value);
            else
                *sink = value; 
        }

        // return the current count in the output
        std::size_t get_out_count() const
        {
            return track_position_data.get_count();
        }

        // plain output iterators are considered to be good all the time
        bool good() const { return true; }

    protected:
        // this is the wrapped user supplied output iterator
        OutputIterator& sink;

    private:
        // these are the hooks providing optional functionality
        counting_sink<most_derived_type>* count;      // for counting
        buffer_sink<most_derived_type>* buffer;       // for buffering
        position_sink track_position_data;            // for position tracking

        // suppress warning about assignment operator not being generated
        output_iterator& operator=(output_iterator const&);
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Elem, typename Traits>
    class output_iterator<ostream_iterator<T, Elem, Traits> >
      : public output_iterator<ostream_iterator<T, Elem, Traits>
          , output_iterator<ostream_iterator<T, Elem, Traits> > >
    {
    private:
        typedef output_iterator<ostream_iterator<T, Elem, Traits>
              , output_iterator<ostream_iterator<T, Elem, Traits> > 
        > base_type;
        typedef ostream_iterator<T, Elem, Traits> base_iterator_type;
        typedef std::basic_ostream<Elem, Traits> ostream_type;

    public:
        output_iterator(base_iterator_type& sink)
          : base_type(sink) {}

        ostream_type& get_ostream() { return this->sink.get_ostream(); }
        ostream_type const& get_ostream() const { return this->sink.get_ostream(); }

        // expose good bit of underlying stream object
        bool good() const { return this->sink.get_ostream().good(); }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Helper class for exception safe enabling of character counting in the
    //  output iterator
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    struct enable_counting
    {
        enable_counting(OutputIterator& sink_, std::size_t count = 0)
          : count_data(sink_, count) {}

        // get number of characters counted since last enable
        std::size_t count() const
        {
            return count_data.get_count();
        }

    private:
        counting_sink<OutputIterator> count_data;              // for counting
    };

    template <typename OutputIterator>
    struct disable_counting
    {
        disable_counting(OutputIterator& sink_)
          : count_data(sink_, 0, false) {}

    private:
        counting_sink<OutputIterator> count_data;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Helper class for exception safe enabling of character buffering in the
    //  output iterator
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    struct enable_buffering
    {
        enable_buffering(OutputIterator& sink_, std::size_t width = 0)
          : sink(sink_), prev_buffer(NULL), enabled(false)
        {
            buffer_data.enable(width);
            prev_buffer = sink.chain_buffering(&buffer_data);
            enabled = true;
        }
        ~enable_buffering()
        {
            disable();
        }

        // reset buffer chain to initial state
        void disable()
        {
            if (enabled) {
                BOOST_VERIFY(&buffer_data == sink.chain_buffering(prev_buffer));
                enabled = false;
            }
        }

        // copy to the underlying sink whatever is in the local buffer
        bool buffer_copy(std::size_t maxwidth = std::size_t(-1)
          , bool disable_ = true)
        {
            if (disable_)
                disable();
            return buffer_data.copy(sink, maxwidth) && sink.good();
        }

        // return number of characters stored in the buffer
        std::size_t buffer_size() const
        {
            return buffer_data.buffer_size();
        }

//         bool buffer_copy_rest(std::size_t start_at = 0)
//         {
//             return buffer_data.copy_rest(sink, start_at);
//         }

    private:
        OutputIterator& sink;
        buffer_sink<OutputIterator> buffer_data;    // for buffering
        buffer_sink<OutputIterator>* prev_buffer;   // previous buffer in chain
        bool enabled;
    };

}}}}

#endif 

