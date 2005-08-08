//
//  Copyright (c) 2005 João Abecasis
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/utility/grammar_def.hpp> 

struct my_grammar1
    : boost::spirit::grammar<my_grammar1>
{
    template <typename Scanner>
    struct definition
        : boost::spirit::grammar_def<
            boost::spirit::rule<Scanner>,
            boost::spirit::same
        >
    {
        definition(my_grammar1 const &)
        {
            BOOST_SPIRIT_DEBUG_NODE(start_rule1);
            BOOST_SPIRIT_DEBUG_NODE(start_rule2);

            start_rule1 = boost::spirit::str_p("int");
            start_rule2 = boost::spirit::int_p;

            this->start_parsers(start_rule1, start_rule2);
        }

        boost::spirit::rule<Scanner>
            start_rule1,
            start_rule2;
    };
};

struct my_closure : boost::spirit::closure<my_closure, int>
{
    member1 value;
};

struct my_grammar2
    : boost::spirit::grammar<my_grammar2, my_closure::context_t>
{
    template <typename Scanner>
    struct definition
        : boost::spirit::grammar_def<
            boost::spirit::rule<Scanner>,
            boost::spirit::same
        >
    {
        definition(my_grammar2 const &)
        {
            BOOST_SPIRIT_DEBUG_NODE(start_rule1);
            BOOST_SPIRIT_DEBUG_NODE(start_rule2);

            start_rule1 = boost::spirit::str_p("int");
            start_rule2 = boost::spirit::int_p;

            this->start_parsers(start_rule1, start_rule2);
        }

        boost::spirit::rule<Scanner>
            start_rule1,
            start_rule2;
    };
};

int main()
{
    my_grammar1 g1;
    my_grammar2 g2;

    BOOST_SPIRIT_DEBUG_NODE(g1);
    BOOST_SPIRIT_DEBUG_NODE(g2);

    parse(
        "int 5",
        g1.use_parser<0>() >> g2.use_parser<1>(),
        boost::spirit::space_p
    );
}
