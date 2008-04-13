// ptr_vector.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_PTR_VECTOR_HPP
#define BOOST_LEXER_PTR_VECTOR_HPP

#include "../size_t.hpp"
#include <vector>

namespace boost
{
namespace lexer
{
namespace internal
{
template<typename Type>
class ptr_vector
{
public:
    typedef std::vector<Type *> vector;

    ptr_vector ()
    {
    }

    ~ptr_vector ()
    {
        clear ();
    }

    vector *operator -> ()
    {
        return &_vector;
    }

    const vector *operator -> () const
    {
        return &_vector;
    }

    vector &operator * ()
    {
        return _vector;
    }

    const vector &operator * () const
    {
        return _vector;
    }

    Type * &operator [] (const std::size_t index_)
    {
        return _vector[index_];
    }

    Type * const &operator [] (const std::size_t index_) const
    {
        return _vector[index_];
    }

    void clear ()
    {
        if (!_vector.empty ())
        {
            Type **iter_ = &_vector.front ();
            Type **end_ = iter_ + _vector.size ();

            for (; iter_ != end_; ++iter_)
            {
                delete *iter_;
            }
        }

        _vector.clear ();
    }

private:
    vector _vector;

    ptr_vector (ptr_vector const &); // No copy construction.
    ptr_vector &operator = (ptr_vector const &); // No assignment.
};
}
}
}

#endif
