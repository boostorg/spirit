/*=============================================================================
    Copyright (c) 2019 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_PSEUDO_ATTRIBUTE_OF_MAY_15_2019_1012PM)
#define BOOST_SPIRIT_X3_PSEUDO_ATTRIBUTE_OF_MAY_15_2019_1012PM

#include <iterator>

namespace boost::spirit::x3::traits
{
    // Pseudo attributes are placeholders for parsers that can only know
    // its actual attribute at parse time. This trait customization point
    // provides a mechanism to convert the trait to the actual trait at
    // parse time.
    template <typename Context, typename Attribute, std::forward_iterator It, std::sentinel_for<It> Se = It, typename Enable = void>
    struct pseudo_attribute
    {
        using attribute_type = Attribute;
        using type = Attribute;

        [[nodiscard]] static constexpr type&& call(It&, Se const&, Attribute&& attribute) noexcept
        {
            return static_cast<type&&>(attribute);
        }
    };

    template <typename Context, typename Attribute, std::forward_iterator It, std::sentinel_for<It> Se>
    using pseudo_attribute_t = typename pseudo_attribute<Context, Attribute, It, Se>::type;

} // boost::spirit::x3::traits

#endif
