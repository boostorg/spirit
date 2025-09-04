/*=============================================================================
    Copyright (c) 2001-2017 Joel de Guzman
    Copyright (c) 2017 think-cell GmbH
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
// TODO: full support for `std::ranges::subrange` requires follow-up PR
//#ifndef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
//# define BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
//#endif

#include <boost/spirit/home/x3.hpp>

#include <boost/core/lightweight_test.hpp>
#include <boost/range/iterator_range.hpp>

#include <ranges>
#include <algorithm>
#include <iostream>
#include <string>

int main()
{
    using boost::spirit::x3::raw;
    using boost::spirit::x3::eps;
    using boost::spirit::x3::eoi;
    using boost::spirit::x3::standard::upper;
    using boost::spirit::x3::repeat;
    using boost::spirit::x3::parse;

    std::string input = "abcde";
    auto const rng = input | std::views::transform([](char c) {
        return c < 'a' || 'z' < c ? c : static_cast<char>(c - 'a' + 'A');
    });
    using range_type = decltype(rng);
    static_assert(std::ranges::forward_range<range_type>);

    {
        std::string str;
        BOOST_TEST((parse(std::ranges::begin(rng), std::ranges::end(rng), +upper >> eoi, str)));
        BOOST_TEST(("ABCDE"==str));
    }

    {
        // TODO: full support for `std::ranges::subrange` requires follow-up PR
        //std::ranges::subrange<std::ranges::iterator_t<range_type>> str;
        boost::iterator_range<std::ranges::iterator_t<range_type>> str;

        BOOST_TEST((parse(std::ranges::begin(rng), std::ranges::end(rng), raw[+upper >> eoi], str)));
        BOOST_TEST((std::ranges::equal(std::string("ABCDE"), str)));
    }

    {
        BOOST_TEST((parse(std::ranges::begin(rng), std::ranges::end(rng), (repeat(6)[upper] | repeat(5)[upper]) >> eoi)));
    }

    return boost::report_errors();
}
