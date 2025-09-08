/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_TST_MARCH_09_2007_0905AM)
#define BOOST_SPIRIT_X3_TST_MARCH_09_2007_0905AM

#include <boost/spirit/home/x3/core/config.hpp>
#include <boost/spirit/home/x3/support/allocator.hpp>

#include <boost/assert.hpp>

#include <iterator>
#include <memory>
#include <string_view>
#include <string>
#include <utility>

namespace boost::spirit::x3::detail
{
    // This file contains low level TST routines, not for
    // public consumption.

    template <typename Char, typename T, typename Alloc = std::allocator<T>>
    struct tst_node
    {
        using allocator_type = Alloc;
        using node_allocator_type = std::allocator_traits<Alloc>::template rebind_alloc<tst_node>;

        constexpr explicit tst_node(Char id, Alloc const& alloc = {}) noexcept
            : alloc(alloc)
            , node_alloc(this->alloc)
            , id(id)
        {
        }

        constexpr tst_node(tst_node const& rhs)
            : alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(rhs.alloc))
            , node_alloc(std::allocator_traits<node_allocator_type>::select_on_container_copy_construction(rhs.node_alloc))
            , id(rhs.id)
            , data(allocator_ops<tst_node>::template copy_construct<&tst_node::alloc, &tst_node::data>(*this, rhs))
            , lt(allocator_ops<tst_node>::template copy_construct<&tst_node::node_alloc, &tst_node::lt>(*this, rhs))
            , eq(allocator_ops<tst_node>::template copy_construct<&tst_node::node_alloc, &tst_node::eq>(*this, rhs))
            , gt(allocator_ops<tst_node>::template copy_construct<&tst_node::node_alloc, &tst_node::gt>(*this, rhs))
        {
        }

        constexpr tst_node(tst_node&& rhs) noexcept
            : alloc(std::move(rhs.alloc))
            , node_alloc(std::move(rhs.node_alloc))
            , id(std::move(rhs.id))
            , data(std::exchange(rhs.data, nullptr))
            , lt(std::exchange(rhs.lt, nullptr))
            , eq(std::exchange(rhs.eq, nullptr))
            , gt(std::exchange(rhs.gt, nullptr))
        {
        }

        constexpr ~tst_node() noexcept
        {
            allocator_ops<tst_node>::template destroy_deallocate<
                &tst_node::alloc, &tst_node::data
            >(*this);

            allocator_ops<tst_node>::template destroy_deallocate<
                &tst_node::node_alloc,
                &tst_node::lt, &tst_node::eq, &tst_node::gt
            >(*this);
        }

        constexpr tst_node& operator=(tst_node const& rhs)
        {
            if (std::addressof(rhs) == this) return *this;

            id = rhs.id;

            allocator_ops<tst_node>::template copy_assign<
                &tst_node::alloc, &tst_node::data
            >(*this, rhs);

            allocator_ops<tst_node>::template copy_assign<
                &tst_node::node_alloc,
                &tst_node::lt, &tst_node::eq, &tst_node::gt
            >(*this, rhs);
        }

        constexpr tst_node& operator=(tst_node&& rhs)
            noexcept(allocator_ops<tst_node>::template move_assign_noexcept<allocator_type, node_allocator_type>)
        {
            if (std::addressof(rhs) == this) return *this;

            id = std::move(rhs.id);

            allocator_ops<tst_node>::template move_assign<
                &tst_node::alloc, &tst_node::data
            >(*this, std::move(rhs));

            allocator_ops<tst_node>::template move_assign<
                &tst_node::node_alloc,
                &tst_node::lt, &tst_node::eq, &tst_node::gt
            >(*this, std::move(rhs));
        }

        template <std::forward_iterator Iterator, typename CaseCompare>
        [[nodiscard]] static constexpr T*
        find(tst_node* start, Iterator& first, Iterator last, CaseCompare const& comp) noexcept
        {
            if (first == last) return nullptr;

            Iterator i = first;
            Iterator latest = first;
            tst_node* p = start;
            T* found = nullptr;

            while (p && i != last)
            {
                auto c = comp(*i,p->id);
                if (c == 0) {
                    if (p->data)
                    {
                        found = p->data;
                        latest = i;
                    }
                    p = p->eq;
                    ++i;
                }
                else if (c < 0)
                {
                    p = p->lt;
                }
                else
                {
                    p = p->gt;
                }
            }

            if (found)
            {
                first = ++latest; // one past the last matching char
            }
            return found;
        }

        template <typename F>
        static void
        for_each(tst_node* const p, std::basic_string_view<Char> const prefix, F&& f)
        {
            if (!p) return;

            tst_node::for_each(p->lt, prefix, f);
            std::basic_string<Char> s = std::basic_string<Char>(prefix) + p->id;
            tst_node::for_each(p->eq, s, f);
            if (p->data)
            {
                f(s, *p->data);
            }
            tst_node::for_each(p->gt, prefix, f);
        }

        friend struct allocator_ops<tst_node>;

        BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS Alloc alloc;
        BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS node_allocator_type node_alloc;
        Char id;                // the node's identity character
        T* data = nullptr;      // optional data
        tst_node* lt = nullptr; // left pointer
        tst_node* eq = nullptr; // middle pointer
        tst_node* gt = nullptr; // right pointer
    };
}

#endif
