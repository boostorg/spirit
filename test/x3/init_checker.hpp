/*=============================================================================
    Copyright (c) 2018 Nikita Kniazev

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_SPIRIT_TEST_X3_INIT_CHECKER_HPP)
#define BOOST_SPIRIT_TEST_X3_INIT_CHECKER_HPP

#include <iosfwd>
#include <stdexcept>

enum class how_initialized
{
    default_constructed,
    value_constructed,
    copy_constructed,
    move_constructed,
    copy_assigned,
    value_assigned,
    move_assigned,
};

struct init_checker
{
    how_initialized how_initialized;
    bool moved_out;
    mutable unsigned int copy_count;

    init_checker()
      : how_initialized{ how_initialized::default_constructed }
      , moved_out{ false }
      , copy_count{ 0 }
    {}

    init_checker(init_checker const& other)
      : how_initialized{ how_initialized::copy_constructed }
      , moved_out{ false }
      , copy_count{ 0 }
    {
        ++other.copy_count;
    }

    init_checker(init_checker && other)
      : how_initialized{ how_initialized::move_constructed }
      , moved_out{ false }
      , copy_count{ 0 }
    {
        other.moved_out = true;
    }

    init_checker& operator=(init_checker && other) {
        how_initialized = how_initialized::move_assigned;
        moved_out = false;
        copy_count = 0;
        other.moved_out = true;
        return *this;
    }

    init_checker& operator=(init_checker const& other) {
        how_initialized = how_initialized::copy_assigned;
        moved_out = false;
        copy_count = 0;
        ++other.copy_count;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, how_initialized const& v)
{
    switch (v) {
    case how_initialized::default_constructed: return os << "default_constructed";
    case how_initialized::value_constructed:   return os << "value_constructed";
    case how_initialized::copy_constructed:    return os << "copy_constructed";
    case how_initialized::move_constructed:    return os << "move_constructed";
    case how_initialized::copy_assigned:       return os << "copy_assigned";
    case how_initialized::value_assigned:      return os << "value_assigned";
    case how_initialized::move_assigned:       return os << "move_assigned";
    }
    throw std::runtime_error("invalid state");
}

std::ostream& operator<<(std::ostream& os, init_checker const& v)
{
    return os << "init_checker[" << v.how_initialized << "]";
}

#endif
