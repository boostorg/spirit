/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SUPPORT_ALLOCATOR_HPP)
#define BOOST_SPIRIT_X3_SUPPORT_ALLOCATOR_HPP

#include <boost/assert.hpp>

#include <memory>
#include <type_traits>

namespace boost::spirit::x3::detail {

template <typename Klass>
struct allocator_ops
{
    template <typename... Allocs>
    static constexpr bool move_assign_noexcept = std::conjunction_v<
        std::disjunction<
            typename std::allocator_traits<Allocs>::propagate_on_container_move_assignment,
            typename std::allocator_traits<Allocs>::is_always_equal
        >...
    >;

    template <auto AllocMem, auto Mem>
    [[nodiscard]] static constexpr auto copy_construct(Klass& self, Klass const& other)
        -> std::remove_reference_t<decltype(self.*Mem)>
    {
        if (!(other.*Mem)) return nullptr;

        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        auto& alloc = self.*AllocMem;
        auto* data = std::allocator_traits<Alloc>::allocate(alloc, 1);
        std::allocator_traits<Alloc>::construct(alloc, data, *(other.*Mem));
        return data;
    }

    template <auto AllocMem, auto... Mems>
    static constexpr void destroy_deallocate(Klass& self) noexcept
    {
        static_assert(sizeof...(Mems) > 0);
        (allocator_ops::destroy_deallocate_impl<AllocMem, Mems>(self), ...);
    }

    template <auto AllocMem, auto... Mems>
    static constexpr void copy_assign(Klass& self, Klass const& other)
    {
        BOOST_ASSERT(std::addressof(self) != std::addressof(other));

        (allocator_ops::copy_assign_impl<AllocMem, Mems>(self, other), ...);

        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        constexpr bool pocca = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value;
        if constexpr (pocca)
        {
            self.*AllocMem = other.*AllocMem;
        }
    }

    template <auto AllocMem, auto... Mems>
    static constexpr void move_assign(Klass& self, Klass&& other)
        noexcept(move_assign_noexcept<decltype(self.*AllocMem)>)
    {
        BOOST_ASSERT(std::addressof(self) != std::addressof(other));

        (allocator_ops::move_assign_impl<AllocMem, Mems>(self, std::move(other)), ...);

        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        constexpr bool pocca = std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value;
        if constexpr (pocca)
        {
            self.*AllocMem = std::move(other.*AllocMem);
        }
    }


private:
    template <auto AllocMem, auto Mem>
    static constexpr void destroy_deallocate_impl(Klass& self) noexcept
    {
        auto& data = self.*Mem;
        if (!data) return;

        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        auto& alloc = self.*AllocMem;
        std::allocator_traits<Alloc>::destroy(alloc, data);
        std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
    }

    template <auto AllocMem, auto Mem>
    static constexpr void copy_assign_impl(Klass& self, Klass const& other)
    {
        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        constexpr bool pocca = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value;

        auto& data = self.*Mem;
        auto& alloc = self.*AllocMem;

        auto const& other_data = other.*Mem;
        auto const& other_alloc = other.*AllocMem;

        if (other_data)
        {
            if constexpr (std::allocator_traits<Alloc>::is_always_equal::value)
            {
                if (data)
                {
                    *data = *other_data;
                    return;
                }
                if constexpr (pocca)
                {
                    data = std::allocator_traits<Alloc>::allocate(other_alloc, 1);
                    std::allocator_traits<Alloc>::construct(other_alloc, data, *other_data);
                }
                else
                {
                    data = std::allocator_traits<Alloc>::allocate(alloc, 1);
                    std::allocator_traits<Alloc>::construct(alloc, data, *other_data);
                }
                return;
            }
            else if (alloc == other_alloc)
            {
                if (data)
                {
                    *data = *other_data;
                    return;
                }
                if constexpr (pocca)
                {
                    data = std::allocator_traits<Alloc>::allocate(other_alloc, 1);
                    std::allocator_traits<Alloc>::construct(other_alloc, data, *other_data);
                }
                else
                {
                    data = std::allocator_traits<Alloc>::allocate(alloc, 1);
                    std::allocator_traits<Alloc>::construct(alloc, data, *other_data);
                }
                return;
            }
            else
            {
                if (data)
                {
                    std::allocator_traits<Alloc>::destroy(alloc, data);
                    std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                    data = nullptr;
                }
                if constexpr (pocca)
                {
                    data = std::allocator_traits<Alloc>::allocate(other_alloc, 1);
                    std::allocator_traits<Alloc>::construct(other_alloc, data, *other_data);
                }
                else
                {
                    data = std::allocator_traits<Alloc>::allocate(alloc, 1);
                    std::allocator_traits<Alloc>::construct(alloc, data, *other_data);
                }
                return;
            }
        }
        else // !other_data
        {
            if (data)
            {
                std::allocator_traits<Alloc>::destroy(alloc, data);
                std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                data = nullptr;
            }
            return;
        }
    }

    template <auto AllocMem, auto Mem>
    static constexpr void move_assign_impl(Klass& self, Klass&& other)
        noexcept(move_assign_noexcept<std::remove_reference_t<decltype(self.*AllocMem)>>)
    {
        using Alloc = std::remove_reference_t<decltype(self.*AllocMem)>;
        constexpr bool pocma = std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value;

        auto& data = self.*Mem;
        auto& alloc = self.*AllocMem;

        auto& other_data = other.*Mem;
        auto& other_alloc = other.*AllocMem;

        if (other_data)
        {
            if constexpr (std::allocator_traits<Alloc>::is_always_equal::value)
            {
                if (data)
                {
                    std::allocator_traits<Alloc>::destroy(alloc, data);
                    std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                }
                data = std::exchange(other_data, nullptr);
                return;
            }
            else if (alloc == other_alloc)
            {
                if (data)
                {
                    std::allocator_traits<Alloc>::destroy(alloc, data);
                    std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                }
                data = std::exchange(other_data, nullptr);
                return;
            }
            else
            {
                if (data)
                {
                    std::allocator_traits<Alloc>::destroy(alloc, data);
                    std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                }
                if constexpr (pocma)
                {
                    data = std::allocator_traits<Alloc>::allocate(other_alloc, 1);
                    std::allocator_traits<Alloc>::construct(other_alloc, data, std::move(*other_data));
                }
                else
                {
                    data = std::allocator_traits<Alloc>::allocate(alloc, 1);
                    std::allocator_traits<Alloc>::construct(alloc, data, std::move(*other_data));
                }

                std::allocator_traits<Alloc>::destroy(other_alloc, other_data);
                std::allocator_traits<Alloc>::deallocate(other_alloc, other_data, 1);
                return;
            }
        }
        else // !other_data
        {
            if (data)
            {
                std::allocator_traits<Alloc>::destroy(alloc, data);
                std::allocator_traits<Alloc>::deallocate(alloc, data, 1);
                data = nullptr;
            }
            return;
        }
    }
};

} // boost::spirit::x3::detail

#endif
