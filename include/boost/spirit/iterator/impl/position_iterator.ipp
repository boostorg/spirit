/*=============================================================================
    Copyright (c) 2002 Juan Carlos Arevalo-Baeza
    Copyright (c) 2002-2003 Hartmut Kaiser
    Copyright (c) 2003 Giovanni Bajo
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef POSITION_ITERATOR_IPP
#define POSITION_ITERATOR_IPP

#include <boost/config.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/core/nil.hpp>  // for nil_t

namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
//
//  position_policy<file_position_without_column>
//
//  Specialization to handle file_position_without_column. Only take care of
//  newlines since no column tracking is needed.
//
///////////////////////////////////////////////////////////////////////////////
template <>
class position_policy<file_position_without_column> {

public:
    void next_line(file_position_without_column& pos)
    {
        ++pos.line;
    }

    void set_tab_chars(unsigned int chars){}
    void next_char(file_position_without_column& pos)    {}
    void tabulation(file_position_without_column& pos)   {}
};

///////////////////////////////////////////////////////////////////////////////
//
//  position_policy<file_position>
//
//  Specialization to handle file_position. Track characters and tabulation
//  to compute the current column correctly.
//
//  Default tab size is 4. You can change this with the set_tabchars member
//  of position_iterator.
//
///////////////////////////////////////////////////////////////////////////////
template <>
class position_policy<file_position> {

public:
    position_policy()
        : m_CharsPerTab(4)
    {}

    void next_line(file_position& pos)
    {
        ++pos.line;
        pos.column = 1;
    }

    void set_tab_chars(unsigned int chars)
    {
        m_CharsPerTab = chars;
    }

    void next_char(file_position& pos)
    {
        ++pos.column;
    }

    void tabulation(file_position& pos)
    {
        pos.column += m_CharsPerTab - (pos.column - 1) % m_CharsPerTab;
    }

private:
    unsigned int m_CharsPerTab;
};

/* namespace boost::spirit { */ namespace iterator_ { namespace impl {

#if !defined(BOOST_ITERATOR_ADAPTORS_VERSION) || \   
     BOOST_ITERATOR_ADAPTORS_VERSION < 0x0200   
    
///////////////////////////////////////////////////////////////////////////////   
//   
//  Uses the iterator_adaptor version from Boost V1.30.0   
//   
///////////////////////////////////////////////////////////////////////////////   
  
///////////////////////////////////////////////////////////////////////////////   
//   
//  position_iterator_policy   
//   
//  Policy for iterator_adaptors, to define the semantic of the iterator.   
//  The policy also stores the iterator local variables in it, and inherits   
//  the position_policy to know how to update the position structure.   
//   
///////////////////////////////////////////////////////////////////////////////   
  
template <typename MainIterT, typename ForwardIteratorT, typename PositionT>   
struct position_iterator_policy   
    : public position_policy<PositionT>,   
      public boost::default_iterator_policies   
{   
protected:   
    typedef position_policy<PositionT> position_policy_t;   
  
public:   
    position_iterator_policy()   
        : _isend(true)   
    {}   
  
    position_iterator_policy(   
        const PositionT& pos,   
        const ForwardIteratorT& end)   
        : _end(end), _pos(pos), _isend(false)   
    {}   
  
    template <typename BaseT>   
    void initialize(BaseT& x)   
    {   
        _isend = (x == _end);   
    }   
  
    template <typename IteratorT>   
    void increment(IteratorT& x)   
    {   
        typename IteratorT::value_type val = *x.base();   
        if (val == '\n' || val == '\r') {   
            ++x.base();   
            if (x.base() != this->_end) {   
                typename IteratorT::value_type val2 = *x.base();   
                if ((val == '\n' && val2 == '\r')   
                    || (val == '\r' && val2 == '\n')) {   
                    ++x.base();   
                }   
            }   
            this->next_line(_pos);   
            static_cast<MainIterT&>(x).newline();   
        } else if (val == '\t') {   
            this->tabulation(_pos);   
            ++x.base();   
        } else {   
            this->next_char(_pos);   
            ++x.base();   
        }   
  
        // The iterator is at the end only if it's the same   
        //  of the   
        this->_isend = (x.base() == this->_end);   
    }   
  
    template <typename Iterator1T, typename Iterator2T>   
    bool equal(const Iterator1T& x, const Iterator2T& y) const   
    {   
        bool x_is_end = x.policies()._isend;   
        bool y_is_end = y.policies()._isend;   
  
        return (x_is_end && y_is_end) ||   
            (!x_is_end && !y_is_end && x.base() == y.base());   
    }   
  
    ForwardIteratorT _end;   
    PositionT _pos;   
    bool _isend;   
};   
  
///////////////////////////////////////////////////////////////////////////////   
//   
//  position_iterator_generator   
//   
//  Metafunction to generate the iterator type using boost::iterator_adaptors,   
//  hiding all the metaprogramming thunking code in it. It is used   
//  mainly to keep the public interface (position_iterator) cleanear.   
//   
///////////////////////////////////////////////////////////////////////////////   
  
template <typename MainIterT, typename ForwardIterT, typename PositionT>   
struct position_iterator_generator   
{   
private:   
    typedef BOOST_SPIRIT_IT_NS   
        ::iterator_traits<ForwardIterT> traits;   
    typedef typename traits::value_type value_type;   
  
    // Position iterator is always a non-mutable iterator   
    typedef typename boost::add_const<value_type>::type const_value_type;   
  
    // Check if the MainIterT is nil. If it's nil, it means that the actual   
    //  self type is position_iterator. Otherwise, it's a real type we   
    //  must use   
    typedef typename boost::mpl::if_   
    <   
        typename boost::is_same<MainIterT, nil_t>::type,   
        position_iterator<ForwardIterT, PositionT, nil_t>,   
        MainIterT   
    >::type main_iter_t;   
  
public:   
    typedef boost::iterator_adaptor   
    <   
        ForwardIterT,   
        position_iterator_policy<main_iter_t, ForwardIterT, PositionT>,   
        const_value_type,   
        boost::iterator_category_is<std::forward_iterator_tag>   
    > type;   
};   
    
#else // BOOST_ITERATOR_ADAPTORS_VERSION < 0x0200   
  
///////////////////////////////////////////////////////////////////////////////
//
//  Uses the newer iterator_adaptor version (should be released with
//  Boost V1.31.0)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  position_iterator_generator
//
//  Metafunction to generate the iterator type using boost::iterator_adaptors,
//  hiding all the metaprogramming thunking code in it. It is used
//  mainly to keep the public interface (position_iterator) cleanear.
//
///////////////////////////////////////////////////////////////////////////////
template <typename MainIterT, typename ForwardIterT, typename PositionT>
struct position_iterator_generator
{
private:
    typedef std::iterator_traits<ForwardIterT> traits;
    typedef typename traits::value_type value_type;
    typedef typename traits::iterator_category iter_category_t;

    // Position iterator is always a non-mutable iterator
    typedef typename boost::add_const<value_type>::type const_value_type;

public:
    // Check if the MainIterT is nil. If it's nil, it means that the actual
    //  self type is position_iterator. Otherwise, it's a real type we
    //  must use
    typedef typename boost::mpl::if_<
        typename boost::is_same<MainIterT, nil_t>::type,
        position_iterator<ForwardIterT, PositionT, nil_t>,
        MainIterT
    >::type main_iter_t;

    typedef boost::iterator_adaptor<
        main_iter_t,
        ForwardIterT,
        const_value_type 
    > type;
};

#endif // BOOST_ITERATOR_ADAPTORS_VERSION < 0x0200   
  
}}}} /* namespace boost::spirit::iterator_::impl */

#endif
