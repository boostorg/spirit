// iteration_node.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_ITERATION_NODE_HPP
#define BOOST_LEXER_ITERATION_NODE_HPP

#include "node.hpp"

namespace boost
{
namespace lexer
{
namespace internal
{
class iteration_node : public node
{
public:
    iteration_node (node *next_) :
        node (true),
        _next (next_)
    {
        node_vector::iterator iter_;
        node_vector::iterator end_;

        _next->append_firstpos (_firstpos);
        _next->append_lastpos (_lastpos);

        for (iter_ = _lastpos.begin (), end_ = _lastpos.end ();
            iter_ != end_; ++iter_)
        {
            (*iter_)->append_followpos (_firstpos);
        }
    }

    virtual ~iteration_node ()
    {
    }

    virtual type what_type () const
    {
        return ITERATION;
    }

    virtual bool traverse (const_node_stack &node_stack_,
        bool_stack &perform_op_stack_) const
    {
        perform_op_stack_.push (true);
        node_stack_.push (_next);
        return true;
    }

private:
    // Not owner of this pointer...
    node *_next;

    virtual void copy_node (node_ptr_vector &node_ptr_vector_,
        node_stack &new_node_stack_, bool_stack &perform_op_stack_,
        bool &down_) const
    {
        if (perform_op_stack_.top ())
        {
            node *ptr_  = new_node_stack_.top ();

            node_ptr_vector_->push_back (0);
            node_ptr_vector_->back () = new iteration_node (ptr_);
            new_node_stack_.top () = node_ptr_vector_->back ();
        }
        else
        {
            down_ = true;
        }

        perform_op_stack_.pop ();
    }
};
}
}
}

#endif
