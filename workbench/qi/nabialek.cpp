/*=============================================================================
Copyright (c) 2016  Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

// it may be ugly, but it's deprecated :)
// without that kwd and / operator don't work
#define BOOST_RESULT_OF_USE_TR1
// this one does not work
//#define BOOST_RESULT_OF_USE_DECLTYPE_WITH_TR1_FALLBACK

#include "../measure.hpp"
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <libs/spirit/test/qi/nabialek.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/spirit/repository/include/qi_keywords.hpp>


// enable this define to activate asserts to check
// if the parsers succeed and (first == last)

#define CHECK_RESULT

namespace
{
    typedef std::string::const_iterator iterator_type;

    std::string source("name = Tom age = 27 sex = m age = 32 sex = f name = Sue");

    namespace qi = boost::spirit::qi;
    namespace repo = boost::spirit::repository::qi;
    using mxc::qitoo::nabialek;

    ///////////////////////////////////////////////////////////////////////////
    struct tg0 : qi::grammar<iterator_type, qi::locals<qi::rule<iterator_type, qi::space_type>*>
        ,qi::space_type>
    {
        qi::rule<iterator_type,  qi::space_type> name;
        qi::rule<iterator_type,  qi::space_type> age;
        qi::rule<iterator_type,  qi::space_type> sex;
        qi::rule<iterator_type, qi::space_type> kwd;
        qi::rule<iterator_type,  qi::locals<qi::rule<iterator_type, qi::space_type>*>, qi::space_type > start;
        qi::symbols<char, qi::rule<iterator_type, qi::space_type>*> keywords;


        tg0() : tg0::base_type(start)
        {
            using qi::_1;
            using qi::_a;

            name = qi::lit('=') >> qi::lexeme[+qi::alpha];
            age = qi::lit('=') >> qi::int_;
            sex = qi::lit('=') >> qi::char_("mf");

            keywords.add("name", &name)("age", &age)("sex", &sex);

            start = +( keywords[_a = _1] >> qi::lazy(*_a));
            kwd = repo::kwd("name")[name] / repo::kwd("age")[age] / repo::kwd("sex")[sex];
        }
    };

    tg0 g0;

    struct nabialek_trick : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g0.start, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_slash_rule : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g0.kwd, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_slash_direct : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, repo::kwd("name")[qi::lit('=') >> qi::lexeme[+qi::alpha]] / repo::kwd("age")[qi::lit('=') >> qi::int_] / repo::kwd("sex")[qi::lit('=') >> qi::char_("mf")], qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct tg1 : qi::grammar<iterator_type, qi::space_type >
    {
        qi::rule<iterator_type,  qi::space_type> name;
        qi::rule<iterator_type,  qi::space_type> age;
        qi::rule<iterator_type,  qi::space_type> sex;
        qi::rule<iterator_type,  qi::space_type> start;
        qi::symbols<char, int> keywords;

        tg1() : tg1::base_type(start)
        {
            keywords.add("name", 1)("age", 2)("sex", 3);

            name = qi::lit('=') >> qi::lexeme[+qi::alpha];
            age = qi::lit('=') >> qi::int_;
            sex = qi::lit('=') >> qi::char_("mf");

            start = +nabialek(keywords, name, age, sex);
        }
    };

    tg1 g1;

    struct nab_rules : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, +nabialek(g1.keywords, g1.name, g1.age, g1.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nab_rules_ni : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g1.start, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct nab_direct : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last,
                +nabialek(g1.keywords,
                    qi::lit('=') >> qi::lexeme[+qi::alpha],
                    qi::lit('=') >> qi::int_,
                    qi::lit('=') >> qi::char_("mf"))
                , qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct tg2 : qi::grammar<iterator_type, qi::space_type >
    {
        qi::rule<iterator_type, qi::space_type> name;
        qi::rule<iterator_type, qi::space_type> age;
        qi::rule<iterator_type, qi::space_type> sex;
        qi::rule<iterator_type, qi::space_type> start;
        qi::symbols<char, int> keywords;

        tg2() : tg2::base_type(start)
        {
            keywords.add("name", 1)("age", 2)("sex", 3);

            name %= qi::lit('=') >> qi::lexeme[+qi::alpha];
            age %= qi::lit('=') >> qi::int_;
            sex %= qi::lit('=') >> qi::char_("mf");

            start = +nabialek(keywords, name, age, sex);
        }
    };

    tg2 g2;

    struct nab_rules_me : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, +nabialek(g2.keywords, g2.name, g2.age, g2.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nab_rules_me_ni : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g2.start, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct tg3 : qi::grammar<iterator_type, qi::space_type >
    {
        qi::rule<iterator_type,  qi::space_type> name;
        qi::rule<iterator_type,  qi::space_type> age;
        qi::rule<iterator_type,  qi::space_type> sex;
        qi::rule<iterator_type,  qi::space_type> start;
        qi::symbols<char, int> keywords;

        tg3() : tg3::base_type(start)
        {
            keywords.add("name", 1)("age", 2)("sex", 3);

            name = qi::lit('=') > qi::lexeme[+qi::alpha];
            age = qi::lit('=') > qi::int_;
            sex = qi::lit('=') > qi::char_("mf");

            start = +nabialek(keywords, name, age, sex);
        }
    };

    tg3 g3;

    struct nab_rules_expect : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, +nabialek(g3.keywords, g3.name, g3.age, g3.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nab_rules_expect_ni : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g3.start, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct tg4 : qi::grammar<iterator_type, qi::space_type >
    {
        qi::rule<iterator_type, qi::space_type> name;
        qi::rule<iterator_type, qi::space_type> age;
        qi::rule<iterator_type, qi::space_type> sex;
        qi::rule<iterator_type, qi::space_type> start;
        qi::symbols<char, int> keywords;

        tg4() : tg4::base_type(start)
        {
            keywords.add("name", 1)("age", 2)("sex", 3);
            
            name %= qi::lit('=') > qi::lexeme[+qi::alpha];
            age %= qi::lit('=') > qi::int_;
            sex %= qi::lit('=') > qi::char_("mf");

            start = +nabialek(keywords, name, age, sex);
        }
    };

    tg4 g4;

    struct nab_rules_expect_me : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, +nabialek(g4.keywords, g4.name, g4.age, g4.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nab_rules_expect_me_ni : test::base
    {
        void benchmark()
        {
            iterator_type first = source.begin();
            iterator_type last = source.end();
            bool success = qi::phrase_parse(first, last, g4.start, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };
}

int main(int argc, char **argv)
{
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "// Benchmark" << std::endl;
    std::cout << "// Example nabialek operator vs nabialek trick vs kwd and / combination" << std::endl;
    std::cout << "//" << std::endl;
    std::cout << "// Please have a look at main() for an explanation of the test cases" << std::endl;
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;

    BOOST_SPIRIT_TEST_BENCHMARK(
        10000000,     // This is the maximum repetitions to execute
         (nabialek_trick)               // orginal nabialek trick with pointers to parsers
         (keyword_slash_rule)           // kwd directice with divide operator using rules as argument parsers
         (keyword_slash_direct)         // kwd directive using parser expressions directly
         (nab_direct)                   // direct instantiation of nabialek and all arguments
         (nab_rules)                    // direct instantiation of nabialek with rule arguments
         (nab_rules_ni)                 // same as nab_rules but using a start rule defining op and args
         (nab_rules_expect)             // same as nab_rules but rules use expect instead of sequence
         (nab_rules_expect_ni)          // same as nab_rules_expect but using as start rule defining op and args

        // No test does attribute handling. Attribute handling slows things down significantly on all 
        // tested parsers. The _me test cases are not relevant without attribute handling and therefore
        // commented out.

//         (nab_rules_me)                 // same as nab_rules but rules use %= instead of =
//         (nab_rules_me_ni)              // same as nab_rules_me but using a start rule defining op and args 
//         (nab_rules_expect_me)          // same as nab_rules_expect and rules use %= instead of =
//         (nab_rules_expect_me_ni)       // same as nab_rules_expect_me but using start rule defining op and args
    )

        // This is ultimately responsible for preventing all the test code
        // from being optimized away.  Change this to return 0 and you
        // unplug the whole test's life support system.
        return test::live_code != 0;
}

