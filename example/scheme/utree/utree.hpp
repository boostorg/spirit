/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE)
#define BOOST_SPIRIT_UTREE

#include <cstddef>
#include <algorithm>
#include <string>
#include <ostream>

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/ref.hpp>
#include <utree/detail/utree_detail1.hpp>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804)
# pragma warning(disable: 4805)
# pragma warning(disable: 4244)
#endif

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // Our utree can store these types. This enum tells us what type
    // of data is stored in utree's discriminated union.
    ///////////////////////////////////////////////////////////////////////////
    struct utree_type
    {
        enum info
        {
            nil_type,
            bool_type,
            int_type,
            double_type,
            string_type,
            symbol_type,
            binary_type,
            list_type,
            range_type,
            reference_type,
            function_type
        };
    };

    ///////////////////////////////////////////////////////////////////////////
    // The nil type
    ///////////////////////////////////////////////////////////////////////////
    struct nil {};

    ///////////////////////////////////////////////////////////////////////////
    // A typed string with parametric Base storage. The storage can be any
    // range or (stl container) of chars.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Base, utree_type::info type_>
    struct basic_string : Base
    {
        static utree_type::info const type = type_;

        basic_string()
          : Base() {}

        basic_string(Base const& base)
          : Base(base) {}

        template <typename Iterator>
        basic_string(Iterator bits, std::size_t len)
          : Base(bits, bits + len) {}

        template <typename Iterator>
        basic_string(Iterator first, Iterator last)
          : Base(first, last) {}

        basic_string& operator=(basic_string const& other)
        {
            Base::operator=(other);
            return *this;
        }

        basic_string& operator=(Base const& other)
        {
            Base::operator=(other);
            return *this;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Binary string
    ///////////////////////////////////////////////////////////////////////////
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::binary_type>
    binary_range;

    typedef basic_string<
        std::string,
        utree_type::binary_type>
    binary_string;

    ///////////////////////////////////////////////////////////////////////////
    // Our UTF-8 string
    ///////////////////////////////////////////////////////////////////////////
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::string_type>
    utf8_string_range;

    typedef basic_string<
        std::string,
        utree_type::string_type>
    utf8_string;

    ///////////////////////////////////////////////////////////////////////////
    // Our UTF-8 symbol (for identifiers)
    ///////////////////////////////////////////////////////////////////////////
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::symbol_type>
    utf8_symbol_range;

    typedef basic_string<
        std::string,
        utree_type::symbol_type>
    utf8_symbol;

    ///////////////////////////////////////////////////////////////////////////
    // Our function type
    ///////////////////////////////////////////////////////////////////////////
    class utree;
    typedef boost::iterator_range<utree const*> args_type;

    struct function_base
    {
        virtual ~function_base() {};
        virtual utree operator()(args_type args) const = 0;
        virtual function_base* clone() const = 0;
    };

    template <typename F>
    struct stored_function : function_base
    {
        F f;
        stored_function(F f = F());
        virtual ~stored_function();
        virtual utree operator()(args_type args) const;
        virtual function_base* clone() const;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Shallow tag. Instructs utree to hold an iterator_range
    // as-is without deep copying the range.
    ///////////////////////////////////////////////////////////////////////////
    struct shallow_tag {};
    shallow_tag const shallow = {};

    ///////////////////////////////////////////////////////////////////////////
    // The main utree (Universal Tree) class
    // The utree is a hierarchical, dynamic type that can store:
    //  - a nil
    //  - a bool
    //  - an integer
    //  - a double
    //  - a string
    //  - a symbol (identifier)
    //  - binary data
    //  - a (doubly linked) list of utree
    //  - a reference to a utree
    //
    // The utree has minimal memory footprint. The data structure size is
    // 16 bytes on a 32-bit platform. Being a container of itself, it can
    // represent tree structures.
    ///////////////////////////////////////////////////////////////////////////
    class utree
    {
    public:

        typedef utree value_type;
        typedef detail::list::node_iterator<utree> iterator;
        typedef detail::list::node_iterator<utree const> const_iterator;
        typedef detail::list::node_iterator<boost::reference_wrapper<utree> >
            ref_iterator;
        typedef utree& reference;
        typedef utree const& const_reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;

        typedef boost::iterator_range<iterator> range;
        typedef boost::iterator_range<const_iterator> const_range;

        utree();
        utree(bool b);
        utree(unsigned int i);
        utree(int i);
        utree(double d);
        utree(char const* str);
        utree(char const* str, std::size_t len);
        utree(std::string const& str);
        utree(boost::reference_wrapper<utree> ref);

        template <typename Iter>
        utree(boost::iterator_range<Iter> r);
        utree(range r, shallow_tag);
        utree(const_range r, shallow_tag);

        template <typename F>
        utree(stored_function<F> const& pf);

        template <typename Base, utree_type::info type_>
        utree(basic_string<Base, type_> const& bin);

        utree(utree const& other);
        ~utree();

        utree& operator=(utree const& other);
        utree& operator=(bool b);
        utree& operator=(unsigned int i);
        utree& operator=(int i);
        utree& operator=(double d);
        utree& operator=(char const* s);
        utree& operator=(std::string const& s);
        utree& operator=(boost::reference_wrapper<utree> ref);

        template <typename F>
        utree& operator=(stored_function<F> const& pf);

        template <typename Iter>
        utree& operator=(boost::iterator_range<Iter> r);

        template <typename Base, utree_type::info type_>
        utree& operator=(basic_string<Base, type_> const& bin);

        template <typename F>
        typename F::result_type
        static visit(utree const& x, F f);

        template <typename F>
        typename F::result_type
        static visit(utree& x, F f);

        template <typename F>
        typename F::result_type
        static visit(utree const& x, utree const& y, F f);

        template <typename F>
        typename F::result_type
        static visit(utree& x, utree const& y, F f);

        template <typename F>
        typename F::result_type
        static visit(utree const& x, utree& y, F f);

        template <typename F>
        typename F::result_type
        static visit(utree& x, utree& y, F f);

        template <typename T>
        void push_back(T const& val);

        template <typename T>
        void push_front(T const& val);

        template <typename T>
        iterator insert(iterator pos, T const& x);

        template <typename T>
        void insert(iterator pos, std::size_t, T const& x);

        template <typename Iter>
        void insert(iterator pos, Iter first, Iter last);

        template <typename Iter>
        void assign(Iter first, Iter last);

        void clear();
        void pop_front();
        void pop_back();
        iterator erase(iterator pos);
        iterator erase(iterator first, iterator last);

        utree& front();
        utree& back();
        utree const& front() const;
        utree const& back() const;

        utree& operator[](std::size_t i);
        utree const& operator[](std::size_t i) const;

        void swap(utree& other);

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

        ref_iterator ref_begin();
        ref_iterator ref_end();

        bool empty() const;
        std::size_t size() const;

        utree_type::info which() const;

        template <typename T>
        T get() const;

        utree& deref();
        utree const& deref() const;

        short tag() const;
        void tag(short tag);

        utree eval(args_type args) const;

    private:

        typedef utree_type type;

        template <typename UTreeX, typename UTreeY>
        friend struct detail::visit_impl;
        friend struct detail::index_impl;

        template <typename T>
        friend struct detail::get_impl;

        type::info get_type() const;
        void set_type(type::info t);
        void ensure_list_type();
        void free();
        void copy(utree const& other);

        union
        {
            detail::fast_string s;
            detail::list l;
            detail::range r;
            bool b;
            int i;
            double d;
            utree* p;
            function_base* pf;
        };
    };
}

#if defined(BOOST_MSVC)
# pragma warning(pop)
#endif

#include <utree/detail/utree_detail2.hpp>

// $$$ move this in its own file $$$
namespace scheme { namespace utree_functions
{
    ///////////////////////////////////////////////////////////////////////////
    // Extra functions
    ///////////////////////////////////////////////////////////////////////////
    inline utree rest(utree& x)
    {
        utree::iterator i = x.begin(); ++i;
        return utree(utree::range(i, x.end()), shallow);
    }

    inline utree rest(utree const& x)
    {
        utree::const_iterator i = x.begin(); ++i;
        return utree(utree::const_range(i, x.end()), shallow);
    }
}}

#endif
