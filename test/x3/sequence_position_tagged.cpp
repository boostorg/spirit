/*=============================================================================
    Copyright (c) 2022 Denis Mikhailov

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/comparison.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/next.hpp>

#include <string>
#include <iostream>
#include "test.hpp"
#include "utils.hpp"

int
main()
{
    using boost::spirit::x3::unused_type;

    using boost::spirit::x3::char_;
    using boost::spirit::x3::space;
    using boost::spirit::x3::string;
    using boost::spirit::x3::attr;
    using boost::spirit::x3::omit;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::unused;
    using boost::spirit::x3::int_;
    using boost::spirit::x3::float_;
    using boost::spirit::x3::no_case;
    using boost::spirit::x3::rule;
    using boost::spirit::x3::alnum;
    
    using boost::spirit::x3::position_tagged;

    using boost::spirit::x3::traits::attribute_of;

    using boost::fusion::vector;
    using boost::fusion::deque;
    using boost::fusion::at_c;
    using boost::fusion::begin;
    using boost::fusion::next;

    using spirit_test::test;
    using spirit_test::test_attr;

    {
        vector<char, char, position_tagged> attr;
        BOOST_TEST((test_attr("ab", char_ >> char_, attr)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'b'));
    }

#ifdef BOOST_SPIRIT_COMPILE_ERROR_CHECK
    {
        // Compile check only
        vector<char, char, position_tagged> attr;

        // error: attr does not have enough elements
        test_attr("abc", char_ >> char_ >> char_, attr);
    }
#endif

    {
        vector<char, char, char, position_tagged> attr;
        BOOST_TEST((test_attr(" a\n  b\n  c", char_ >> char_ >> char_, attr, space)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'b'));
        BOOST_TEST((at_c<2>(attr) == 'c'));
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char, position_tagged> attr;
        BOOST_TEST((test_attr("abc", char_ >> 'b' >> char_, attr)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'c'));
    }

    {
        // 'b' has an unused_type. unused attributes are not part of the sequence
        vector<char, char, position_tagged> attr;
        BOOST_TEST((test_attr("acb", char_ >> char_ >> 'b', attr)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'c'));
    }

    {
        // "hello" has an unused_type. unused attributes are not part of the sequence
        vector<char, char, position_tagged> attr;
        BOOST_TEST((test_attr("a hello c", char_ >> "hello" >> char_, attr, space)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'c'));
    }

#ifdef BOOST_SPIRIT_COMPILE_ERROR_CHECK
    {
        // a single element fusion sequence
        vector<char, position_tagged> attr;
        BOOST_TEST((test_attr("ab", char_ >> 'b', attr)));
        BOOST_TEST((at_c<0>(attr) == 'a'));
    }
#endif

    // TODO: enable it
#if 0
    {
        // make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. Edit JDG 2014:
        // actually he issue here is that if the rhs in this case a rule
        // (r), it should get it (i.e. the sequence parser should not
        // unwrap it). It's odd that the RHS (r) does not really have a
        // single element tuple (it's a deque<char, int>), so the original
        // comment is not accurate.

        typedef deque<char, int, position_tagged> attr_type;
        attr_type fv;

        auto r = rule<class r_id, attr_type>()
            = char_ >> ',' >> int_;

        BOOST_TEST(test_attr("test:x,1", "test:" >> r, fv));
        BOOST_TEST(*begin(fv) == 'x');
        BOOST_TEST(*next(begin(fv)) == 1);
    }
#endif

#ifdef BOOST_SPIRIT_COMPILE_ERROR_CHECK
    {
        // make sure single element tuples get passed through if the rhs
        // has a single element tuple as its attribute. This is a correction
        // of the test above.

        typedef deque<int, position_tagged> attr_type;
        attr_type fv;

        auto r = rule<class r_id, attr_type>()
            = int_;

        BOOST_TEST(test_attr("test:1", "test:" >> r, fv));
        BOOST_TEST(*begin(fv) == 1);
    }
#endif

    { // non-flat optional
        vector<int, boost::optional<vector<int, int>>, position_tagged> v;
        auto const p = int_ >> -(':' >> int_ >> '-' >> int_);
        BOOST_TEST(test_attr("1:2-3", p, v))
            && BOOST_TEST(at_c<1>(v)) && BOOST_TEST_EQ(at_c<0>(*at_c<1>(v)), 2);
    }

    { // optional with container attribute
        vector<char, boost::optional<std::string>, position_tagged> v;
        auto const p = char_ >> -(':' >> +char_);
        BOOST_TEST(test_attr("x", p, v))
            && BOOST_TEST(!at_c<1>(v));
        v = {};
        BOOST_TEST(test_attr("x:abc", p, v))
            && BOOST_TEST(at_c<1>(v)) && BOOST_TEST(*at_c<1>(v) == "abc");
    }

    return boost::report_errors();
}
