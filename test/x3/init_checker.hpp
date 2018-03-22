/*=============================================================================
    Copyright (c) 2018 Nikita Kniazev

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_SPIRIT_TEST_X3_INIT_CHECKER_HPP)
#define BOOST_SPIRIT_TEST_X3_INIT_CHECKER_HPP

#include <iosfwd>
#include <stdexcept>

struct init_checker
{
    enum state_type
    {
        default_constructed,
        value_constructed,
        copy_constructed,
        move_constructed,
        value_assigned,
        copy_assigned,
        move_assigned,
        moved_out,
    };
    state_type state;
    mutable unsigned int copy_count;

    init_checker()
      : state{ default_constructed }
      , copy_count{ 0 }
    {}

    init_checker(init_checker const& other)
      : state{ copy_constructed }
      , copy_count{ 0 }
    {
        BOOST_ASSERT(other.state != moved_out);
        ++other.copy_count;
    }

    init_checker(init_checker && other)
      : state{ move_constructed }
      , copy_count{ 0 }
    {
        BOOST_ASSERT(other.state != moved_out);
        other.state = moved_out;
    }

    init_checker& operator=(init_checker && other) {
        BOOST_ASSERT(other.state != moved_out);
        state = move_assigned;
        copy_count = 0;
        other.state = moved_out;
        return *this;
    }

    init_checker& operator=(init_checker const& other) {
        BOOST_ASSERT(other.state != moved_out);
        state = copy_assigned;
        copy_count = 0;
        ++other.copy_count;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, init_checker::state_type v)
{
    switch (v) {
    case init_checker::default_constructed: return os << "default_constructed";
    case init_checker::value_constructed:   return os << "value_constructed";
    case init_checker::copy_constructed:    return os << "copy_constructed";
    case init_checker::move_constructed:    return os << "move_constructed";
    case init_checker::value_assigned:      return os << "value_assigned";
    case init_checker::copy_assigned:       return os << "copy_assigned";
    case init_checker::move_assigned:       return os << "move_assigned";
    case init_checker::moved_out:           return os << "moved_out";
    }
    throw std::runtime_error("invalid state");
}

std::ostream& operator<<(std::ostream& os, init_checker const& v)
{
    return os << "init_checker[" << v.state << "]";
}

#endif
