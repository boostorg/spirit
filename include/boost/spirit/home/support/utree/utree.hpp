/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2010      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE)
#define BOOST_SPIRIT_UTREE

#include <cstddef>
#include <algorithm>
#include <string>
#include <ostream>
#include <typeinfo>

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/ref.hpp>

#include <boost/spirit/home/support/utree/detail/utree_detail1.hpp>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804)
# pragma warning(disable: 4805)
# pragma warning(disable: 4244)
#endif

namespace boost { namespace spirit
{
    //[utree_exceptions
    // All exceptions thrown by utree are derived from utree_exception.
    struct utree_exception : std::exception {};

    // bad_type_exception is thrown whenever somebody calls a member function
    // which applies to certain stored utree_type's only, but this precondition
    // is violated as the utree instance holds some other type.
    struct bad_type_exception : utree_exception
    {
        virtual const char* what() const throw()
        {
            return "utree: Illegal operation for currently stored data.";
        }
    };
    //]

    //[utree_types
    struct utree_type
    {
        enum info
        {
            nil_type,       // nil is the sentinel utree type.
            list_type,      // A doubly linked list of utrees.
            range_type,     // A range of list::iterators. 
            reference_type, // A reference to another utree.
            any_type,       // A pointer or reference to any C++ type. 
            function_type,  // A utree holding a stored_function<F> object,
                            // where F is an unary function object taking a 
                            // scope as it's parameter and returning a
                            // utree.

            // numeric atoms
            bool_type,
            int_type,
            double_type,

            // text atoms (utf8)
            string_type, 
            string_range_type,
            symbol_type,
            binary_type 
        };
    };
    //]

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

    //[utree_strings
    // Binary string
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::binary_type>
      binary_range;
    typedef basic_string<
        std::string,
        utree_type::binary_type>
     binary_string;

    // UTF-8 string
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::string_type>
      utf8_string_range;
    typedef basic_string<
        std::string,
        utree_type::string_type>
      utf8_string_type;

    // UTF-8 symbol
    typedef basic_string<
        boost::iterator_range<char const*>,
        utree_type::symbol_type>
      utf8_symbol_range;
    typedef basic_string<
        std::string,
        utree_type::symbol_type>
      utf8_symbol;
    //]

    ///////////////////////////////////////////////////////////////////////////
    // Our function type
    ///////////////////////////////////////////////////////////////////////////
    class utree;
    class scope;

    //[utree_function_object_interface
    struct function_base
    {
        virtual ~function_base() {};
        virtual utree operator()(scope const& env) const = 0;
        virtual function_base* clone() const = 0; // Calling f.clone() must
                                                  // return a newly allocated
                                                  // function_base instance
                                                  // that is equal to f.
    };

    template <typename F>
    struct stored_function : function_base
    {
        F f;
        stored_function(F f = F());
        virtual ~stored_function();
        virtual utree operator()(scope const& env) const;
        virtual function_base* clone() const;
    };
    //]

    ///////////////////////////////////////////////////////////////////////////
    // Shallow tag. Instructs utree to hold an iterator_range
    // as-is without deep copying the range.
    ///////////////////////////////////////////////////////////////////////////
    struct shallow_tag {};
    shallow_tag const shallow = {};

    ///////////////////////////////////////////////////////////////////////////
    // A void* plus type_info
    ///////////////////////////////////////////////////////////////////////////
    class any_ptr
    {
    public:

        template <typename Ptr>
        typename boost::disable_if<
            boost::is_polymorphic<
                typename boost::remove_pointer<Ptr>::type>,
            Ptr>::type
        get() const
        {
            if (*i == typeid(Ptr))
            {
                return static_cast<Ptr>(p);
            }
            throw std::bad_cast();
        }

        template <typename T>
        any_ptr(T* p)
          : p(p), i(&typeid(T*))
        {}

        friend bool operator==(any_ptr const& a, any_ptr const& b)
        {
            return (a.p == b.p) && (*a.i == *b.i);
        }

    private:

        // constructor is private
        any_ptr(void* p, std::type_info const* i)
          : p(p), i(i) {}

        template <typename UTreeX, typename UTreeY>
        friend struct detail::visit_impl;

        friend class utree;

        void* p;
        std::type_info const* i;
    };

    //[utree
    class utree {
    public:
        typedef utree value_type;
        typedef utree& reference;
        typedef utree const& const_reference;
        typedef std::ptrdiff_t difference_type;
        typedef std::size_t size_type;

        typedef detail::list::node_iterator<utree>
          iterator;
        typedef detail::list::node_iterator<utree const>
          const_iterator;
        typedef detail::list::node_iterator<boost::reference_wrapper<utree> >
          ref_iterator;

        typedef boost::iterator_range<iterator> range;
        typedef boost::iterator_range<const_iterator> const_range;

        // dtor
        ~utree();

        ////////////////////////////////////////////////////////////////////////
        // ctors and assignment operators

        // nil 
        utree();

        // boolean 
        utree(bool);
        reference operator=(bool);

        // integer 
        utree(unsigned int);
        utree(int);
        reference operator=(unsigned int);
        reference operator=(int);

        // double 
        utree(double);
        reference operator=(double);

        // string 
        utree(char);
        utree(char const*);
        utree(char const*, std::size_t);
        utree(std::string const&);
        utree(utf8_string_range const&, shallow_tag);
        reference operator=(char);
        reference operator=(char const*);
        reference operator=(std::string const&);

        // reference 
        utree(boost::reference_wrapper<utree>);
        reference operator=(boost::reference_wrapper<utree>);

        // any 
        utree(any_ptr const&);
        reference operator=(any_ptr const&);

        // range 
        template <class Iter>
          utree(boost::iterator_range<Iter>);
        template <class Iter>
          reference operator=(boost::iterator_range<Iter>);

        // function object 
        template <class F>
          utree(stored_function<F> const&);
        template <class F>
          reference operator=(stored_function<F> const&);

        // typed string
        template <class Base, utree_type::info type_>
          utree(basic_string<Base, type_> const&);
        template <class Base, utree_type::info type_>
          reference operator=(basic_string<Base, type_> const&);

        // copy 
        utree(const_reference);
        reference operator=(const_reference);

        // range
        utree(range, shallow_tag);
        utree(const_range, shallow_tag);

        // assign dispatch
        template <class Iter>
          void assign(Iter, Iter);

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // function object visitation interface

        // single dispatch
        template <class F>
          typename boost::result_of<F(utree const&)>::type
          static visit(utree const&, F);

        template <class F>
          typename boost::result_of<F(utree&)>::type
          static visit(utree&, F);

        // double dispatch
        template <class F>
          typename boost::result_of<F(utree const&, utree const&)>::type
          static visit(utree const&, utree const&, F);

        template <class F>
          typename boost::result_of<F(utree&, utree const&)>::type
          static visit(utree&, utree const&, F);

        template <class F>
          typename boost::result_of<F(utree const&, utree&)>::type
          static visit(utree const&, utree&, F);

        template <class F>
          typename boost::result_of<F(utree&, utree&)>::type
          static visit(utree&, utree&, F);

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // STL Container interface

        // insertion 
        template <class T>
          void push_back(T const&);
        template <class T>
          void push_front(T const&);
        template <class T>
          iterator insert(iterator, T const&);
        template <class T>
          void insert(iterator, std::size_t, T const&);
        template <class Iter>
          void insert(iterator, Iter, Iter);

        // erasure
        void pop_front();
        void pop_back();
        iterator erase(iterator);
        iterator erase(iterator, iterator);

        // front access
        reference front();
        const_reference front() const;
        iterator begin();
        const_iterator begin() const;
        ref_iterator ref_begin();

        // back access
        reference back();
        const_reference back() const;
        iterator end();
        const_iterator end() const;
        ref_iterator ref_end();

        // random access
        reference operator[](size_type);
        const_reference operator[](size_type) const;

        void clear();

        void swap(utree&);

        bool empty() const;
        size_type size() const;

        ////////////////////////////////////////////////////////////////////////

        utree_type::info which() const;

        template <class T>
          T get() const;

        reference deref();
        const_reference deref() const;

        short tag() const;
        void tag(short);

        utree eval(scope const&) const;

    //<-
    private:
        typedef utree_type type;

        template <class UTreeX, class UTreeY>
        friend struct detail::visit_impl;
        friend struct detail::index_impl;
        friend struct detail::assign_impl;

        template <class T>
        friend struct detail::get_impl;

        type::info get_type() const;
        void set_type(type::info);
        void ensure_list_type();
        void free();
        void copy(const_reference);

        union {
            detail::fast_string s;
            detail::list l;
            detail::range r;
            detail::string_range sr;
            detail::void_ptr v;
            bool b;
            int i;
            double d;
            utree* p;
            function_base* pf;
        };
    //->
    };
    //]

    //[utree_scope
    class scope: public boost::iterator_range<utree*> {
      public:
        scope(utree* first = 0,
              utree* last = 0,
              scope const* parent = 0) 
              //<-
              : boost::iterator_range<utree*>(first, last)
              , parent(parent)
              , depth(parent? parent->depth + 1 : 0) {}
              //->

        scope const* outer() const 
        //<-
        {
            return parent;
        }
        //->

        std::size_t level() const 
        //<-
        {
            return depth;
        }
        //->

      //<-  
      private:
        scope const* parent;
        std::size_t depth;
      //->
    };
    //]
}}

#if defined(BOOST_MSVC)
  #pragma warning(pop)
#endif

#endif

