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
namespace util {

template <typename TokenT, typename ContainerT>
class unput_queue_policies : public boost::default_iterator_policies
{
public:
    unput_queue_policies(ContainerT &unput_queue_)
    :   unput_queue(unput_queue_)
    {}
    
    unput_queue_policies &operator= (unput_queue_policies const &rhs)
    { 
        unput_queue = rhs.unput_queue; 
        return *this; 
    }
    
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
        if (x.policies().unput_queue.size() > 0) {
        // there exist pending tokens in the unput queue
            x.policies().unput_queue.pop_front();
        }
        else {
        // the unput_queue is empty, so advance the base iterator
            ++x.base(); 
        }
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
    
    typename ContainerT::size_type queuesize() const 
    { return unput_queue.size(); }

    ContainerT &get_unput_queue() { return unput_queue; }
    
private:
    ContainerT &unput_queue;
};

///////////////////////////////////////////////////////////////////////////////
//
//  unput_queue_iterator
//
//      The unput_queue_iterator templates encapsulates an unput_queue together
//      with the direct input to be read after the unput queue is emptied
//
///////////////////////////////////////////////////////////////////////////////
template <typename IteratorT, typename TokenT, typename ContainerT>
class unput_queue_iterator 
:   public boost::iterator_adaptor<
        IteratorT, unput_queue_policies<TokenT, ContainerT>, TokenT, 
        TokenT const &, TokenT const *>
{
    typedef 
        boost::iterator_adaptor<
            IteratorT, unput_queue_policies<TokenT, ContainerT>, TokenT, 
            TokenT const &, TokenT const *
        >
        base_t;
        
public:
    unput_queue_iterator(IteratorT const &it, 
            unput_queue_policies<TokenT, ContainerT> policies)
    :   base_t(it, policies)
    {}
    
    ContainerT &get_unput_queue() { return policies().get_unput_queue(); }
};

namespace impl {

    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT, typename TokenT, typename ContainerT>
    struct gen_unput_queue_iterator {
    
        typedef unput_queue_iterator<IteratorT, TokenT, ContainerT> return_t;
        typedef unput_queue_policies<TokenT, ContainerT> policies_t;
        
        static return_t
        generate(ContainerT &queue, IteratorT const &it)
        {
            return return_t(it, policies_t(queue));
        }
    };
    
    template <typename IteratorT, typename TokenT, typename ContainerT>
    struct gen_unput_queue_iterator<
        unput_queue_iterator<IteratorT, TokenT, ContainerT>, 
            TokenT, ContainerT> 
    {
    
        typedef unput_queue_iterator<IteratorT, TokenT, ContainerT> return_t;
        typedef unput_queue_policies<TokenT, ContainerT> policies_t;
        
        static return_t 
        generate(ContainerT &queue, return_t const &it)
        {
            return return_t(it.base(), policies_t(queue));
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
    
    template <typename IteratorT, typename TokenT, typename ContainerT>
    struct assign_iterator<
        unput_queue_iterator<IteratorT, TokenT, ContainerT> > {
    
        typedef unput_queue_iterator<IteratorT, TokenT, ContainerT> iterator_t;
        
        static void 
        do_ (iterator_t &dest, iterator_t const &src)
        {
            dest.base() = src.base();
            dest.policies() = src.policies();
        }
    };

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace cpp

#endif // !defined(_UNPUT_QUEUE_ITERATOR_HPP__76DA23D0_4893_4AD5_ABCC_6CED7CFB89BC__INCLUDED_)
