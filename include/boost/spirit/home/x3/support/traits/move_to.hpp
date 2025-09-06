/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_MOVE_TO_JAN_17_2013_0859PM)
#define BOOST_SPIRIT_X3_MOVE_TO_JAN_17_2013_0859PM

#include <boost/spirit/home/x3/support/traits/attribute_category.hpp>
#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp>
#include <boost/spirit/home/x3/support/traits/variant_has_substitute.hpp>

#include <boost/assert.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/copy.hpp>
#include <boost/fusion/include/is_sequence.hpp>

#ifndef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
# include <boost/range/iterator_range_core.hpp> // deprecated
#endif

#include <memory>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3::traits
{
    // Identical types ---------------------------------------
    //
    // Note that these overloads must be strictly "more constrained"
    // than any of the other overloads.
    //
    // Such resolution should be possible even without an extra
    // constraint that checks `std::is_same_v<Source, Dest>`, thanks to
    // the ordinary overload resolution rules of C++.

    template <typename T>
        requires (!CategorizedAttr<T, unused_attribute>)
    constexpr void move_to(T const&& src, T& dest)
        noexcept(std::is_nothrow_assignable_v<T&, T const&&>)
    {
        dest = std::move(src);
    }

    template <typename T>
        requires (!CategorizedAttr<T, unused_attribute>)
    constexpr void move_to(T&& src, T& dest)
        noexcept(std::is_nothrow_assignable_v<T&, T&&>)
    {
        dest = std::move(src);
    }

    template <typename T>
        requires (!CategorizedAttr<T, unused_attribute>)
    constexpr void move_to(T const& src, T& dest)
        noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        dest = src;
    }

    template <typename T>
        requires (!CategorizedAttr<T, unused_attribute>)
    constexpr void move_to(T&, T&) noexcept
    {
        static_assert(
            !std::is_const_v<T>,
            "`x3::move_to(T const&, T const&)` is not allowed"
        );

        static_assert(
            false,
            "lvalue reference detected on the `src` argument of `x3::move_to`. "
            "The caller is definitely lacking `std::move` or `std::forward`. If you "
            "intend to *copy* the mutable value, apply `x3::move_to(std::as_const(attr_), attr)`."
        );
        // Banned: possible, but bug-prone.
        // dest = std::move(src);
    }

    // unused_type -------------------------------------------
    template <typename Source, typename Dest>
        requires
            CategorizedAttr<Source, unused_attribute> ||
            CategorizedAttr<Dest, unused_attribute>
    constexpr void move_to(Source&&, Dest&) noexcept
    {
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Dest>
        requires
            std::is_same_v<std::remove_const_t<Dest>, unused_type>
    constexpr void
    move_to(It, Se, Dest&) noexcept
    {
    }

    // Category specific --------------------------------------

    template <NonUnusedAttr Source, CategorizedAttr<plain_attribute> Dest>
        requires is_size_one_sequence_v<Source>
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(noexcept(dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)))))
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
        dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)));
    }

    template <NonUnusedAttr Source, CategorizedAttr<plain_attribute> Dest>
        requires (!is_size_one_sequence_v<Source>)
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
        static_assert(std::is_assignable_v<Dest&, Source&&>);
        dest = std::forward<Source>(src);
    }

    template <NonUnusedAttr Source, CategorizedAttr<tuple_attribute> Dest>
        requires
            is_same_size_sequence_v<Dest, Source> &&
            (!is_size_one_sequence_v<Dest>)
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(
            std::is_rvalue_reference_v<Source&&> ?
            noexcept(fusion::move(std::move(src), dest)) :
            noexcept(fusion::copy(src, dest))
        )
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

        if constexpr (std::is_rvalue_reference_v<Source&&>)
        {
            fusion::move(std::move(src), dest);
        }
        else
        {
            fusion::copy(src, dest);
        }
    }

    template <NonUnusedAttr Source, CategorizedAttr<variant_attribute> Dest>
        requires is_size_one_sequence_v<Source> && variant_has_substitute_v<Dest, Source>
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

        // dest is a variant, src is a single element fusion sequence that the variant
        // *can* directly hold.
        dest = std::forward<Source>(src);
    }

    template <NonUnusedAttr Source, CategorizedAttr<variant_attribute> Dest>
        requires is_size_one_sequence_v<Source> && (!variant_has_substitute_v<Dest, Source>)
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(noexcept(dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)))))
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

        // dest is a variant, src is a single element fusion sequence that the variant
        // cannot directly hold. We'll try to unwrap the single element fusion sequence.

        // Make sure that the Dest variant can really hold Source
        static_assert(
            variant_has_substitute_v<Dest, typename fusion::result_of::front<Source>::type>,
            "Error! The destination variant (Dest) cannot hold the source type (Source)"
        );

        dest = std::forward_like<Source>(fusion::front(std::forward<Source>(src)));
    }

    template <NonUnusedAttr Source, CategorizedAttr<variant_attribute> Dest>
        requires (!is_size_one_sequence_v<Source>)
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
        dest = std::forward<Source>(src);
    }

    template <NonUnusedAttr Source, CategorizedAttr<optional_attribute> Dest>
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(std::is_nothrow_assignable_v<Dest&, Source&&>)
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");
        dest = std::forward<Source>(src);
    }

    // Containers -------------------------------------------------

    template <std::forward_iterator It, std::sentinel_for<It> Se, CategorizedAttr<container_attribute> Dest>
    constexpr void
    move_to(It first, Se last, Dest& dest)
        // never noexcept, requires container insertion
    {
        if (traits::is_empty(dest))
        {
            dest = Dest(first, last);
        }
        else
        {
            traits::append(dest, first, last);
        }
    }

#ifndef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
    template <std::forward_iterator It>
    constexpr void
    move_to(It first, It last, boost::iterator_range<It>& rng)
    {
        rng = {first, last};
    }
#endif

    template <std::forward_iterator It, std::sentinel_for<It> Se, std::ranges::subrange_kind Kind>
    constexpr void
    move_to(It first, Se last, std::ranges::subrange<It, Se, Kind>& rng)
    {
        rng = std::ranges::subrange<It, Se, Kind>(std::move(first), std::move(last));
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, CategorizedAttr<tuple_attribute> Dest>
        requires is_size_one_sequence_v<Dest>
    constexpr void
    move_to(It first, Se last, Dest& dest)
        noexcept(noexcept(traits::move_to(first, last, fusion::front(dest))))
    {
        traits::move_to(first, last, fusion::front(dest));
    }

    template <ContainerAttr Source, CategorizedAttr<container_attribute> Dest>
    constexpr void
    move_to(Source&& src, Dest& dest)
        // TODO: noexcept
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

        if constexpr (std::is_rvalue_reference_v<Source&&>)
        {
            traits::move_to(std::make_move_iterator(std::ranges::begin(src)), std::make_move_iterator(std::ranges::end(src)), dest);
        }
        else
        {
            traits::move_to(std::ranges::begin(src), std::ranges::end(src), dest);
        }
    }

    // Size-one fusion tuple forwarding
    template <NonUnusedAttr Source, CategorizedAttr<tuple_attribute> Dest>
        requires is_size_one_sequence_v<Dest>
    constexpr void
    move_to(Source&& src, Dest& dest)
        noexcept(noexcept(traits::move_to(std::forward<Source>(src), fusion::front(dest))))
    {
        static_assert(!std::same_as<std::remove_cvref_t<Source>, Dest>, "[BUG] This call should instead resolve to the overload handling identical types");

        traits::move_to(std::forward<Source>(src), fusion::front(dest));
    }
} // boost::spirit::x3::traits

#endif
