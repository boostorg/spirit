/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the unput queue iterator
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/
#if !defined(_UNPUT_QUEUE_ITERATOR_HPP__76DA23D0_4893_4AD5_ABCC_6CED7CFB89BC__INCLUDED_)
#define _UNPUT_QUEUE_ITERATOR_HPP__76DA23D0_4893_4AD5_ABCC_6CED7CFB89BC__INCLUDED_

#include <list>
#include <boost/iterator_adaptors.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

template <typename TokenT>
class unput_queue_policies : public boost::default_iterator_policies
{
public:
    unput_queue_policies(std::list<TokenT> const &unput_queue_)
    :   unput_queue(unput_queue_)
    {}
    
    unput_queue_policies &operator= (unput_queue_policies const &rhs)
    { unput_queue = rhs.unput_queue; return *this; }
    
    template <typename BaseT>
    void initialize(BaseT &)
    {}

    template <typename IteratorAdaptorT>
    typename IteratorAdaptorT::reference 
    dereference(const IteratorAdaptorT &x) const
    { 
        if (x.policies().unput_queue.size() > 0)
            return x.policies().unput_queue.front();
        return *x.base(); 
    }

    template <typename IteratorAdaptorT>
    void 
    increment(IteratorAdaptorT &x)
    { 
        if (x.policies().unput_queue.size() > 0)
            x.policies().unput_queue.pop_front();
        else
            ++x.base(); 
    }

    template <typename IteratorAdaptorT1, typename IteratorAdaptorT2>
    bool 
    equal(const IteratorAdaptorT1 &x, const IteratorAdaptorT2 &y) const
    { 
    // two iterators are equal, if both begin() iterators of the queue objects
    // are equal and the base iterators too
        return 
           (x.policies().unput_queue.begin() == y.policies().unput_queue.begin() ||
            (0 == x.policies().queuesize() && 0 == y.policies().queuesize())) &&
            x.base() == y.base(); 
    }
    
    typename std::list<TokenT>::size_type queuesize() const 
    { return unput_queue.size(); }
    
private:
    std::list<TokenT> unput_queue;
};

///////////////////////////////////////////////////////////////////////////////
//
//  unput_queue_iterator
//
//      The unput_queue_iterator templates encapsulates an unput_queue together
//      with the direct input to be read after the unput queue is emptied
//
///////////////////////////////////////////////////////////////////////////////
template <typename IteratorT, typename TokenT>
class unput_queue_iterator 
:   public boost::iterator_adaptor<IteratorT, unput_queue_policies<TokenT>,
        TokenT, TokenT const &, TokenT const *>
{
    typedef 
        boost::iterator_adaptor<
            IteratorT, unput_queue_policies<TokenT>, TokenT, TokenT const &, TokenT const *
        >
        base_t;
        
public:
    unput_queue_iterator(IteratorT const &it, 
            unput_queue_policies<TokenT> policies)
    :   base_t(it, policies)
    {}
};

namespace impl {

    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT, typename TokenT>
    struct gen_unput_queue_iterator {
    
        typedef unput_queue_iterator<IteratorT, TokenT> return_t;
        
        static return_t
        do_(std::list<TokenT> const &queue, IteratorT const &it)
        {
            return return_t(it, unput_queue_policies<TokenT>(queue));
        }
    };
    
    template <typename IteratorT, typename TokenT>
    struct gen_unput_queue_iterator<
        unput_queue_iterator<IteratorT, TokenT>, TokenT > {
    
        typedef unput_queue_iterator<IteratorT, TokenT> return_t;
        
        static return_t const &
        do_(std::list<TokenT> const &queue, return_t const &it)
        {
            return it;
        }
    };
    
    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT>
    struct assign_iterator {
    
        static void 
        do_ (IteratorT &dest, IteratorT const &src)
        {
            dest = src;
        }
    };
    
    template <typename IteratorT, typename TokenT>
    struct assign_iterator<unput_queue_iterator<IteratorT, TokenT> > {
    
        typedef unput_queue_iterator<IteratorT, TokenT> iterator_t;
        
        static void 
        do_ (iterator_t &dest, iterator_t const &src)
        {
            dest.base() = src.base();
            dest.policies() = src.policies();
        }
        
//        static void 
//        do_ (iterator_t &dest, IteratorT const &src)
//        {
//            dest.base() = src;
//        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT>
    struct is_fresh_token {

        static bool
        test_(IteratorT const &it)
        {
            return true;
        }
    };
    
    template <typename IteratorT, typename TokenT>
    struct is_fresh_token<unput_queue_iterator<IteratorT, TokenT> > {

        typedef unput_queue_iterator<IteratorT, TokenT> iterator_t;

        static bool
        test_(iterator_t const &it)
        {
            return 0 == it.policies().queuesize();
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
}

#endif // !defined(_UNPUT_QUEUE_ITERATOR_HPP__76DA23D0_4893_4AD5_ABCC_6CED7CFB89BC__INCLUDED_)
