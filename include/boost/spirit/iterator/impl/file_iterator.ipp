/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2003 Giovanni Bajo
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/

#ifndef BOOST_SPIRIT_FILE_ITERATOR_IPP
#define BOOST_SPIRIT_FILE_ITERATOR_IPP

#ifdef BOOST_SPIRIT_FILEITERATOR_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <cstdio>
#include <boost/shared_ptr.hpp>

#ifdef BOOST_SPIRIT_FILEITERATOR_WINDOWS
#  include <boost/type_traits/remove_pointer.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { 

///////////////////////////////////////////////////////////////////////////////
namespace fileiter_impl {

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//  std_file_iterator
//
//  Base class that implements iteration through a file using standard C
//  stream library (fopen and friends). This class and the following are
//  the base components on which the iterator is built (through the 
//  iterator adaptor library).
//
//  The opened file stream (FILE) is held with a shared_ptr<>, whose
//  custom deleter invokes fcose(). This makes the syntax of the class
//  very easy, especially everything related to copying.
//
///////////////////////////////////////////////////////////////////////////////

template <typename CharT>
class std_file_iterator
{
public:
    typedef CharT value_type;

    explicit std_file_iterator(std::string fileName) 
    {
        FILE* f = fopen(fileName.c_str(), "rb");

        // If the file was opened, store it into
        //  the smart pointer.
        if (f)
        {
            m_file.reset(f, fclose);
            m_pos = 0;
            m_eof = false;
            update_char();
        }
    }

    std_file_iterator(const std_file_iterator& iter)
    { *this = iter; }

    std_file_iterator& operator=(const std_file_iterator& iter)
    { 
        m_file = iter.m_file; 
        m_curChar = iter.m_curChar;
        m_eof = iter.m_eof;
        m_pos = iter.m_pos;

        return *this;
    }

    // Nasty bug in Comeau up to 4.3.0.1, we need explicit boolean context
    //  for shared_ptr to evaluate correctly
    operator bool() const
    { return m_file ? true : false; }  

    bool operator==(const std_file_iterator& iter) const
    {
        return (m_file == iter.m_file) && (m_eof == iter.m_eof) &&
            (m_pos == iter.m_pos);
    }

    const CharT& get_cur_char(void) const
    {
        return m_curChar;
    }

    void prev_char(void)
    {
        m_pos -= sizeof(CharT);
        update_char();
    }

    void next_char(void)
    {
        m_pos += sizeof(CharT);
        update_char();
    }

    void seek_end(void)
    {
        fseek(m_file.get(), 0, SEEK_END);
        m_pos = ftell(m_file.get()) / sizeof(CharT);
        m_eof = true;
    }

    void advance(signed long n)
    {
        m_pos += n * sizeof(CharT);
        update_char();
    }

    long distance(const std_file_iterator& iter) const
    {
        return (long)(m_pos - iter.m_pos) / sizeof(CharT);
    }

private:
    boost::shared_ptr<FILE> m_file;
    unsigned long m_pos;
    CharT m_curChar;
    bool m_eof;

    void update_char(void)
    {
        if (ftell(m_file.get()) != m_pos)
            fseek(m_file.get(), m_pos, SEEK_SET);

        m_eof = (fread(&m_curChar, sizeof(CharT), 1, m_file.get()) < 1);
    }
};


///////////////////////////////////////////////////////////////////////////////
//
//  mmap_file_iterator
//
//  File iterator for memory mapped files, for now implemented only on Windows 
//  platforms. This class has the same interface of std_file_iterator,
//  and can be used in its place (in fact, it's the default for Windows).
//
///////////////////////////////////////////////////////////////////////////////

#ifdef BOOST_SPIRIT_FILEITERATOR_WINDOWS

template <typename CharT>
class mmap_file_iterator
{
public:
    typedef CharT value_type;

    explicit mmap_file_iterator(std::string fileName)
    {
        HANDLE hFile = ::CreateFile(
            fileName.c_str(), 
            GENERIC_READ, 
            FILE_SHARE_READ, 
            NULL, 
            OPEN_EXISTING, 
            FILE_FLAG_SEQUENTIAL_SCAN,
            NULL
        );

        if (hFile == INVALID_HANDLE_VALUE)
            return;

        HANDLE hMap = ::CreateFileMapping(
            hFile,
            NULL,
            PAGE_READONLY,
            0, 0,
            NULL
        );
            
        if (hMap == NULL)
        {
            ::CloseHandle(hFile);
            return;
        }

        LPVOID pMem = ::MapViewOfFile(
            hMap,
            FILE_MAP_READ,
            0, 0, 0
        );

        if (pMem == NULL)
        {
            ::CloseHandle(hMap);
            ::CloseHandle(hFile);
            return;
        }

        // We hold both the file handle and the memory pointer.
        // We can close the hMap handle now because Windows holds internally
        //  a reference to it since there is a view mapped.
        ::CloseHandle(hMap);

        // Store the handles inside the shared_ptr (with the custom destructors)
        m_file.reset(hFile, ::CloseHandle);
        m_mem.reset(static_cast<CharT*>(pMem), ::UnmapViewOfFile); 

        // Start of the file
        m_curChar = m_mem.get();
    }

    mmap_file_iterator(const mmap_file_iterator& iter)
    { *this = iter; }

    mmap_file_iterator& operator=(const mmap_file_iterator& iter)
    {
        m_curChar = iter.m_curChar;
        m_mem = iter.m_mem;
        m_file = iter.m_file;

        return *this;
    }

    // Nasty bug in Comeau up to 4.3.0.1, we need explicit boolean context
    //  for shared_ptr to evaluate correctly
    operator bool() const
    { return m_mem ? true : false; }

    bool operator==(const mmap_file_iterator& iter) const
    { return m_curChar == iter.m_curChar; }

    const CharT& get_cur_char(void) const
    { return *m_curChar; }

    void next_char(void)
    { m_curChar++; }

    void prev_char(void)
    { m_curChar--; }

    void advance(signed long n)
    { m_curChar += n; }

    long distance(const mmap_file_iterator& iter) const
    { return m_curChar - iter.m_curChar; }

    void seek_end(void)
    {
        m_curChar = m_mem.get() + 
            (::GetFileSize(m_file.get(), NULL) / sizeof(CharT));
    }

private:
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    typedef boost::remove_pointer<HANDLE>::type handle_t;
#else
    typedef void handle_t;
#endif
    
    boost::shared_ptr<CharT> m_mem;
    boost::shared_ptr<handle_t> m_file;
    CharT* m_curChar;
};

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  file_iterator_policy
//
//  This is the policy used by boost::iterator_adaptor to create a standard
//  conforming iterator wrapping around the base iterators. It basically just
//  dispatches function calls, since everything is implemented inside
//  the base iterators.
//
///////////////////////////////////////////////////////////////////////////////

template <typename BaseT>
struct file_iterator_policy
{
    template <class T>
    void initialize(T&)
    {}

    template <class IterT>
    typename IterT::reference dereference(const IterT& x) const
    {
        return x.base().get_cur_char();
    }

    template <class IterT>
    void increment(IterT& x)
    { 
        x.base().next_char();
    }

    template <class IterT>
    void decrement(IterT& x)
    { 
        x.base().prev_char();
    }

    template <class IterT, class DiffT>
    void advance(IterT& x, DiffT n)
    { 
        x.base().advance(n);
    }

    template <class Iter1T, class Iter2T>
    typename Iter1T::difference_type 
    distance(const Iter1T& x, const Iter2T& y) const
    { 
        return y.base().distance(x.base()); 
    }

    template <class Iter1T, class Iter2T>
    bool equal(const Iter1T& x, const Iter2T& y) const
    { 
        return x.base() == y.base(); 
    }
};


///////////////////////////////////////////////////////////////////////////////
} /* namespace boost::spirit::fileiter_impl */

template <typename CharT, typename BaseIteratorT>
file_iterator<CharT,BaseIteratorT> 
file_iterator<CharT,BaseIteratorT>::make_end(void)
{
    file_iterator iter(*this);

    // base() member function returns the reference
    //  to the adapted type (BaseFileIterator)
    iter.base().seek_end();
    
    return iter;
}

template <typename CharT, typename BaseIteratorT>
file_iterator<CharT,BaseIteratorT>& 
file_iterator<CharT,BaseIteratorT>::operator=(const base_t& iter)
{
    base_t::operator=(iter);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
}} /* namespace boost::spirit */


#endif /* BOOST_SPIRIT_FILE_ITERATOR_IPP */
