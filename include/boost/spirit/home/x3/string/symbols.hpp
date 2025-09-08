/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Carl Barron
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SYMBOLS_MARCH_11_2007_1055AM)
#define BOOST_SPIRIT_X3_SYMBOLS_MARCH_11_2007_1055AM

#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/string/tst.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/no_case.hpp>

#include <boost/spirit/home/x3/char_encoding/detail/encoding_warning.hpp>
#include <boost/spirit/home/x3/char_encoding/ascii.hpp>
#include <boost/spirit/home/x3/char_encoding/iso8859_1.hpp>
#include <boost/spirit/home/x3/char_encoding/standard.hpp>
#include <boost/spirit/home/x3/char_encoding/standard_wide.hpp>

#ifdef BOOST_SPIRIT_X3_UNICODE
# include <boost/spirit/home/x3/char_encoding/unicode.hpp>
#endif

#include <string>
#include <string_view>
#include <ranges>
#include <iterator>
#include <initializer_list>
#include <iterator> // std::begin
#include <memory> // std::shared_ptr
#include <type_traits>
#include <utility>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4355) // 'this' : used in base member initializer list warning
#endif

#define BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING(old_api) \
    "Use `shared_" old_api "` instead. `" old_api "` has had a " \
    "*implicit* trait where the underlying storage is shared via " \
    "`std::shared_ptr`. This disallows `constexpr` usage in generic " \
    "scenarios where the sharing is not actually needed at all. Even " \
    "for non-`constexpr` usage, the old name `" old_api "` does not " \
    "represent this trait, so the usage of the old API is strongly " \
    "discouraged."

namespace boost::spirit::x3
{
namespace detail
{
    template <typename Derived, bool IsShared, typename Encoding, typename T, typename Lookup>
    struct symbols_parser_impl : parser<Derived>
    {
        using char_type = typename Encoding::char_type; // the character type
        using encoding = Encoding;
        using value_type = T; // the value associated with each entry
        using attribute_type = value_type;

        static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;
        static constexpr bool handles_container = traits::is_container_v<attribute_type>;

        constexpr symbols_parser_impl(std::string_view name = "symbols")
            requires(IsShared)
            : add{*this}
            , remove{*this}
            , lookup(std::make_shared<Lookup>())
            , name_(name)
        {
        }

        constexpr symbols_parser_impl(std::string_view name = "symbols")
            requires(!IsShared)
            : add{*this}
            , remove{*this}
            , lookup(std::make_unique<Lookup>())
            , name_(name)
        {
        }

        constexpr symbols_parser_impl(symbols_parser_impl const& syms)
            requires(IsShared)
            : add{*this}
            , remove{*this}
            , lookup(syms.lookup)
            , name_(syms.name_)
        {
        }

        constexpr symbols_parser_impl(symbols_parser_impl const& syms)
            requires(!IsShared)
            : add{*this}
            , remove{*this}
            , lookup(std::make_unique<Lookup>(*syms.lookup))
            , name_(syms.name_)
        {
        }

        constexpr symbols_parser_impl(symbols_parser_impl&&) noexcept = default;

        template <std::ranges::forward_range Symbols>
            requires std::convertible_to<std::ranges::range_value_t<Symbols>, std::basic_string_view<char_type>>
        constexpr symbols_parser_impl(Symbols const& syms, std::string const& name = "symbols")
            : symbols_parser_impl(name)
        {
            for (auto const& sym : syms)
            {
                this->add(sym);
            }
        }

        template <std::ranges::forward_range Symbols, std::ranges::forward_range Data>
            requires
                std::convertible_to<std::ranges::range_value_t<Symbols>, std::basic_string_view<char_type>> &&
                std::convertible_to<std::ranges::range_value_t<Data>, T>
        constexpr symbols_parser_impl(
            Symbols const& syms, Data const& data, std::string const& name = "symbols"
        )
            : symbols_parser_impl(name)
        {
            auto di = std::ranges::begin(data);
            for (auto const& sym : syms)
            {
                this->add(sym, *di++);
            }
        }

        constexpr symbols_parser_impl(
            std::initializer_list<std::pair<char_type const*, T>> syms,
            std::string const & name="symbols"
        )
            : symbols_parser_impl(name)
        {
            for (auto const& sym : syms)
            {
                add(sym.first, sym.second);
            }
        }

        constexpr symbols_parser_impl(
            std::initializer_list<char_type const*> syms,
            std::string const &name="symbols"
        )
            : symbols_parser_impl(name)
        {
            for (auto const& str : syms)
            {
                add(str);
            }
        }

        constexpr symbols_parser_impl& operator=(symbols_parser_impl const& rhs)
        {
            name_ = rhs.name_;
            lookup = rhs.lookup;
            return *this;
        }

        constexpr symbols_parser_impl& operator=(symbols_parser_impl&&) noexcept = default;

        constexpr void clear() noexcept
        {
            lookup->clear();
        }

        struct adder;
        struct remover;

        constexpr adder const&
        operator=(std::basic_string_view<char_type> const s)
        {
            lookup->clear();
            return this->add(s);
        }

        friend constexpr adder const&
        operator+=(symbols_parser_impl& sym, std::basic_string_view<char_type> const s)
        {
            return sym.add(s);
        }

        friend constexpr remover const&
        operator-=(symbols_parser_impl& sym, std::basic_string_view<char_type> const s)
        {
            return sym.remove(s);
        }

        template <typename F>
        constexpr void for_each(F&& f) const
        {
            lookup->for_each(std::forward<F>(f));
        }

        template <typename F>
        constexpr void for_each(F&& f)
        {
            lookup->for_each(std::forward<F>(f));
        }

        [[nodiscard]] constexpr value_type& at(std::basic_string_view<char_type> const s)
        {
            return *lookup->add(s.begin(), s.end(), T{});
        }

        template <std::forward_iterator Iterator>
        [[nodiscard]] constexpr value_type* prefix_find(Iterator& first, Iterator const& last) noexcept
        {
            return lookup->find(first, last, case_compare<Encoding>());
        }

        template <std::forward_iterator Iterator>
        [[nodiscard]] constexpr value_type const* prefix_find(Iterator& first, Iterator const& last) const noexcept
        {
            return lookup->find(first, last, case_compare<Encoding>());
        }

        [[nodiscard]] constexpr value_type* find(std::basic_string_view<char_type> const s) noexcept
        {
            return this->find_impl(s.begin(), s.end());
        }

        [[nodiscard]] constexpr value_type const* find(std::basic_string_view<char_type> const s) const noexcept
        {
            return this->find_impl(s.begin(), s.end());
        }

        template <std::forward_iterator Iterator, typename Context, typename Attribute>
        [[nodiscard]] constexpr bool parse(
            Iterator& first, Iterator const& last, Context const& context, unused_type, Attribute& attr
        ) const noexcept(
            noexcept(x3::skip_over(first, last, context)) &&
            noexcept(x3::traits::move_to(std::declval<value_type const&>(), attr))
        )
        {
            x3::skip_over(first, last, context);

            if (value_type const* val_ptr = lookup->find(first, last, x3::get_case_compare<Encoding>(context)))
            {
                x3::traits::move_to(*val_ptr, attr);
                return true;
            }
            return false;
        }

        constexpr void name(std::string const &str)
        {
            name_ = str;
        }
        [[nodiscard]] constexpr std::string const& name() const noexcept
        {
            return name_;
        }

        struct [[maybe_unused]] adder
        {
            template <std::forward_iterator Iterator>
            [[maybe_unused]] constexpr adder const&
            operator()(Iterator first, Iterator last, T const& val) const
            {
                sym.lookup->add(first, last, val);
                return *this;
            }

            [[maybe_unused]] constexpr adder const&
            operator()(std::basic_string_view<char_type> const s, T const& val = T{}) const
            {
                sym.lookup->add(s.begin(), s.end(), val);
                return *this;
            }

            [[maybe_unused, deprecated("Don't rely on overloaded comma operator. It's the leftover from the black magic in the 2000s.")]]
            constexpr adder const&
            operator,(std::basic_string_view<char_type> const s) const
            {
                sym.lookup->add(s.begin(), s.end(), T{});
                return *this;
            }

            symbols_parser_impl& sym;
        };

        struct [[maybe_unused]] remover
        {
            template <std::forward_iterator Iterator>
            [[maybe_unused]] constexpr remover const&
            operator()(Iterator const& first, Iterator const& last) const
            {
                sym.lookup->remove(first, last);
                return *this;
            }

            [[maybe_unused]] constexpr remover const&
            operator()(std::basic_string_view<char_type> const s) const
            {
                sym.lookup->remove(s.begin(), s.end());
                return *this;
            }

            [[maybe_unused, deprecated("Don't rely on overloaded comma operator. It's the leftover from the black magic in the 2000s.")]]
            constexpr remover const&
            operator,(std::basic_string_view<char_type> const s) const
            {
                sym.lookup->remove(s.begin(), s.end());
                return *this;
            }

            symbols_parser_impl& sym;
        };

        [[maybe_unused]] adder add;
        [[maybe_unused]] remover remove;

    private:
        template <std::forward_iterator Iterator>
        [[nodiscard]] constexpr value_type* find_impl(Iterator begin, Iterator end) noexcept
        {
            value_type* r = lookup->find(begin, end, case_compare<Encoding>());
            return begin == end ? r : 0;
        }

        template <std::forward_iterator Iterator>
        [[nodiscard]] constexpr value_type const* find_impl(Iterator begin, Iterator end) const noexcept
        {
            value_type const* r = lookup->find(begin, end, case_compare<Encoding>());
            return begin == end ? r : 0;
        }

        std::conditional_t<IsShared, std::shared_ptr<Lookup>, std::unique_ptr<Lookup>> lookup;
        std::string name_;
    };
} // detail

    template <typename Encoding, typename T = unused_type, typename Lookup = tst<typename Encoding::char_type, T>>
    struct shared_symbols_parser
        : detail::symbols_parser_impl<shared_symbols_parser<Encoding, T, Lookup>, true, Encoding, T, Lookup>
    {
        using base_type = detail::symbols_parser_impl<shared_symbols_parser<Encoding, T, Lookup>, true, Encoding, T, Lookup>;
        using base_type::base_type;
        using base_type::operator=;
    };

    template <typename Encoding, typename T = unused_type, typename Lookup = tst<typename Encoding::char_type, T>>
    struct [[deprecated(BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING("symbols_parser"))]]
    symbols_parser : shared_symbols_parser<Encoding, T, Lookup>
    {
        using base_type = shared_symbols_parser<Encoding, T, Lookup>;
        using base_type::base_type;
        using base_type::operator=;
    };

    template <typename Encoding, typename T = unused_type, typename Lookup = tst<typename Encoding::char_type, T>>
    struct unique_symbols_parser
        : detail::symbols_parser_impl<unique_symbols_parser<Encoding, T, Lookup>, false, Encoding, T, Lookup>
    {
        using base_type = detail::symbols_parser_impl<unique_symbols_parser<Encoding, T, Lookup>, false, Encoding, T, Lookup>;
        using base_type::base_type;
        using base_type::operator=;
    };

    template <typename Encoding, typename T, typename Lookup>
    struct get_info<shared_symbols_parser<Encoding, T, Lookup>>
    {
        using result_type = std::string const&;

        [[nodiscard]] constexpr result_type operator()(shared_symbols_parser<Encoding, T, Lookup> const& symbols) const noexcept
        {
            return symbols.name();
        }
    };

    template <typename Encoding, typename T, typename Lookup>
    struct get_info<unique_symbols_parser<Encoding, T, Lookup>>
    {
        using result_type = std::string const&;

        [[nodiscard]] constexpr result_type operator()(unique_symbols_parser<Encoding, T, Lookup> const& symbols) const noexcept
        {
            return symbols.name();
        }
    };

    namespace standard
    {
        template <typename T = unused_type>
        using symbols [[deprecated(BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING("symbols"))]]
            = x3::shared_symbols_parser<char_encoding::standard, T>;

        template <typename T = unused_type>
        using shared_symbols = x3::shared_symbols_parser<char_encoding::standard, T>;

        template <typename T = unused_type>
        using unique_symbols = x3::unique_symbols_parser<char_encoding::standard, T>;
    } // standard

    using standard::symbols;
    using standard::shared_symbols;
    using standard::unique_symbols;

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    namespace standard_wide
    {
        template <typename T = unused_type>
        using symbols [[deprecated(BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING("symbols"))]]
            = x3::shared_symbols_parser<char_encoding::standard_wide, T>;

        template <typename T = unused_type>
        using shared_symbols = x3::shared_symbols_parser<char_encoding::standard_wide, T>;

        template <typename T = unused_type>
        using unique_symbols = x3::unique_symbols_parser<char_encoding::standard_wide, T>;
    } // standard_wide
#endif

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] ascii
    {
        template <typename T = unused_type>
        using symbols [[deprecated(BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING("symbols"))]]
            = x3::shared_symbols_parser<char_encoding::ascii, T>;

        template <typename T = unused_type>
        using shared_symbols = x3::shared_symbols_parser<char_encoding::ascii, T>;

        template <typename T = unused_type>
        using unique_symbols = x3::unique_symbols_parser<char_encoding::ascii, T>;
    } // ascii

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] iso8859_1
    {
        template <typename T = unused_type>
        using symbols [[deprecated(BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING("symbols"))]]
            = x3::shared_symbols_parser<char_encoding::iso8859_1, T>;

        template <typename T = unused_type>
        using shared_symbols = x3::shared_symbols_parser<char_encoding::iso8859_1, T>;

        template <typename T = unused_type>
        using unique_symbols = x3::unique_symbols_parser<char_encoding::iso8859_1, T>;
    } // iso8859_1

#ifdef BOOST_SPIRIT_X3_UNICODE
    namespace unicode {
        template <typename T = unused_type>
        using shared_symbols = x3::shared_symbols_parser<char_encoding::unicode, T>;

        template <typename T = unused_type>
        using unique_symbols = x3::unique_symbols_parser<char_encoding::unicode, T>;
    } // unicode
#endif

} // boost::spirit::x3

#undef BOOST_SPIRIT_X3_IMPLICIT_SHARED_SYMBOLS_WARNING

#if defined(BOOST_MSVC)
# pragma warning(pop)
#endif

#endif
