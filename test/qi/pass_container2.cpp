//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "test.hpp"

using namespace spirit_test;

inline bool compare(std::vector<char> const& v, std::string const& s)
{
    return v.size() == s.size() && std::equal(v.begin(), v.end(), s.begin());
}

int main()
{
    using boost::spirit::qi::char_;
    using boost::spirit::qi::omit;

    {
        std::vector<std::vector<char> > v1;
        BOOST_TEST(test_attr("abc,def,gh", *~char_(',') % ',', v1) &&
            v1.size() == 3 && 
            compare(v1[0], "abc") && 
            compare(v1[1], "def") && 
            compare(v1[2], "gh"));

        std::vector<std::string> v2;
        BOOST_TEST(test_attr("abc,def,gh", *~char_(',') % ',', v2) && 
            v2.size() == 3 && v2[0] == "abc" && v2[1] == "def" && v2[2] == "gh");

        BOOST_TEST(test("abc,def,gh", *~char_(',') % ','));
        BOOST_TEST(test("abc,def,gh", omit[*~char_(',')] % ','));
    }

    {
        std::vector<char> v1;
        BOOST_TEST(test_attr("a", char_ >> -(char_ % ','), v1) &&
            compare(v1, "a"));
        v1.clear();
        BOOST_TEST(test_attr("ab,c", char_ >> -(char_ % ','), v1) &&
            compare(v1, "abc"));
        v1.clear();
        BOOST_TEST(test_attr("a", char_ >> -char_, v1) &&
            compare(v1, "a"));
        v1.clear();
        BOOST_TEST(test_attr("ab", char_ >> -char_, v1) &&
            compare(v1, "ab"));

        std::vector<boost::optional<char> > v2;
        BOOST_TEST(test_attr("a", char_ >> -char_, v2) &&
            v2.size() == 2 && 
            boost::get<char>(v2[0]) == 'a' && 
            !v2[1]);
        v2.clear();
        BOOST_TEST(test_attr("ab", char_ >> -char_, v2) &&
            v2.size() == 2 && 
            boost::get<char>(v2[0]) == 'a' && 
            boost::get<char>(v2[1]) == 'b');

        std::string s;
        BOOST_TEST(test_attr("a", char_ >> -(char_ % ','), s) &&
            s == "a");
        s.clear();
        BOOST_TEST(test_attr("ab,c", char_ >> -(char_ % ','), s) &&
            s == "abc");
        s.clear();
        BOOST_TEST(test_attr("ab", char_ >> -char_, s) &&
            s == "ab");
        s.clear();
        BOOST_TEST(test_attr("a", char_ >> -char_, s) &&
            s ==  "a");

        BOOST_TEST(test("a", char_ >> -(char_ % ',')));
        BOOST_TEST(test("ab,c", char_ >> -(char_ % ',')));
        BOOST_TEST(test("a", char_ >> -char_));
        BOOST_TEST(test("ab", char_ >> -char_));
    }

    {
        using boost::spirit::qi::eps;

        std::vector<char> v;
        BOOST_TEST(test_attr("a", char_ >> ((char_ % ',') | eps), v) &&
            compare(v, "a"));
        v.clear();
        BOOST_TEST(test_attr("ab,c", char_ >> ((char_ % ',') | eps), v) &&
            compare(v, "abc"));

        std::string s;
        BOOST_TEST(test_attr("a", char_ >> ((char_ % ',') | eps), s) &&
            s == "a");
        s.clear();
        BOOST_TEST(test_attr("ab,c", char_ >> ((char_ % ',') | eps), s) &&
            s == "abc");

        BOOST_TEST(test("a", char_ >> ((char_ % ',') | eps)));
        BOOST_TEST(test("ab,c", char_ >> ((char_ % ',') | eps)));
    }

    {
        std::vector<char> v1;
        BOOST_TEST(test_attr("abc1,abc2", 
                *~char_(',') >> *(',' >> *~char_(',')), v1) &&
            compare(v1, "abc1abc2"));

        std::vector<std::string> v2;
        BOOST_TEST(test_attr("abc1,abc2", 
                *~char_(',') >> *(',' >> *~char_(',')), v2) &&
            v2.size() == 2 && 
            v2[0] == "abc1" &&
            v2[1] == "abc2");

        std::string s;
        BOOST_TEST(test_attr("abc1,abc2", 
                *~char_(',') >> *(',' >> *~char_(',')), s) &&
            s == "abc1abc2");
    }

    {
        using boost::spirit::qi::alpha;
        using boost::spirit::qi::digit;

        std::vector<char> v1;
        BOOST_TEST(test_attr("ab1cd2", *(alpha >> alpha | +digit), v1) &&
            compare(v1, "ab1cd2"));
        v1.clear();
        BOOST_TEST(test_attr("ab1cd2", *(alpha >> alpha | digit), v1) &&
            compare(v1, "ab1cd2"));

        std::string s1;
        BOOST_TEST(test_attr("ab1cd2", *(alpha >> alpha | +digit), s1) &&
            s1 == "ab1cd2");
        s1.clear();
        BOOST_TEST(test_attr("ab1cd2", *(alpha >> alpha | digit), s1) &&
            s1 == "ab1cd2");

// doesn't work yet
//         std::vector<std::vector<char> > v2;
//         BOOST_TEST(test_attr("ab1cd123", *(alpha >> alpha | +digit), v2) &&
//             v2.size() == 4 &&
//             compare(v2[0], "ab") &&
//             compare(v2[1], "1") &&
//             compare(v2[2], "cd") &&
//             compare(v2[3], "123"));
// 
//         std::vector<std::string> v3;
//         BOOST_TEST(test_attr("ab1cd123", *(alpha >> alpha | +digit), v3) &&
//             v3.size() == 4 &&
//             v3[0] == "ab" &&
//             v3[1] == "1" &&
//             v3[2] == "cd" &&
//             v3[3] == "123");
    }

    {
    }

    return boost::report_errors();
}

