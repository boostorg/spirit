/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "rule_separate_tu_grammar.hpp"

#include <boost/core/lightweight_test.hpp>

int main()
{
    char const* const s = "*", * const end = s + std::strlen(s);

    BOOST_TEST(parse(s, end, unused_attr::skipper));
    BOOST_TEST(parse(s, end, unused_attr::grammar));
    BOOST_TEST(phrase_parse(s, end, unused_attr::grammar, unused_attr::skipper));

    return boost::report_errors();
}
