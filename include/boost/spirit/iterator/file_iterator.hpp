/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2003 Giovanni Bajo
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
//
//  File Iterator structure
//
//  The new structure is designed on layers. The top class (used by the user)
//  is file_iterator, which implements a full random access iterator through
//  the file, and some specific member functions (constructor that opens
//  the file, make_end() to generate the end iterator, operator bool to check
//  if the file was opened correctly).
//
//  file_iterator implements the random access iterator interface by the means
//  of boost::iterator_adaptor, that is inhering an object created with it.
//  iterator_adaptor gets a low-level file iterator implementation (with just
//  a few member functions) and a policy (that basically describes to it how
//  the low-level file iterator interface is). The advantage is that
//  with boost::iterator_adaptor only 5 functions are needed to implement
//  a fully conformant random access iterator, instead of dozens of functions
//  and operators.
//
//  There are two low-level file iterators implemented in this module. The 
//  first (std_file_iterator) uses cstdio stream functions (fopen/fread), which 
//  support full buffering, and is available everywhere (it's standard C++).
//  The second (mmap_file_iterator) is currently available only on Windows 
//  platforms, and uses memory mapped files, which gives a decent speed boost.
//
///////////////////////////////////////////////////////////////////////////////
//
//  TODO LIST:
//
//  - In the Win32 mmap iterator, we could check if keeping a handle to the 
//    opened file is really required. If it's not, we can just store the file 
//    length (for make_end()) and save performance. Notice that this should be 
//    tested under different Windows versions, the behaviour might change.
//  - Add some error support (by the means of some exceptions) in case of
//    low-level I/O failure.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_SPIRIT_FILE_ITERATOR_HPP
#define BOOST_SPIRIT_FILE_ITERATOR_HPP

#include <string>
#include <boost/config.hpp>
#include <boost/iterator_adaptors.hpp>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) \
	&& !defined(BOOST_DISABLE_WIN32)
#define BOOST_SPIRIT_FILEITERATOR_WINDOWS
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
namespace fileiter_impl {

    // Forward declarations
    template <typename CharT = char>
    struct std_file_iterator;
    
#ifdef BOOST_SPIRIT_FILEITERATOR_WINDOWS
    template <typename CharT = char>
    class mmap_file_iterator;
#endif

    template <typename BaseT>
    struct file_iterator_policy;

    /////////////////////////////////////////////////////////////////////////
    //
    //  file_iter_generator
    //
    //  Template meta-function to invoke boost::iterator_adaptor
    //  NOTE: This cannot be moved into the implementation file because of
    //  a bug of MSVC 7.0 and previous versions (base classes types are
    //  looked up at compilation time, not instantion types, and 
    //  file_iterator would break).
    //
    /////////////////////////////////////////////////////////////////////////

    template <typename BaseIteratorT>
    struct file_iter_generator
    {
    public:
        typedef BaseIteratorT adapted_t;
        typedef typename adapted_t::value_type value_type;
        typedef file_iterator_policy<adapted_t> policy_t;

        typedef boost::iterator_adaptor
        <
            adapted_t,
            policy_t,
            const value_type, const value_type&, const value_type*,
            std::random_access_iterator_tag,
            signed long
        > type;
    };

///////////////////////////////////////////////////////////////////////////////
} /* namespace impl */


///////////////////////////////////////////////////////////////////////////////
//
//  file_iterator
//
//  Iterates through an opened file.
//
//  The main iterator interface is implemented by the iterator_adaptors
//  library, which wraps a conforming iterator interface around the
//  impl::BaseIterator class. This class merely derives the iterator_adaptors
//  generated class to implement the custom constructors and make_end()
//  member function.
//
///////////////////////////////////////////////////////////////////////////////

template 
<
    typename CharT = char, 
    typename BaseIteratorT = 
#ifndef BOOST_SPIRIT_FILEITERATOR_WINDOWS
        fileiter_impl::std_file_iterator<CharT> 
#else
        fileiter_impl::mmap_file_iterator<CharT> 
#endif
>
class file_iterator
    : public fileiter_impl::file_iter_generator<BaseIteratorT>::type
{
private:
    typedef typename 
        fileiter_impl::file_iter_generator<BaseIteratorT>::type base_t;
    typedef typename 
        fileiter_impl::file_iter_generator<BaseIteratorT>::adapted_t adapted_t;

public:
    file_iterator(std::string fileName)
        : base_t(adapted_t(fileName))
    {}

    file_iterator(const base_t& iter)
        : base_t(iter)
    {}

    inline file_iterator& operator=(const base_t& iter);
    file_iterator make_end(void);

    // operator bool. This borrows a trick from boost::shared_ptr to avoid
    //   to interfere with arithmetic operations. The dummy member function
    //   is needed because there are no other normal member function around
    //   (make_end() confuses GCC because of its return type)
    //   (NOTE: The bug has been submitted and it's fixed in 3.4)
    typedef void (file_iterator::*bool_wrap)(void);

    operator bool_wrap()
    { return (this->base() ? &file_iterator::dummy : 0); }

private:
    void dummy(void) {}
};


///////////////////////////////////////////////////////////////////////////////
}} /* namespace boost::spirit */


///////////////////////////////////////////////////////////////////////////////
#include "impl/file_iterator.ipp"  /* implementation */

#endif /* BOOST_SPIRIT_FILE_ITERATOR_HPP */

