/*=============================================================================
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_stream.hpp>
#include <boost/spirit/include/qi_match.hpp>
#include <boost/spirit/include/qi_match_auto.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <list>

#include <boost/detail/lightweight_test.hpp>

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename Expr>
bool test(Char const *toparse, Expr const& expr)
{
    namespace spirit = boost::spirit;
    BOOST_SPIRIT_ASSERT_MATCH(spirit::qi::domain, Expr);

    std::istringstream istrm(toparse);
    istrm.unsetf(std::ios::skipws);
    istrm >> spirit::qi::compile<spirit::qi::domain>(expr);
    return istrm.good() || istrm.eof();
}

template <typename Char, typename Expr, typename CopyExpr, typename CopyAttr
  , typename Skipper, typename Attribute>
bool test(Char const *toparse,
    boost::spirit::qi::detail::match_manip<
        Expr, CopyExpr, CopyAttr, Skipper, Attribute> const& mm)
{
    std::istringstream istrm(toparse);
    istrm.unsetf(std::ios::skipws);
    istrm >> mm;
    return istrm.good() || istrm.eof();
}

///////////////////////////////////////////////////////////////////////////////
bool is_list_ok(std::list<char> const& l)
{
    std::list<char>::const_iterator cit = l.begin();
    if (cit == l.end() || *cit != 'a')
        return false;
    if (++cit == l.end() || *cit != 'b')
        return false;

    return ++cit != l.end() && *cit == 'c';
}

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::match;
    using boost::spirit::qi::phrase_match;
    using boost::spirit::qi::typed_stream;
    using boost::spirit::qi::stream;
    using boost::spirit::qi::int_;

    using namespace boost::spirit::ascii;
    namespace fusion = boost::fusion;
    namespace phx = boost::phoenix;

    {
        char c = '\0';
        BOOST_TEST(test( "a",
            char_[phx::ref(c) = _1]
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( "a",
            match(char_[phx::ref(c) = _1])
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( " a",
            phrase_match(char_[phx::ref(c) = _1], space)
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( "a",
            match(char_, c)
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( " a",
            phrase_match(char_, space, c)
        ) && c == 'a');
    }

    {
        ///////////////////////////////////////////////////////////////////////
        typedef typed_stream<char> char_stream_type;
        char_stream_type const char_stream = char_stream_type();

        typedef typed_stream<int> int_stream_type;
        int_stream_type const int_stream = int_stream_type();

        ///////////////////////////////////////////////////////////////////////
        char c = '\0';
        BOOST_TEST(test( "a",
            char_stream[phx::ref(c) = _1]
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( "a",
            match(char_stream[phx::ref(c) = _1])
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( " a",
            phrase_match(char_stream[phx::ref(c) = _1], space)
        ) && c == 'a');

        int i = 0;
        BOOST_TEST(test( "42",
            int_stream[phx::ref(i) = _1]
        ) && i == 42);

        i = 0;
        BOOST_TEST(test( "42",
            match(int_stream[phx::ref(i) = _1])
        ) && i == 42);

        i = 0;
        BOOST_TEST(test( " 42",
            phrase_match(int_stream[phx::ref(i) = _1], space)
        ) && i == 42);

        ///////////////////////////////////////////////////////////////////////
        c = '\0';
        BOOST_TEST(test( "a",
            match(stream, c)
        ) && c == 'a');

        c = '\0';
        BOOST_TEST(test( " a",
            phrase_match(stream, space, c)
        ) && c == 'a');

        i = 0;
        BOOST_TEST(test( "42",
            match(stream, i)
        ) && i == 42);

        i = 0;
        BOOST_TEST(test( " 42",
            phrase_match(stream, space, i)
        ) && i == 42);
    }

    {
        char a = '\0', b = '\0';
        BOOST_TEST(test( "ab",
            char_[phx::ref(a) = _1] >> char_[phx::ref(b) = _1]
        ) && a == 'a' && b == 'b');

        a = '\0', b = '\0';
        BOOST_TEST(test( "ab",
            match(char_[phx::ref(a) = _1] >> char_[phx::ref(b) = _1])
        ) && a == 'a' && b == 'b');

        a = '\0', b = '\0';
        BOOST_TEST(test( " a b",
            phrase_match(char_[phx::ref(a) = _1] >> char_[phx::ref(b) = _1], space)
        ) && a == 'a' && b == 'b');

        fusion::vector<char, char> t;
        BOOST_TEST(test( "ab",
            match(char_ >> char_, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b');

        t = fusion::vector<char, char>();
        BOOST_TEST(test( " a b",
            phrase_match(char_ >> char_, space, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b');
    }

    {
        char a = '\0', b = '\0', c = '\0';
        BOOST_TEST(test( "abc",
            char_[phx::ref(a) = _1] >> char_[phx::ref(b) = _1] >> char_[phx::ref(c) = _1]
        ) && a == 'a' && b == 'b' && c == 'c');

        BOOST_TEST(test( "abc",
            match(char_('a') >> char_('b') >> char_('c'))
        ));

        BOOST_TEST(test( " a b c",
            phrase_match(char_('a') >> char_('b') >> char_('c'), space)
        ));

        BOOST_TEST(!test( "abc",
            match(char_('a') >> char_('b') >> char_('d'))
        ));

        BOOST_TEST(!test( " a b c",
            phrase_match(char_('a') >> char_('b') >> char_('d'), space)
        ));

        fusion::vector<char, char, char> t;
        BOOST_TEST(test( "abc",
            match(char_ >> char_ >> char_, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b' && fusion::at_c<2>(t) == 'c');

        t = fusion::vector<char, char, char>();
        BOOST_TEST(test( " a b c",
            phrase_match(char_ >> char_ >> char_, space, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b' && fusion::at_c<2>(t) == 'c');

        t = fusion::vector<char, char, char>();
        BOOST_TEST(test( "abc",
            match(t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b' && fusion::at_c<2>(t) == 'c');

        t = fusion::vector<char, char, char>();
        BOOST_TEST(test( " a b c",
            phrase_match(t, space)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 'b' && fusion::at_c<2>(t) == 'c');
    }

    {
        char a = '\0';
        int i = 0;
        BOOST_TEST(test( "a2",
            (char_ >> int_)[phx::ref(a) = _1, phx::ref(i) = _2]
        ) && a == 'a' && i == 2);

        fusion::vector<char, int> t;
        BOOST_TEST(test( "a2",
            match(char_ >> int_, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 2);

        t = fusion::vector<char, int>();
        BOOST_TEST(test( " a 2",
            phrase_match(char_ >> int_, space, t)
        ) && fusion::at_c<0>(t) == 'a' && fusion::at_c<1>(t) == 2);

        BOOST_TEST(!test( "a2",
            match(char_ >> alpha, t)
        ));
        BOOST_TEST(!test( " a 2",
            phrase_match(char_ >> alpha, space, t)
        ));
    }

    {
        // parse elements of a vector
        std::vector<char> v;
        BOOST_TEST(test( "abc",
            (*char_)[phx::ref(v) = _1]
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        v.clear();
        BOOST_TEST(test( "abc",
            match(*char_, v)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        v.clear();
        BOOST_TEST(test( " a b c",
            phrase_match(*char_, space, v)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        v.clear();
        BOOST_TEST(test( "abc",
            match(v)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        v.clear();
        BOOST_TEST(test( " a b c",
            phrase_match(v, space)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        // parse a comma separated list of vector elements
        v.clear();
        BOOST_TEST(test( "a,b,c",
            match(char_ % ',', v)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        v.clear();
        BOOST_TEST(test( " a , b , c",
            phrase_match(char_ % ',', space, v)
        ) && 3 == v.size() && v[0] == 'a' && v[1] == 'b' && v[2] == 'c');

        // output all elements of a list
        std::list<char> l;
        BOOST_TEST(test( "abc",
            match(*char_, l)
        ) && 3 == l.size() && is_list_ok(l));

        l.clear();
        BOOST_TEST(test( " a b c",
            phrase_match(*char_, space, l)
        ) && 3 == l.size() && is_list_ok(l));

        l.clear();
        BOOST_TEST(test( "abc",
            match(l)
        ) && 3 == l.size() && is_list_ok(l));

        l.clear();
        BOOST_TEST(test( " a b c",
            phrase_match(l, space)
        ) && 3 == l.size() && is_list_ok(l));
    }

    return boost::report_errors();
}

