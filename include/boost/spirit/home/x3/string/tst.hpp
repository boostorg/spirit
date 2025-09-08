/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_TST_JUNE_03_2007_1031AM)
#define BOOST_SPIRIT_X3_TST_JUNE_03_2007_1031AM

#include <boost/spirit/home/x3/core/config.hpp>
#include <boost/spirit/home/x3/string/detail/tst_node.hpp>
#include <boost/spirit/home/x3/support/allocator.hpp>
#include <boost/assert.hpp>

#include <iterator>
#include <memory>
#include <string>
#include <type_traits>

namespace boost::spirit::x3
{
    struct tst_pass_through
    {
        template <typename Char>
        [[nodiscard]] constexpr Char operator()(Char ch) const noexcept
        {
            return ch;
        }
    };

    template <typename Char, typename T, typename Alloc = std::allocator<T>>
    struct tst
    {
        using char_type = Char; // the character type
        using value_type = T; // the value associated with each entry
        using allocator_type = Alloc;
        using node = detail::tst_node<Char, T>;
        using node_allocator_type = std::allocator_traits<Alloc>::template rebind_alloc<node>;

        constexpr tst() noexcept(std::is_nothrow_default_constructible_v<Alloc>) = default;

        constexpr explicit tst(Alloc const& alloc) noexcept
            : alloc_(alloc)
            , node_alloc_(alloc_)
        {}

        constexpr ~tst() noexcept
        {
            detail::allocator_ops<tst>::template destroy_deallocate<&tst::node_alloc_, &tst::root_>(*this);
        }

        constexpr tst(tst const& other)
            : alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.alloc_))
            , node_alloc_(std::allocator_traits<node_allocator_type>::select_on_container_copy_construction(other.node_alloc_))
            , root_(detail::allocator_ops<tst>::template copy_construct<&tst::node_alloc_, &tst::root_>(*this, other))
        {
        }

        constexpr tst(tst&& other) noexcept
            : alloc_(std::move(other.alloc_))
            , node_alloc_(std::move(other.node_alloc_))
            , root_(std::exchange(other.root_, nullptr))
        {
        }

        constexpr tst& operator=(tst const& other)
        {
            if (this == std::addressof(other)) return *this;
            detail::allocator_ops<tst>::template copy_assign<&tst::node_alloc_, &tst::root_>(*this, other);
            detail::allocator_ops<tst>::template copy_assign<&tst::alloc_>(*this, other);
            return *this;
        }

        constexpr tst& operator=(tst&& other)
            noexcept(detail::allocator_ops<tst>::template move_assign_noexcept<allocator_type, node_allocator_type>)
        {
            if (this == std::addressof(other)) return *this;
            detail::allocator_ops<tst>::template move_assign<&tst::node_alloc_, &tst::root_>(*this, std::move(other));
            detail::allocator_ops<tst>::template move_assign<&tst::alloc_>(*this, std::move(other));
            return *this;
        }

        template <std::forward_iterator Iterator, typename CaseCompare>
        [[nodiscard]] constexpr T* find(Iterator& first, Iterator last, CaseCompare caseCompare) const noexcept
        {
            return node::find(root_, first, last, caseCompare);
        }

        template <std::forward_iterator Iterator, typename Val>
        constexpr T* add(Iterator first, Iterator last, Val&& val)
        {
            if (first == last) return nullptr;
            if (!root_)
            {
                root_ = std::allocator_traits<node_allocator_type>::allocate(node_alloc_, 1);
                std::allocator_traits<node_allocator_type>::construct(node_alloc_, root_, *first, alloc_);
            }

            return this->add(root_, first, last, std::forward<Val>(val));
        }

        template <std::forward_iterator Iterator>
        constexpr void remove(Iterator first, Iterator last) noexcept
        {
            this->remove(root_, first, last);
        }

        constexpr void clear() noexcept
        {
            if (!root_) return;
            std::allocator_traits<node_allocator_type>::destroy(node_alloc_, root_);
            std::allocator_traits<node_allocator_type>::deallocate(node_alloc_, root_, 1);
            root_ = nullptr;
        }

        template <typename F>
        constexpr void for_each(F&& f) const
        {
            node::for_each(root_, {}, std::forward<F>(f));
        }

        friend struct detail::allocator_ops<tst>;

    private:
        template <std::forward_iterator Iterator, typename Val>
        [[nodiscard]] constexpr T*
        add(node* root, Iterator first, Iterator last, Val&& val)
        {
            BOOST_ASSERT(root != nullptr);
            BOOST_ASSERT(first != last);

            node** pp = &root;
            auto c = *first;

            while (true)
            {
                node* const p = *pp;

                if (c == p->id)
                {
                    if (++first == last)
                    {
                        if (!p->data)
                        {
                            p->data = std::allocator_traits<Alloc>::allocate(alloc_, 1);
                            std::allocator_traits<Alloc>::construct(alloc_, p->data, std::forward<Val>(val));
                        }
                        return p->data;
                    }
                    pp = &p->eq;
                    c = *first;
                }
                else if (c < p->id)
                {
                    pp = &p->lt;
                }
                else
                {
                    pp = &p->gt;
                }

                if (!*pp)
                {
                    *pp = std::allocator_traits<node_allocator_type>::allocate(node_alloc_, 1);
                    std::allocator_traits<node_allocator_type>::construct(node_alloc_, *pp, c);
                }
            }
        }

        template <std::forward_iterator Iterator>
        constexpr void
        remove(node*& p, Iterator first, Iterator last) noexcept
        {
            if (!p || first == last) return;

            auto c = *first;

            if (c == p->id)
            {
                if (++first == last)
                {
                    if (p->data)
                    {
                        std::allocator_traits<Alloc>::destroy(alloc_, p->data);
                        std::allocator_traits<Alloc>::deallocate(alloc_, p->data, 1);
                        p->data = nullptr;
                    }
                }
                this->remove(p->eq, first, last);
            }
            else if (c < p->id)
            {
                this->remove(p->lt, first, last);
            }
            else
            {
                this->remove(p->gt, first, last);
            }

            if (!p->data && !p->lt && !p->eq && !p->gt)
            {
                std::allocator_traits<node_allocator_type>::destroy(node_alloc_, p);
                std::allocator_traits<node_allocator_type>::deallocate(node_alloc_, p, 1);
                p = nullptr;
            }
        }

        BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS Alloc alloc_;
        BOOST_SPIRIT_X3_NO_UNIQUE_ADDRESS node_allocator_type node_alloc_;
        node* root_ = nullptr;
    };
}

#endif
