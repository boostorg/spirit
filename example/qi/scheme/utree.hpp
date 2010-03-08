/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE)
#define BOOST_SPIRIT_UTREE

#include <cstddef>
#include <algorithm>
#include <string>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/type_traits/is_pointer.hpp>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804)
# pragma warning(disable: 4805)
# pragma warning(disable: 4244)
#endif

namespace scheme { namespace detail
{
    template <typename UTreeX, typename UTreeY>
    struct visit_impl;
    struct index_impl;

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
            small_string_type,
            heap_string_type,
            list_type
        };
    };

    ///////////////////////////////////////////////////////////////////////////
    // Our POD fast string. This implementation is very primitive and is not
    // meant to be used stand-alone. This is the internal data representation
    // of strings in our utree. This is deliberately a POD to allow it to be
    // placed in a union. This POD fast string specifically utilizes
    // (sizeof(double) * 2) - (2 * sizeof(char)). In a 32 bit system, this is
    // 14 bytes. The two extra bytes are used by utree to store management info.
    //
    // It is a const string (i.e. immutable). It stores the characters directly
    // if possible and only uses the heap if the string does not fit. Null
    // characters are allowed, making it suitable to encode raw binary. The
    // string length is encoded in the first byte if the string is placed in-situ,
    // else, the length plus a pointer to the string in the heap are stored.
    ///////////////////////////////////////////////////////////////////////////
    struct fast_string // Keep this a POD!
    {
        static std::size_t const
            buff_size = (sizeof(double)*2)/sizeof(char);

        static std::size_t const
            small_string_size = buff_size-(sizeof(char)*2);

        struct heap_store
        {
            char* str;
            std::size_t size;
        };

        union
        {
            char buff[buff_size];
            heap_store heap;
        };

        utree_type::info get_type() const;
        void set_type(utree_type::info t);
        std::size_t size() const;
        char const* str() const;

        template <typename Iterator>
        void construct(Iterator f, Iterator l);

        void swap(fast_string& other);
        void free();
        void copy(fast_string const& other);
    };

    ///////////////////////////////////////////////////////////////////////////
    // Our POD double linked list. Straightforward implementation.
    // This implementation is very primitive and is not meant to be
    // used stand-alone. This is the internal data representation
    // of lists in our utree.
    ///////////////////////////////////////////////////////////////////////////
    struct list // keep this a POD!
    {
        struct node;

        template <typename Value>
        class node_iterator;

        void free();
        void copy(list const& other);
        void default_construct();

        template <typename T>
        void insert_before(T const& val, node* node);

        template <typename T>
        void insert_after(T const& val, node* node);

        template <typename T>
        void push_front(T const& val);

        template <typename T>
        void push_back(T const& val);

        void pop_front();
        void pop_back();
        node* erase(node* pos);

        node* first;
        node* last;
        std::size_t size;
    };
}}

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // The main utree (Universal Tree) class
    // The utree is a hierarchical, dynamic type that can store:
    //  - a nil
    //  - a bool
    //  - an integer
    //  - a double
    //  - a string (textual or binary)
    //  - a (doubly linked) list of utree
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
        typedef utree& reference;
        typedef utree const& const_reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;

        struct nil {};

        utree();
        explicit utree(bool b);
        explicit utree(unsigned int i);
        explicit utree(int i);
        explicit utree(double d);
        explicit utree(char const* str);
        explicit utree(char const* str, std::size_t len);
        explicit utree(std::string const& str);

        utree(utree const& other);
        ~utree();

        utree& operator=(utree const& other);
        utree& operator=(bool b);
        utree& operator=(unsigned int i);
        utree& operator=(int i);
        utree& operator=(double d);
        utree& operator=(char const* s);
        utree& operator=(std::string const& s);

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

        bool empty() const;
        std::size_t size() const;

    private:

        typedef detail::utree_type type;

        template <typename UTreeX, typename UTreeY>
        friend struct detail::visit_impl;
        friend struct detail::index_impl;
        friend struct ulist;

        type::info get_type() const;
        void set_type(type::info t);
        void ensure_list_type();
        void free();
        void copy(utree const& other);

        struct construct_list {};
        utree(construct_list);

        union
        {
            detail::fast_string s;
            detail::list l;
            bool b;
            int i;
            double d;
        };
    };

    bool operator==(utree const& a, utree const& b);
    bool operator<(utree const& a, utree const& b);
    bool operator!=(utree const& a, utree const& b);
    bool operator>(utree const& a, utree const& b);
    bool operator<=(utree const& a, utree const& b);
    bool operator>=(utree const& a, utree const& b);

    ///////////////////////////////////////////////////////////////////////////
    // The ulist is a utility class for easy construction of utree lists
    ///////////////////////////////////////////////////////////////////////////
    struct ulist : utree
    {
        ulist() : utree(construct_list()) {}
    };
}

namespace scheme { namespace detail
{
    inline utree_type::info fast_string::get_type() const
    {
        return static_cast<utree_type::info>(buff[small_string_size]);
    }

    inline void fast_string::set_type(utree_type::info t)
    {
        buff[small_string_size] = static_cast<char>(t);
    }

    inline std::size_t fast_string::size() const
    {
        BOOST_ASSERT(get_type() == utree_type::small_string_type
            || get_type() == utree_type::heap_string_type);

        if (get_type() == utree_type::small_string_type)
            return buff[0];
        else
            return heap.size;
    }

    inline char const* fast_string::str() const
    {
        BOOST_ASSERT(get_type() == utree_type::small_string_type
            || get_type() == utree_type::heap_string_type);

        if (get_type() == utree_type::small_string_type)
            return buff + 1;
        else
            return heap.str;
    }

    template <typename Iterator>
    inline void fast_string::construct(Iterator f, Iterator l)
    {
        unsigned const size = l-f;
        char* str;
        if (size < small_string_size)
        {
            // if it fits, store it in-situ; the first byte
            // is the length of the string.
            str = buff + 1;
            buff[0] = size;
            set_type(utree_type::small_string_type);
        }
        else
        {
            // else, store it in the heap
            str = new char[size];
            heap.str = str;
            heap.size = size;
            set_type(utree_type::heap_string_type);
        }
        for (std::size_t i = 0; i != size; ++i)
        {
            *str++ = *f++;
        }
    }

    inline void fast_string::swap(fast_string& other)
    {
        fast_string temp = other;
        other = *this;
        *this = temp;
    }

    inline void fast_string::free()
    {
        if (get_type() == utree_type::heap_string_type)
        {
            delete [] heap.str;
            heap.str = 0;
        }
    }

    inline void fast_string::copy(fast_string const& other)
    {
        construct(other.str(), other.str() + other.size());
    }

    struct list::node : boost::noncopyable
    {
        template <typename T>
        node(T const& val, node* next, node* prev)
          : val(val), next(next), prev(prev) {}

        void unlink()
        {
            prev->next = next;
            next->prev = prev;
        }

        utree val;
        node* next;
        node* prev;
    };

    template <typename Value>
    class list::node_iterator
      : public boost::iterator_facade<
            node_iterator<Value>
          , Value
          , boost::bidirectional_traversal_tag
        >
    {
    public:

        node_iterator()
          : node(0) {}

        explicit node_iterator(list::node* p)
          : node(p) {}

    private:

        friend class boost::iterator_core_access;
        friend class scheme::utree;

        void increment()
        {
            node = node->next;
        }

        void decrement()
        {
            node = node->prev;
        }

        bool equal(node_iterator const& other) const
        {
            return node == other.node;
        }

        utree& dereference() const
        {
            return node->val;
        }

        list::node* node;
    };

    inline void list::free()
    {
        node* p = first;
        while (p != last)
        {
            node* next = p->next;
            delete p;
            p = next;
        }
        first = last = 0;
        size = 0;
    }

    inline void list::copy(list const& other)
    {
        first = last = 0;
        node* p = other.first;
        while (p != 0)
        {
            push_back(p->val);
            p = p->next;
        }
    }

    inline void list::default_construct()
    {
        first = last = 0;
        size = 0;
    }

    template <typename T>
    inline void list::insert_before(T const& val, node* np)
    {
        BOOST_ASSERT(np != 0);
        node* new_node = new node(val, np, np->prev);
        if (np->prev)
            np->prev->next = new_node;
        else
            first = new_node;
        np->prev = new_node;
        ++size;
    }

    template <typename T>
    inline void list::insert_after(T const& val, node* np)
    {
        BOOST_ASSERT(np != 0);
        node* new_node = new node(val, np->next, np);
        if (np->next)
            np->next->prev = new_node;
        else
            last = new_node;
        np->next = new_node;
        ++size;
    }

    template <typename T>
    inline void list::push_front(T const& val)
    {
        detail::list::node* new_node;
        if (first == 0)
        {
            new_node = new detail::list::node(val, 0, 0);
            first = last = new_node;
            ++size;
        }
        else
        {
             insert_before(val, first);
        }
    }

    template <typename T>
    inline void list::push_back(T const& val)
    {
        if (last == 0)
            push_front(val);
        else
            insert_after(val, last);
    }

    inline void list::pop_front()
    {
        BOOST_ASSERT(size != 0);
        node* np = first;
        first = first->next;
        first->prev = 0;
        delete np;
        --size;
    }

    inline void list::pop_back()
    {
        BOOST_ASSERT(size != 0);
        node* np = last;
        last = last->prev;
        last->next = 0;
        delete np;
        --size;
    }

    inline list::node* list::erase(node* pos)
    {
        BOOST_ASSERT(pos != 0);
        if (pos == first)
        {
            pop_front();
            return first;
        }
        else if (pos == last)
        {
            pop_back();
            return 0;
        }
        else
        {
            node* next(pos->next);
            pos->unlink();
            delete pos;
            --size;
            return next;
        }
    }

    template <typename F, typename X>
    struct bind_impl // simple binder for binary visitation (we don't want to bring in the big guns)
    {
        typedef typename F::result_type result_type;
        X& x; // always by reference
        F f;
        bind_impl(F f, X& x) : x(x), f(f) {}

        template <typename Y>
        typename F::result_type operator()(Y& y) const
        {
            return f(x, y);
        }

        template <typename Y>
        typename F::result_type operator()(Y const& y) const
        {
            return f(x, y);
        }
    };

    template <typename F, typename X>
    bind_impl<F, X const> bind(F f, X const& x)
    {
        return bind_impl<F, X const>(f, x);
    }

    template <typename F, typename X>
    bind_impl<F, X> bind(F f, X& x)
    {
        return bind_impl<F, X>(f, x);
    }

    struct utree_is_equal
    {
        typedef bool result_type;

        template <typename A, typename B>
        bool dispatch(const A&, const B&, boost::mpl::false_) const
        {
            return false; // cannot compare different types by default
        }

        template <typename A, typename B>
        bool dispatch(const A& a, const B& b, boost::mpl::true_) const
        {
            return a == b; // for arithmetic types
        }

        template <typename A, typename B>
        bool operator()(const A& a, const B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<boost::is_arithmetic<A>, boost::is_arithmetic<B> >());
        }

        template <typename T>
        bool operator()(const T& a, const T& b) const
        {
            // This code works for lists and strings as well
            return a == b;
        }

        bool operator()(utree::nil, utree::nil) const
        {
            return true;
        }
    };

    struct utree_is_less_than
    {
        typedef bool result_type;

        template <typename A, typename B>
        bool dispatch(const A&, const B&, boost::mpl::false_) const
        {
            return false; // cannot compare different types by default
        }

        template <typename A, typename B>
        bool dispatch(const A& a, const B& b, boost::mpl::true_) const
        {
            return a < b; // for arithmetic types
        }

        template <typename A, typename B>
        bool operator()(const A& a, const B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<boost::is_arithmetic<A>, boost::is_arithmetic<B> >());
        }

        template <typename T>
        bool operator()(const T& a, const T& b) const
        {
            // This code works for lists and strings as well
            return a < b;
        }

        bool operator()(utree::nil, utree::nil) const
        {
            BOOST_ASSERT(false);
            return false; // no less than comparison for nil
        }
    };

    template <typename UTreeX, typename UTreeY = UTreeX>
    struct visit_impl
    {
        template <typename F>
        typename F::result_type
        static apply(UTreeX& x, F f) // single dispatch
        {
            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>, char const*, char*>::type
            string_type;

            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>,
                typename UTreeX::const_iterator,
                typename UTreeX::iterator>::type
            iterator;

            typedef boost::iterator_range<iterator> list_range;
            typedef boost::iterator_range<string_type> string_range;
            typedef detail::utree_type type;

            switch (x.get_type())
            {
                default:
                    BOOST_ASSERT(false); // can't happen
                case type::nil_type:
                    typename UTreeX::nil arg;
                    return f(arg);
                case type::bool_type:
                    return f(x.b);
                case type::int_type:
                    return f(x.i);
                case type::double_type:
                    return f(x.d);
                case type::list_type:
                    return f(list_range(iterator(x.l.first), iterator(0)));
                case type::heap_string_type:
                case type::small_string_type:
                    return f(string_range(x.s.str(), x.s.str() + x.s.size()));
            }
        }

        template <typename F>
        typename F::result_type
        static apply(UTreeX& x, UTreeY& y, F f) // double dispatch
        {
            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>, char const*, char*>::type
            string_type;

            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>,
                typename UTreeX::const_iterator,
                typename UTreeX::iterator>::type
            iterator;

            typedef boost::iterator_range<iterator> list_range;
            typedef boost::iterator_range<string_type> string_range;
            typedef detail::utree_type type;

            switch (x.get_type())
            {
                default:
                    BOOST_ASSERT(false); // can't happen
                case type::nil_type:
                    typename UTreeX::nil x_;
                    return visit_impl::apply(y, detail::bind(f, x_));
                case type::bool_type:
                    return visit_impl::apply(y, detail::bind(f, x.b));
                case type::int_type:
                    return visit_impl::apply(y, detail::bind(f, x.i));
                case type::double_type:
                    return visit_impl::apply(y, detail::bind(f, x.d));
                case type::list_type:
                    return visit_impl::apply(
                        y, detail::bind<F, list_range>(f,
                        list_range(iterator(x.l.first), iterator(0))));
                case type::heap_string_type:
                case type::small_string_type:
                    return visit_impl::apply(y, detail::bind(
                        f, string_range(x.s.str(), x.s.str() + x.s.size())));
            }
        }
    };

    struct index_impl
    {
        static utree& apply(detail::list::node* node, std::size_t i)
        {
            for (; i > 0; --i)
                node = node->next;
            return node->val;
        }

        static utree const& apply(detail::list::node const* node, std::size_t i)
        {
            for (; i > 0; --i)
                node = node->next;
            return node->val;
        }
    };
}}

namespace scheme
{
    inline utree::utree()
    {
        set_type(type::nil_type);
    }

    inline utree::utree(bool b) : b(b)
    {
        set_type(type::bool_type);
    }

    inline utree::utree(unsigned int i) : i(i)
    {
        set_type(type::int_type);
    }

    inline utree::utree(int i) : i(i)
    {
        set_type(type::int_type);
    }

    inline utree::utree(double d) : d(d)
    {
        set_type(type::double_type);
    }

    inline utree::utree(char const* str)
    {
        s.construct(str, str + strlen(str));
    }

    inline utree::utree(char const* str, std::size_t len)
    {
        s.construct(str, str + len);
    }

    inline utree::utree(std::string const& str)
    {
        s.construct(str.begin(), str.end());
    }

    inline utree::utree(utree const& other)
    {
        copy(other);
    }

    inline utree::~utree()
    {
        free();
    }

    inline utree& utree::operator=(utree const& other)
    {
        if (this != &other)
        {
            free();
            copy(other);
        }
        return *this;
    }

    inline utree& utree::operator=(bool b_)
    {
        free();
        b = b_;
        set_type(type::bool_type);
        return *this;
    }

    inline utree& utree::operator=(unsigned int i_)
    {
        free();
        i = i_;
        set_type(type::int_type);
        return *this;
    }

    inline utree& utree::operator=(int i_)
    {
        free();
        i = i_;
        set_type(type::int_type);
        return *this;
    }

    inline utree& utree::operator=(double d_)
    {
        free();
        d = d_;
        set_type(type::double_type);
        return *this;
    }

    inline utree& utree::operator=(char const* s_)
    {
        free();
        s.construct(s_, s_ + strlen(s_));
        return *this;
    }

    inline utree& utree::operator=(std::string const& s_)
    {
        free();
        s.construct(s_.begin(), s_.end());
        return *this;
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree const& x, F f)
    {
        return detail::visit_impl<utree const>::apply(x, f);
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree& x, F f)
    {
        return detail::visit_impl<utree>::apply(x, f);
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree const& x, utree const& y, F f)
    {
        return detail::visit_impl<utree const, utree const>::apply(x, y, f);
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree const& x, utree& y, F f)
    {
        return detail::visit_impl<utree const, utree>::apply(x, y, f);
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree& x, utree const& y, F f)
    {
        return detail::visit_impl<utree, utree const>::apply(x, y, f);
    }

    template <typename F>
    typename F::result_type
    inline utree::visit(utree& x, utree& y, F f)
    {
        return detail::visit_impl<utree, utree>::apply(x, y, f);
    }

    inline utree& utree::operator[](std::size_t i)
    {
        BOOST_ASSERT(get_type() == type::list_type && size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    inline utree const& utree::operator[](std::size_t i) const
    {
        BOOST_ASSERT(get_type() == type::list_type && size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    inline bool operator==(utree const& a, utree const& b)
    {
        return utree::visit(a, b, detail::utree_is_equal());
    }

    inline bool operator<(utree const& a, utree const& b)
    {
        return utree::visit(a, b, detail::utree_is_less_than());
    }

    inline bool operator!=(utree const& a, utree const& b)
    {
        return !(a == b);
    }

    inline bool operator>(utree const& a, utree const& b)
    {
        return b < a;
    }

    inline bool operator<=(utree const& a, utree const& b)
    {
        return !(b < a);
    }

    inline bool operator>=(utree const& a, utree const& b)
    {
        return !(a < b);
    }

    template <typename T>
    inline void utree::push_front(T const& val)
    {
        ensure_list_type();
        l.push_front(val);
    }

    template <typename T>
    inline void utree::push_back(T const& val)
    {
        ensure_list_type();
        l.push_back(val);
    }

    template <typename T>
    inline utree::iterator utree::insert(iterator pos, T const& val)
    {
        ensure_list_type();
        if (pos.node == l.last)
        {
            push_back(val);
            return begin();
        }
        else
        {
            l.insert_before(val, pos.node);
            return utree::iterator(pos.node->prev);
        }
    }


    template <typename T>
    inline void utree::insert(iterator pos, std::size_t n, T const& val)
    {
        for (std::size_t i = 0; i != n; ++i)
            insert(pos, val);
    }

    template <typename Iter>
    inline void utree::insert(iterator pos, Iter first, Iter last)
    {
        ensure_list_type();
        while (first != last)
            insert(pos, *first++);
    }

    template <typename Iter>
    inline void utree::assign(Iter first, Iter last)
    {
        ensure_list_type();
        clear();
        while (first != last)
            push_back(*first++);
    }

    inline void utree::clear()
    {
        // clear will always make this a nil type
        free();
        set_type(type::nil_type);
    }

    inline void utree::pop_front()
    {
        BOOST_ASSERT(get_type() == type::list_type);
        l.pop_front();
    }

    inline void utree::pop_back()
    {
        BOOST_ASSERT(get_type() == type::list_type);
        l.pop_back();
    }

    inline utree::iterator utree::erase(iterator pos)
    {
        BOOST_ASSERT(get_type() == type::list_type);
        return iterator(l.erase(pos.node));
    }

    inline utree::iterator utree::erase(iterator first, iterator last)
    {
        while (first != last)
            erase(first++);
        return last;
    }

    inline utree::iterator utree::begin()
    {
        ensure_list_type();
        return iterator(l.first);
    }

    inline utree::iterator utree::end()
    {
        ensure_list_type();
        return iterator(l.last);
    }

    inline utree::const_iterator utree::begin() const
    {
        BOOST_ASSERT(get_type() == type::list_type);
        return const_iterator(l.first);
    }

    inline utree::const_iterator utree::end() const
    {
        BOOST_ASSERT(get_type() == type::list_type);
        return const_iterator(l.last);
    }

    inline bool utree::empty() const
    {
        if (get_type() == type::list_type)
            return l.size == 0;
        BOOST_ASSERT(get_type() == type::nil_type);
        return true;
    }

    inline std::size_t utree::size() const
    {
        if (get_type() == type::list_type)
            return l.size;
        BOOST_ASSERT(get_type() == type::nil_type);
        return 0;
    }

    inline utree& utree::front()
    {
        BOOST_ASSERT(get_type() == type::list_type && l.first != 0);
        return l.first->val;
    }

    inline utree& utree::back()
    {
        BOOST_ASSERT(get_type() == type::list_type && l.last != 0);
        return l.last->val;
    }

    inline utree const& utree::front() const
    {
        BOOST_ASSERT(get_type() == type::list_type && l.first != 0);
        return l.first->val;
    }

    inline utree const& utree::back() const
    {
        BOOST_ASSERT(get_type() == type::list_type && l.last != 0);
        return l.last->val;
    }

    inline void utree::swap(utree& other)
    {
        s.swap(other.s);
    }

    inline utree::type::info utree::get_type() const
    {
        // the fast string holds the type info
        return s.get_type();
    }

    inline void utree::set_type(type::info t)
    {
        // the fast string holds the type info
        s.set_type(t);
    }

    inline void utree::ensure_list_type()
    {
        if (get_type() == type::nil_type)
        {
            set_type(type::list_type);
            l.default_construct();
        }
        else
        {
            BOOST_ASSERT(get_type() == type::list_type);
        }
    }

    inline void utree::free()
    {
        switch (get_type())
        {
            case type::heap_string_type:
                s.free();
                break;
            case type::list_type:
                l.free();
                break;
            default:
                break;
        };
    }

    inline void utree::copy(utree const& other)
    {
        set_type(other.get_type());
        switch (other.get_type())
        {
            case type::nil_type:
                break;
            case type::bool_type:
                b = other.b;
                break;
            case type::int_type:
                i = other.i;
                break;
            case type::double_type:
                d = other.d;
                break;
            case type::small_string_type:
            case type::heap_string_type:
                s.copy(other.s);
                break;
            case type::list_type:
                l.copy(other.l);
                break;
        }
    }

    inline utree::utree(construct_list)
    {
        l.default_construct();
        set_type(type::list_type);
    }
}

#endif
