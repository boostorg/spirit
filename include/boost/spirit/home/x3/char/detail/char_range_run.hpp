/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_CHAR_DETAIL_CHAR_RANGE_RUN_HPP
#define BOOST_SPIRIT_X3_CHAR_DETAIL_CHAR_RANGE_RUN_HPP

#include <boost/spirit/home/x3/char/detail/char_range.hpp>
#include <boost/spirit/home/x3/char/detail/char_range_functions.hpp>

#include <boost/assert.hpp>

#include <algorithm>
#include <vector>
#include <iterator>
#include <ranges>
#include <utility>

namespace boost::spirit::x3::detail
{
    // An implementation of a sparse bit (boolean) set. The set uses
    // a sorted vector of disjoint ranges. This class implements the
    // bare minimum essentials from which the full range of set
    // operators can be implemented. The set is constructed from
    // ranges. Internally, adjacent or overlapping ranges are
    // coalesced.
    //
    // range_runs are very space-economical in situations where there
    // are lots of ranges and a few individual disjoint values.
    // Searching is O(log n) where n is the number of ranges.
    template <typename CharT>
    class char_range_run
    {
    public:
        using range_type = char_range<CharT>;
        using storage_type = std::vector<range_type>; // TODO: use default_init_allocator as soon as constexpr placement new is available

        [[nodiscard]] static constexpr bool
        try_merge(storage_type& run, typename storage_type::iterator iter, range_type const& range)
            noexcept(std::is_nothrow_move_assignable_v<CharT>)
        {
            // *iter intersects with or is adjacent to 'range'?
            if (!detail::can_merge(*iter, range)) return false;

            // merge range and *iter
            detail::merge(*iter, range);

            // collapse all subsequent ranges that can merge with *iter:
            auto it = std::next(iter);
            // 1. skip subsequent ranges completely included in *iter
            while (it != run.end() && it->last <= iter->last)
            {
                ++it;
            }
            // 2. collapse next range if adjacent or overlapping with *iter
            if (it != run.end() && it->first-1 <= iter->last)
            {
                iter->last = it->last;
                ++it;
            }

            // erase all ranges that were collapsed
            run.erase(std::next(iter), it);
            return true;
        }

        [[nodiscard]] constexpr bool test(CharT val) const noexcept
        {
            if (run_.empty()) return false;

            // search the ranges for one that potentially includes val
            auto const iter = std::ranges::upper_bound(
                run_, val,
                char_range_compare{}
            );

            // return true if *(iter-1) includes val
            return iter != run_.begin() && detail::includes(*std::prev(iter), val);
        }

        constexpr void swap(char_range_run& other) noexcept
        {
            run_.swap(other.run_);
        }

        constexpr void set(range_type const& range)
        {
            BOOST_ASSERT(detail::is_valid(range));
            if (run_.empty())
            {
                // the vector is empty, insert 'range'
                run_.emplace_back(range);
                return;
            }

            // search the ranges for one that potentially includes 'range'
            auto iter = std::ranges::upper_bound(run_, range, char_range_compare{});

            if (iter != run_.begin())
            {
                // if *(iter-1) includes 'range', return early
                if (detail::includes(*std::prev(iter), range))
                {
                    return;
                }

                // if *(iter-1) can merge with 'range', merge them and return
                if (this->try_merge(run_, std::prev(iter), range))
                {
                    return;
                }
            }

            // if *iter can merge with with 'range', merge them
            if (iter == run_.end() || !this->try_merge(run_, iter, range))
            {
                // no overlap, insert 'range'
                run_.insert(iter, range);
            }
        }

        constexpr void clear(range_type const& range)
        {
            BOOST_ASSERT(detail::is_valid(range));
            if (run_.empty()) return;

            // search the ranges for one that potentially includes 'range'
            auto iter = std::ranges::upper_bound(run_, range, char_range_compare{});

            // 'range' starts with or after another range:
            if (iter != run_.begin())
            {
                auto const left_iter = std::prev(iter);

                // 'range' starts after '*left_iter':
                if (left_iter->first < range.first)
                {
                    // if 'range' is completely included inside '*left_iter':
                    // need to break it apart into two ranges (punch a hole),
                    if (left_iter->last > range.last)
                    {
                        auto const last_save = left_iter->last;
                        left_iter->last = range.first-1;
                        run_.insert(iter, range_type(range.last+1, last_save));
                        return;
                    }
                    // if 'range' contains 'left_iter->last':
                    // truncate '*left_iter' (clip its right)
                    else if (left_iter->last >= range.first)
                    {
                        left_iter->last = range.first-1;
                    }
                }

                // 'range' has the same left bound as '*left_iter': it
                // must be removed or truncated by the code below
                else
                {
                    iter = left_iter;
                }
            }

            // remove or truncate subsequent ranges that overlap with 'range':
            auto it = iter;
            // 1. skip subsequent ranges completely included in 'range'
            while (it != run_.end() && it->last <= range.last)
            {
                ++it;
            }
            // 2. clip left of next range if overlapping with 'range'
            if (it != run_.end() && it->first <= range.last)
            {
                it->first = range.last+1;
            }

            // erase all ranges that 'range' contained
            run_.erase(iter, it);
        }

        constexpr void clear() noexcept
        {
            run_.clear();
        }

    private:
        storage_type run_;
    };
} // boost::spirit::x3::detail

#endif
