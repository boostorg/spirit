/*=============================================================================
Copyright (c) 2016  Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_directive.hpp>

#include <string>
#include <iostream>
#include <vector>
#include "test.hpp"

///////////////////////////////////////////////////////////////////////////
// Implementation of a ternary conditional operator if_ for test and
// demonstration purposes.
//
// The implementation shows how to implement an operator
// which lives in a user's namespace (here mxc::qitoo).
///////////////////////////////////////////////////////////////////////////

namespace mxc { namespace qitoo {
    BOOST_SPIRIT_TERMINAL(if_)
}}

namespace boost { namespace spirit {
    template <>
    struct use_function<qi::domain, mxc::qitoo::tag::if_>
        : mpl::true_ {};
}}

namespace mxc {  namespace qitoo {
     namespace fusion = boost::fusion;
     namespace spirit = boost::spirit;
     namespace qi = spirit::qi;

    template <typename Elements>
    struct if_parser : qi::nary_parser<if_parser<Elements>>
    {
        if_parser(Elements elements) : elements(elements) {}

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef typename fusion::result_of::pop_front<Elements>::type optionals;
            typedef typename spirit::traits::build_variant<optionals>::type type;
        };

        template <typename F>
        bool parse_container(F f) const
        {
            // the condition parser does not contribute to the attribute
            if (fusion::at_c<0>(elements)
                .parse(f.f.first, f.f.last, f.f.context, f.f.skipper, qi::unused)) 
            {
                return !f(fusion::at_c<1>(elements));
            }
            else {
                return !f(fusion::at_c<2>(elements));
            }
        }

        template <typename Iterator, typename Context, typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
            , Context& context, Skipper const& skipper
            , Attribute& attr_) const
        {
            spirit::traits::make_container(attr_);

            qi::detail::fail_function<Iterator, Context, Skipper>
                f(first, last, context, skipper);

            if (!parse_container(qi::detail::make_pass_container(f, attr_)))
                return false;

            return true;
        }

        template <typename Context>
        qi::info what(Context& context) const
        {
            qi::info result("if_");
            fusion::for_each(elements,
                spirit::detail::what_function<Context>(result, context));

            return result;
        }

        Elements elements;
    };
}}

namespace boost { namespace spirit { 
    
    namespace qi {
        ///////////////////////////////////////////////////////////////////////////
        // Parser generator: make_xxx function (objects)
        ///////////////////////////////////////////////////////////////////////////
        template <typename Expr, typename Modifiers>
        struct make_composite<mxc::qitoo::tag::if_, Expr, Modifiers>
            : make_nary_composite<Expr, mxc::qitoo::if_parser>
        {
            BOOST_STATIC_ASSERT_MSG(fusion::result_of::size<Expr>::value == boost::mpl::int_<3>::value,
                "qitoo::if_: Invalid number of arguments. Usage qitoo::if_(expr, expr, expr).");
        };
    }

    namespace traits {
        ///////////////////////////////////////////////////////////////////////////
        template <typename Elements>
        struct has_semantic_action<mxc::qitoo::if_parser<Elements> >
            : nary_has_semantic_action<Elements> {};

        ///////////////////////////////////////////////////////////////////////////
        template <typename Elements, typename Attribute, typename Context
            , typename Iterator>
            struct handles_container<mxc::qitoo::if_parser<Elements>, Attribute, Context
            , Iterator>
            : mpl::true_ {};
    }
}}
///////////////////////////////////////////////////////////////////////////
// End of test parser implementation
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Test implementation
///////////////////////////////////////////////////////////////////////////


int main() {

    using mxc::qitoo::if_;
    namespace qi = boost::spirit::qi;
    using namespace boost::spirit::qi;
    using spirit_test::test;
    using spirit_test::test_attr;

    // to test whether if_ works with grammar and rule parameter
    struct test_grammar : qi::grammar<char const*, std::string(), qi::space_type >
    {
        qi::rule<char const*, std::string(), qi::space_type > r;
        test_grammar() : test_grammar::base_type(r)
        {
            r = if_(qi::char_('I'), qi::string("if"), qi::lit('E') >> qi::string("else"));
        }
    };

    std::string s;

    // condition matches, if-parser matches
    // test if condition parser attr gets omitted
    BOOST_TEST(test_attr("I if", if_(qi::char_('I'), qi::string("if"), qi::lit('E') >> qi::string("else")), s, qi::space));
    BOOST_TEST(s == "if");

    //same test using grammar
    test_grammar g;
    s.clear();
    BOOST_TEST(test_attr("I if", g, s, qi::space));
    BOOST_TEST(s == "if");

    //same test using grammar
    s.clear();
    BOOST_TEST(test_attr("I if", g.r, s, qi::space));
    BOOST_TEST(s == "if");

    // condition matches, if-parser fails
    BOOST_TEST(!test("I mismatch", if_(lit('I'), qi::string("if"), qi::lit('E') >> qi::string("else")), qi::space));

    // condition fails, else-parser matches
    s.clear();
    BOOST_TEST(test_attr("E else", if_(lit('I'), qi::string("if"), qi::lit('E') >> qi::string("else")), s, qi::space));
    BOOST_TEST(s == "else");

    // condition fails, else-parser fails
    BOOST_TEST(!test("E mismatch", if_(lit('I'), qi::string("if"), qi::lit('E') >> qi::string("else")), qi::space));

    // if embedded in sequence, string matches, condition matches, if-parser matches
    std::vector<std::string> v;
    BOOST_TEST(test_attr(
        "w I if", 
        qi::string("w") 
            >> if_(qi::lit('I') 
                , qi::raw[qi::string("if")]
                , qi::string("else") >> qi::lit(",;"))
        , v, qi::space));
    BOOST_TEST(v.size() == 2 &&
        v[0] == "w" && v[1] == "if");

    // nested if_, outer condition fails, inner condition matches, string matches
    s.clear();
    BOOST_TEST(test_attr(
        "E else"
        , if_(qi::lit('I') , 
            qi::string("if")
            , if_(qi::lit('E')
                , qi::string("else")
                , qi::string("other")))
            , s, qi::space
            ));
    BOOST_TEST(s == "else");

    return boost::report_errors();
}
