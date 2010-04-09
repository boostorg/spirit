/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE_DETAIL2)
#define BOOST_SPIRIT_UTREE_DETAIL2

namespace scheme { namespace detail
{
    inline char& fast_string::info()
    {
        return buff[small_string_size];
    }

    inline char fast_string::info() const
    {
        return buff[small_string_size];
    }

    inline int fast_string::get_type() const
    {
        return info() >> 1;
    }

    inline void fast_string::set_type(int t)
    {
        info() = (t << 1) | (info() & 1);
    }

    inline bool fast_string::is_heap_allocated() const
    {
        return info() & 1;
    }

    inline std::size_t fast_string::size() const
    {
        if (is_heap_allocated())
            return heap.size;
        else
            return max_string_len - buff[small_string_size - 1];
    }

    inline char const* fast_string::str() const
    {
        if (is_heap_allocated())
            return heap.str;
        else
            return buff;
    }

    template <typename Iterator>
    inline void fast_string::construct(Iterator f, Iterator l)
    {
        unsigned const size = l-f;
        char* str;
        if (size < small_string_size)
        {
            // if it fits, store it in-situ; small_string_size minus the length
            // of the string is placed in buff[small_string_size - 1]
            str = buff;
            buff[small_string_size - 1] = max_string_len - size;
            info() &= ~0x1;
        }
        else
        {
            // else, store it in the heap
            str = new char[size + 1]; // add one for the null char
            heap.str = str;
            heap.size = size;
            info() |= 0x1;
        }
        for (std::size_t i = 0; i != size; ++i)
        {
            *str++ = *f++;
        }
        *str = '\0'; // add the null char
    }

    inline void fast_string::swap(fast_string& other)
    {
        std::swap(*this, other);
    }

    inline void fast_string::free()
    {
        if (is_heap_allocated())
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

        explicit node_iterator(list::node* node)
          : node(node), prev(node->prev) {}

        node_iterator(list::node* node, list::node* prev)
          : node(node), prev(prev) {}

    private:

        friend class boost::iterator_core_access;
        friend class scheme::utree;

        void increment()
        {
            if (node != 0) // not at end
            {
                prev = node;
                node = node->next;
            }
        }

        void decrement()
        {
            if (prev != 0) // not at begin
            {
                node = prev;
                prev = prev->prev;
            }
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
        list::node* prev;
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

    template <typename UTreeX, typename UTreeY = UTreeX>
    struct visit_impl
    {
        template <typename F>
        typename F::result_type
        static apply(UTreeX& x, F f) // single dispatch
        {
            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>,
                typename UTreeX::const_iterator,
                typename UTreeX::iterator>::type
            iterator;

            typedef boost::iterator_range<iterator> list_range;
            typedef utree_type type;

            switch (x.get_type())
            {
                default:
                    BOOST_ASSERT(false); // can't happen

                case type::nil_type:
                    nil arg;
                    return f(arg);

                case type::bool_type:
                    return f(x.b);

                case type::int_type:
                    return f(x.i);

                case type::double_type:
                    return f(x.d);

                case type::list_type:
                    return f(list_range(iterator(x.l.first), iterator(0, x.l.last)));

                case type::string_type:
                    return f(utf8_string_range(x.s.str(), x.s.size()));

                case type::symbol_type:
                    return f(utf8_symbol_range(x.s.str(), x.s.size()));

                case type::binary_type:
                    return f(binary_range(x.s.str(), x.s.size()));

                case type::reference_type:
                    return apply(*x.p, f);

                case type::function_type:
                    return f(x.f);
            }
        }

        template <typename F>
        typename F::result_type
        static apply(UTreeX& x, UTreeY& y, F f) // double dispatch
        {
            typedef typename
                boost::mpl::if_<boost::is_const<UTreeX>,
                typename UTreeX::const_iterator,
                typename UTreeX::iterator>::type
            iterator;

            typedef boost::iterator_range<iterator> list_range;
            typedef utree_type type;

            switch (x.get_type())
            {
                default:
                    BOOST_ASSERT(false); // can't happen

                case type::nil_type:
                    nil x_;
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
                        list_range(iterator(x.l.first), iterator(0, x.l.last))));

                case type::string_type:
                    return visit_impl::apply(y, detail::bind(
                        f, utf8_string_range(x.s.str(), x.s.size())));

                case type::symbol_type:
                    return visit_impl::apply(y, detail::bind(
                        f, utf8_symbol_range(x.s.str(), x.s.size())));

                case type::binary_type:
                    return visit_impl::apply(y, detail::bind(
                        f, binary_range(x.s.str(), x.s.size())));

                case type::reference_type:
                    return apply(*x.p, y, f);

                case type::function_type:
                    return visit_impl::apply(y, detail::bind(f, x.f));
            }
        }
    };

    struct index_impl
    {
        static utree& apply(list::node* node, std::size_t i)
        {
            for (; i > 0; --i)
                node = node->next;
            return node->val;
        }

        static utree const& apply(list::node const* node, std::size_t i)
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
        set_type(type::string_type);
    }

    inline utree::utree(char const* str, std::size_t len)
    {
        s.construct(str, str + len);
        set_type(type::string_type);
    }

    inline utree::utree(std::string const& str)
    {
        s.construct(str.begin(), str.end());
        set_type(type::string_type);
    }

    template <typename Base, utree_type::info type_>
    inline utree::utree(basic_string<Base, type_> const& bin)
    {
        s.construct(bin.begin(), bin.end());
        set_type(type_);
    }

    inline utree::utree(boost::reference_wrapper<utree> ref)
      : p(ref.get_pointer())
    {
        set_type(type::reference_type);
    }

    inline utree::utree(function_ptr fptr)
      : f(fptr)
    {
        set_type(type::function_type);
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
        set_type(type::string_type);
        return *this;
    }

    inline utree& utree::operator=(std::string const& s_)
    {
        free();
        s.construct(s_.begin(), s_.end());
        set_type(type::string_type);
        return *this;
    }

    template <typename Base, utree_type::info type_>
    inline utree& utree::operator=(basic_string<Base, type_> const& bin)
    {
        free();
        s.construct(bin.begin(), bin.end());
        set_type(type_);
        return *this;
    }

    inline utree& utree::operator=(boost::reference_wrapper<utree> ref)
    {
        free();
        p = ref.get_pointer();
        set_type(type::reference_type);
        return *this;
    }

    inline utree& utree::operator=(function_ptr fptr)
    {
        free();
        f = fptr;
        set_type(type::function_type);
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
        if (get_type() == type::reference_type)
            return (*p)[i];
        BOOST_ASSERT(get_type() == type::list_type && size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    inline utree const& utree::operator[](std::size_t i) const
    {
        if (get_type() == type::reference_type)
            return (*(utree const*)p)[i];
        BOOST_ASSERT(get_type() == type::list_type && size() > i);
        return detail::index_impl::apply(l.first, i);
    }

    template <typename T>
    inline void utree::push_front(T const& val)
    {
        if (get_type() == type::reference_type)
            return p->push_front(val);
        ensure_list_type();
        l.push_front(val);
    }

    template <typename T>
    inline void utree::push_back(T const& val)
    {
        if (get_type() == type::reference_type)
            return p->push_back(val);
        ensure_list_type();
        l.push_back(val);
    }

    template <typename T>
    inline utree::iterator utree::insert(iterator pos, T const& val)
    {
        if (get_type() == type::reference_type)
            return p->insert(pos, val);
        ensure_list_type();
        if (pos == end())
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
        if (get_type() == type::reference_type)
            return p->insert(pos, n, val);
        for (std::size_t i = 0; i != n; ++i)
            insert(pos, val);
    }

    template <typename Iter>
    inline void utree::insert(iterator pos, Iter first, Iter last)
    {
        if (get_type() == type::reference_type)
            return p->insert(pos, first, last);
        ensure_list_type();
        while (first != last)
            insert(pos, *first++);
    }

    template <typename Iter>
    inline void utree::assign(Iter first, Iter last)
    {
        if (get_type() == type::reference_type)
            return p->assign(first, last);
        ensure_list_type();
        clear();
        while (first != last)
            push_back(*first++);
    }

    inline void utree::clear()
    {
        if (get_type() == type::reference_type)
            return p->clear();
        // clear will always make this a nil type
        free();
        set_type(type::nil_type);
    }

    inline void utree::pop_front()
    {
        if (get_type() == type::reference_type)
            return p->pop_front();
        BOOST_ASSERT(get_type() == type::list_type);
        l.pop_front();
    }

    inline void utree::pop_back()
    {
        if (get_type() == type::reference_type)
            return p->pop_back();
        BOOST_ASSERT(get_type() == type::list_type);
        l.pop_back();
    }

    inline utree::iterator utree::erase(iterator pos)
    {
        if (get_type() == type::reference_type)
            return p->erase(pos);
        BOOST_ASSERT(get_type() == type::list_type);
        detail::list::node* np = l.erase(pos.node);
        return iterator(np, np?np->prev:l.last);
    }

    inline utree::iterator utree::erase(iterator first, iterator last)
    {
        if (get_type() == type::reference_type)
            return p->erase(first, last);
        while (first != last)
            erase(first++);
        return last;
    }

    inline utree::iterator utree::begin()
    {
        if (get_type() == type::reference_type)
            return p->begin();
        ensure_list_type();
        return iterator(l.first);
    }

    inline utree::iterator utree::end()
    {
        if (get_type() == type::reference_type)
            return p->end();
        ensure_list_type();
        return iterator(0, l.last);
    }

    inline utree::const_iterator utree::begin() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->begin();
        BOOST_ASSERT(get_type() == type::list_type);
        return const_iterator(l.first);
    }

    inline utree::const_iterator utree::end() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->end();
        BOOST_ASSERT(get_type() == type::list_type);
        return const_iterator(0, l.last);
    }

    inline bool utree::empty() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->empty();
        if (get_type() == type::list_type)
            return l.size == 0;
        BOOST_ASSERT(get_type() == type::nil_type);
        return true;
    }

    inline std::size_t utree::size() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->size();
        if (get_type() == type::list_type)
            return l.size;
        BOOST_ASSERT(get_type() == type::nil_type);
        return 0;
    }

    inline utree_type::info utree::which() const
    {
        return get_type();
    }

    inline utree& utree::front()
    {
        if (get_type() == type::reference_type)
            return p->front();
        BOOST_ASSERT(get_type() == type::list_type && l.first != 0);
        return l.first->val;
    }

    inline utree& utree::back()
    {
        if (get_type() == type::reference_type)
            return p->back();
        BOOST_ASSERT(get_type() == type::list_type && l.last != 0);
        return l.last->val;
    }

    inline utree const& utree::front() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->front();
        BOOST_ASSERT(get_type() == type::list_type && l.first != 0);
        return l.first->val;
    }

    inline utree const& utree::back() const
    {
        if (get_type() == type::reference_type)
            return ((utree const*)p)->back();
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
        return static_cast<utree::type::info>(s.get_type());
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
            case type::binary_type:
            case type::symbol_type:
            case type::string_type:
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
            case type::reference_type:
                p = other.p;
                break;
            case type::string_type:
            case type::symbol_type:
            case type::binary_type:
                s.copy(other.s);
                break;
            case type::list_type:
                l.copy(other.l);
                break;
        }
    }

    template <typename To>
    struct utree_cast
    {
        typedef To result_type;

        template <typename From>
        To dispatch(From const& val, boost::mpl::true_) const
        {
            return To(val); // From is convertible to To
        }

        template <typename From>
        To dispatch(From const& val, boost::mpl::false_) const
        {
            // From is NOT convertible to To !!!
            BOOST_ASSERT(false);
            return To();
        }

        template <typename From>
        To operator()(From const& val) const
        {
            return dispatch(val, boost::is_convertible<From, To>());
        }
    };

    template <typename T>
    inline T utree::as() const
    {
        return utree::visit(*this, utree_cast<T>());
    }
}

#endif
