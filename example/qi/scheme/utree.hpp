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

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804) // unsafe mix of type 'const bool' and type 'const int' in operation
# pragma warning(disable: 4805) // unsafe mix of type 'const bool' and type 'const int' in operation
#endif

namespace scheme { namespace detail
{
    template <typename UTreeX, typename UTreeY>
    struct visit_impl;
    struct index_impl;

    // Our utree can store these types. This enum tells us what type
    // of data is stored in the discriminated union.
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

    // Our POD fast string. This implementation is very primitive and is not
    // meant to be used stand-alone. This is the internal data representation
    // of strings in our utree. This is deliberately a POD to allow it to be
    // placed in a union. This POD fast string specifically utilizes
    // (sizeof(double) * 2) - 2 * sizeof(char) (14 bytes in a 32 bit system).
    // Two extra bytes are used by utree to store management info.
    //
    // It is a const string (i.e. immutable). It stores the characters directly
    // if possible and only uses the heap if the string does not fit. Null
    // characters are allowed, making it suitable to encode raw binary. The
    // string length is encoded in the first byte if the string is placed in-situ,
    // else, the length plus a pointer to the string in the heap are stored.

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

        utree_type::info get_type() const
        {
            return static_cast<utree_type::info>(buff[small_string_size]);
        }

        void set_type(utree_type::info t)
        {
            buff[small_string_size] = static_cast<char>(t);
        }

        std::size_t size() const
        {
            BOOST_ASSERT(get_type() == utree_type::small_string_type
                || get_type() == utree_type::heap_string_type);

            if (get_type() == utree_type::small_string_type)
                return this->buff[0];
            else
                return this->heap.size;
        }

        char const* str() const
        {
            BOOST_ASSERT(get_type() == utree_type::small_string_type
                || get_type() == utree_type::heap_string_type);

            if (get_type() == utree_type::small_string_type)
                return this->buff + 1;
            else
                return this->heap.str;
        }

        template <typename Iterator>
        void construct(Iterator f, Iterator l)
        {
            unsigned const size = (l-f)+1;
            char* str;
            if (size <= small_string_size)
            {
                // if it fits, store it in-situ; the first byte
                // is the length of the string.
                str = this->buff + 1;
                this->buff[0] = size;
                set_type(utree_type::small_string_type);
            }
            else
            {
                // else, store it in the heap
                str = new char[size];
                this->heap.str = str;
                this->heap.size = size;
                set_type(utree_type::heap_string_type);
            }
            for (int i = 0; i != size; ++i)
                *str++ = *f++;
        }

        void swap(fast_string& other)
        {
            fast_string temp = other;
            other = *this;
            *this = temp;
        }

        void free()
        {
            if (get_type() == utree_type::heap_string_type)
            {
                delete [] this->heap.str;
                this->heap.str = 0;
            }
        }

        void copy(fast_string const& other)
        {
            construct(other.str(), other.str() + other.size());
        }
    };

    // Our POD double linked list. Straightforward implementation.
    // This implementation is very primitive and is not meant to be
    // used stand-alone. This is the internal data representation
    // of lists in our utree.
    struct list // keep this a POD!
    {
        struct node;

        template <typename Value>
        class node_iterator;

        void free();
        void copy(list const& other);
        void default_construct()
        {
            this->first = this->last = 0;
            this->size = 0;
        }

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

        utree() { set_type(type::nil_type); }
        explicit utree(bool b) : b(b) { set_type(type::bool_type); }
        explicit utree(unsigned i) : i(i) { set_type(type::int_type); }
        explicit utree(int i) : i(i) { set_type(type::int_type); }
        explicit utree(double d) : d(d) { set_type(type::double_type); }
        explicit utree(char const* str) { s.construct(str, str+strlen(str)); }
        explicit utree(std::string const& str) { s.construct(str.begin(), str.end()); }

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

        utree(utree const& other) { copy(*this, other); }
        ~utree() { free(*this); }

        utree& operator=(utree const& other);

        utree& operator=(bool b);
        utree& operator=(unsigned i);
        utree& operator=(int i);
        utree& operator=(double d);
        utree& operator=(char const* s);
        utree& operator=(std::string const& s);

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

        type::info get_type() const
        {
            // the fast string holds the type info
            return s.get_type();
        }

        void set_type(type::info t)
        {
            // the fast string holds the type info
            s.set_type(t);
        }

        void ensure_list_type();

        static void free(utree& x);
        static void copy(utree& x, utree const& other);

        struct construct_list {};

        utree(construct_list)
        {
            l.default_construct();
            set_type(type::list_type);
        }

        union
        {
            detail::fast_string s;
            detail::list l;
            bool b;
            int i;
            double d;
        };
    };

    std::ostream& operator<<(std::ostream& out, utree const& val);

    bool operator==(utree const& a, utree const& b);
    bool operator<(utree const& a, utree const& b);
    std::ostream& operator<<(std::ostream& out, utree const& val);

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

    struct ulist : utree
    {
        ulist() : utree(construct_list()) {}
    };
}

namespace scheme { namespace detail
{
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
        friend class utree;

        void increment() { node = node->next; }
        void decrement() { node = node->prev; }

        bool equal(node_iterator const& other) const
        {
            return this->node == other.node;
        }

        utree& dereference() const { return node->val; }
        list::node* node;
    };

    inline void list::free()
    {
        node* p = this->first;
        while (p != this->last)
        {
            node* next = p->next;
            delete p;
            p = next;
        }
        this->first = this->last = 0;
        this->size = 0;
    }

    inline void list::copy(list const& other)
    {
        this->first = this->last = 0;
        node* p = other.first;
        while (p != 0)
        {
            this->push_back(p->val);
            p = p->next;
        }
    }

    template <typename T>
    inline void list::insert_before(T const& val, node* np)
    {
        node* new_node = new node(val, np, np->prev);
        if (np->prev)
            np->prev->next = new_node;
        else
            this->first = new_node;
        np->prev = new_node;
        ++this->size;
    }

    template <typename T>
    inline void list::insert_after(T const& val, node* np)
    {
        node* new_node = new node(val, np->next, np);
        if (np->next)
            np->next->prev = new_node;
        else
            this->last = new_node;
        np->next = new_node;
        ++this->size;
    }

    template <typename T>
    inline void list::push_front(T const& val)
    {
        detail::list::node* new_node;
        if (this->first == 0)
        {
            new_node = new detail::list::node(val, 0, 0);
            this->first = this->last = new_node;
            ++this->size;
        }
        else
        {
             this->insert_before(val, this->first);
        }
    }

    template <typename T>
    inline void list::push_back(T const& val)
    {
        if (this->last == 0)
            this->push_front(val);
        else
            this->insert_after(val, this->last);
    }

    inline void list::pop_front()
    {
        node* np = this->first;
        this->first = this->first->next;
        this->first->prev = 0;
        delete np;
        --this->size;
    }

    inline void list::pop_back()
    {
        node* np = this->last;
        this->last = this->last->prev;
        this->last->next = 0;
        delete np;
        --this->size;
    }

    inline list::node* list::erase(node* pos)
    {
        if (pos == this->first)
        {
            this->pop_front();
            return this->first;
        }
        else if (pos == this->last)
        {
            this->pop_back();
            return 0;
        }
        else
        {
            node* next(pos->next);
            pos->unlink();
            delete pos;
            --this->size;
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

    struct utree_print
    {
        typedef void result_type;
        std::ostream& out;
        utree_print(std::ostream& out)
          : out(out) {}

        void operator()(utree::nil) const
        {
            out << "nil";
        }

        template <typename T>
        void operator()(T val) const
        {
            out << val;
        }

        void operator()(bool b) const
        {
            out << (b?"true":"false");
        }

        void operator()(char const* s) const
        {
            out << '"' << s << '"';
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            out << '[';
            for (typename boost::iterator_range<Iterator>::const_iterator
                i = range.begin(); i != range.end(); ++i)
            {
                if (i != range.begin())
                    out << ',';
                out << *i;
            }
            out << ']';
        }
    };

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
            return a == b;
        }

        bool operator()(utree::nil, utree::nil) const
        {
            return true;
        }

        bool operator()(char const* a, char const* b) const
        {
            return strcmp(a, b) == 0; // $$$ use utf8 comparison here! $$$
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
            return a < b;
        }

        bool operator()(utree::nil, utree::nil) const
        {
            BOOST_ASSERT(false);
            return false; // no less than comparison for nil
        }

        bool operator()(char const* a, char const* b) const
        {
            return strcmp(a, b) < 0; // $$$ use utf8 comparison here! $$$
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
            typedef detail::utree_type type;

            switch (x.get_type())
            {
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
                default:
                    return f(string_type(x.s.str()));
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
            typedef detail::utree_type type;

            switch (x.get_type())
            {
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
                default:
                    return visit_impl::apply(y, detail::bind(f, string_type(x.s.str())));
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
    inline utree& utree::operator=(utree const& other)
    {
        if (this != &other)
        {
            free(*this);
            copy(*this, other);
        }
        return *this;
    }

    inline utree& utree::operator=(bool b_)
    {
        free(*this);
        b = b_;
        set_type(type::bool_type);
        return *this;
    }

    inline utree& utree::operator=(unsigned i_)
    {
        free(*this);
        i = i_;
        set_type(type::int_type);
        return *this;
    }

    inline utree& utree::operator=(int i_)
    {
        free(*this);
        i = i_;
        set_type(type::int_type);
        return *this;
    }

    inline utree& utree::operator=(double d_)
    {
        free(*this);
        d = d_;
        set_type(type::double_type);
        return *this;
    }

    inline utree& utree::operator=(char const* s)
    {
        free(*this);
        this->s.construct(s, s + strlen(s));
        return *this;
    }

    inline utree& utree::operator=(std::string const& s)
    {
        free(*this);
        this->s.construct(s.begin(), s.end());
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
        BOOST_ASSERT(this->get_type() == type::list_type && this->size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    inline utree const& utree::operator[](std::size_t i) const
    {
        BOOST_ASSERT(this->get_type() == type::list_type && this->size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    inline std::ostream& operator<<(std::ostream& out, utree const& val)
    {
        utree::visit(val, detail::utree_print(out));
        return out;
    }

    inline bool operator==(utree const& a, utree const& b)
    {
        return utree::visit(a, b, detail::utree_is_equal());
    }

    inline bool operator<(utree const& a, utree const& b)
    {
        return utree::visit(a, b, detail::utree_is_less_than());
    }

    inline void utree::ensure_list_type()
    {
        if (this->get_type() == type::nil_type)
        {
            this->set_type(type::list_type);
            this->l.first = this->l.last = 0;
            this->l.size = 0;
        }
        else
        {
            BOOST_ASSERT(this->get_type() == type::list_type);
        }
    }

    template <typename T>
    inline void utree::push_front(T const& val)
    {
        ensure_list_type();
        this->l.push_front(val);
    }

    template <typename T>
    inline void utree::push_back(T const& val)
    {
        ensure_list_type();
        this->l.push_back(val);
    }

    template <typename T>
    inline utree::iterator utree::insert(iterator pos, T const& val)
    {
        ensure_list_type();
        this->l.insert_before(val, pos.node);
        return utree::iterator(pos.node->prev);
    }


    template <typename T>
    inline void utree::insert(iterator pos, std::size_t n, T const& val)
    {
        for (std::size_t i = 0; i != n; ++i)
            this->insert(pos, val);
    }

    template <typename Iter>
    inline void utree::insert(iterator pos, Iter first, Iter last)
    {
        ensure_list_type();
        while (first != last)
            this->l.insert_before(*first++, pos.node);
    }

    template <typename Iter>
    inline void utree::assign(Iter first, Iter last)
    {
        ensure_list_type();
        clear();
        while (first != last)
            this->push_back(*first++);
    }

    inline void utree::clear()
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        free(*this);
    }

    inline void utree::pop_front()
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        this->l.pop_front();
    }

    inline void utree::pop_back()
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        this->l.pop_back();
    }

    inline utree::iterator utree::erase(iterator pos)
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return iterator(this->l.erase(pos.node));
    }

    inline utree::iterator utree::erase(iterator first, iterator last)
    {
        while (first != last)
            this->erase(first++);
        return last;
    }

    inline utree::iterator utree::begin()
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return iterator(l.first);
    }

    inline utree::iterator utree::end()
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return iterator(l.last);
    }

    inline utree::const_iterator utree::begin() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return const_iterator(l.first);
    }

    inline utree::const_iterator utree::end() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return const_iterator(l.last);
    }

    inline bool utree::empty() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return l.first == l.last;
    }

    inline std::size_t utree::size() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type);
        return this->l.size;
    }

    inline utree& utree::front()
    {
        BOOST_ASSERT(this->get_type() == type::list_type && this->l.first != 0);
        return this->l.first->val;
    }

    inline utree& utree::back()
    {
        BOOST_ASSERT(this->get_type() == type::list_type && this->l.last != 0);
        return this->l.last->val;
    }

    inline utree const& utree::front() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type && this->l.first != 0);
        return this->l.first->val;
    }

    inline utree const& utree::back() const
    {
        BOOST_ASSERT(this->get_type() == type::list_type && this->l.last != 0);
        return this->l.last->val;
    }


    inline void utree::free(utree& x)
    {
        switch (x.get_type())
        {
            default:
                break;
            case type::heap_string_type:
                x.s.free();
                break;
            case type::list_type:
                x.l.free();
                break;
        };
    }

    inline void utree::copy(utree& x, utree const& other)
    {
        x.set_type(other.get_type());
        switch (other.get_type())
        {
            case type::nil_type:
                break;
            case type::bool_type:
                x.b = other.b;
                break;
            case type::int_type:
                x.i = other.i;
                break;
            case type::double_type:
                x.d = other.d;
                break;
            case type::small_string_type:
            case type::heap_string_type:
                x.s.copy(other.s);
                break;
            case type::list_type:
                x.l.copy(other.l);
                break;
        }
    }

    inline void utree::swap(utree& other)
    {
        this->s.swap(other.s);
    }
}

#endif
