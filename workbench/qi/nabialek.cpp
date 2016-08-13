/*=============================================================================
Copyright (c) 2016  Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/


// without that kwd and / operator don't work
#define BOOST_RESULT_OF_USE_TR1
// this one does not do the trick 
//#define BOOST_RESULT_OF_USE_DECLTYPE_WITH_TR1_FALLBACK

#include "../measure.hpp"
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <libs/spirit/test/qi/nabialek.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/spirit/repository/include/qi_keywords.hpp>


// enable this define to activate asserts which check if the parsers 
// and parse the whole input (for debugging)
//#define CHECK_RESULT

// change this define to set the number max number of runs of the
// benchmark
#define NABIALEK_MAX_BENCHMARK 10000000


namespace
{
    char* source = "name = Tom age = 27 sex = m age = 32 sex = f name = Sue";
    char* last;

    namespace qi = boost::spirit::qi;
    namespace repo = boost::spirit::repository::qi;
    using mxc::qitoo::nabialek;

    ///////////////////////////////////////////////////////////////////////////
    // rule container


    ///////////////////////////////////////////////////////////////////////////
    // rule container
    struct tg 
    {
        qi::rule<char*, qi::locals<qi::rule<char*, qi::space_type>>, qi::space_type > ntrick_org;
        qi::rule<char*, qi::locals<qi::rule<char*, qi::space_type>*>, qi::space_type > ntrick_ptr;

        std::vector<qi::rule<char*, qi::space_type>> rules;
        qi::rule<char*, qi::space_type> name, age, sex;

        qi::rule<char*, qi::space_type> 
            alternatives_r, alternatives_ni, kwd_r, kwd_ni, nabialek_r, nabialek_ni;

        qi::symbols<char, qi::rule<char *, qi::space_type>>  sym1;
        qi::symbols<char, qi::rule<char*, qi::space_type>*> sym2;
        qi::symbols<char, int>                                      sym3;

        tg() { 
            using qi::_1;
            using qi::_a;


            name = qi::lit('=') >> qi::lexeme[+qi::alpha];
            age = qi::lit('=') >> qi::int_;
            sex = qi::lit('=') >> qi::char_("mf");

            // for _ni test cases
            kwd_ni = repo::kwd("name")[name] / repo::kwd("age")[age] / repo::kwd("sex")[sex];
            nabialek_ni = +nabialek(sym3, name, age, sex);
            alternatives_ni = +(("name" >> name) | ("age" >> age) | ("sex" >> sex));

            // for _r test cases
            kwd_r = repo::kwd("name")[qi::lit('=') >> qi::lexeme[+qi::alpha]] / repo::kwd("age")[qi::lit('=') >> qi::int_] / repo::kwd("sex")[qi::lit('=') >> qi::char_("mf")];
            alternatives_r = +(("name" >> qi::lit('=') >> qi::lexeme[+qi::alpha]) | ("age" >> qi::lit('=') >> qi::int_) | ("sex" >> qi::lit('=') >> qi::char_("mf")));
            nabialek_r = +nabialek(sym3, qi::lit('=') >> qi::lexeme[+qi::alpha], qi::lit('=') >> qi::int_, qi::lit('=') >> qi::char_("mf"));

            sym2.add("name", &name)("age", &age)("sex", &sex);
            sym1.add("name", name)("age", age)("sex", sex);
            sym3.add("name", 1)("age", 2)("sex", 3);

            ntrick_org = +(sym1[_a = _1] >> qi::lazy(_a));
            ntrick_ptr = +(sym2[_a = _1] >> qi::lazy(*_a));
        }

    };

    tg g;

    ///////////////////////////////////////////////////////////////////////////
    // nabialek trick
    struct nabialek_trick_org : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.ntrick_org, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_trick_ptr : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.ntrick_ptr, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // qi alternative
    struct alternative_d : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, +(("name" >> qi::lit('=') >> qi::lexeme[+qi::alpha]) | ("age" >> qi::lit('=') >> qi::int_) | ("sex" >> qi::lit('=') >> qi::char_("mf"))), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_rr : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, +(("name" >> g.name) | ("age" >> g.age) | ("sex" >> g.sex)), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_ni : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.alternatives_ni, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_r : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.alternatives_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // kwd directive
    struct keyword_d : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, repo::kwd("name")[qi::lit('=') >> qi::lexeme[+qi::alpha]] / repo::kwd("age")[qi::lit('=') >> qi::int_] / repo::kwd("sex")[qi::lit('=') >> qi::char_("mf")], qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_ni : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.kwd_ni, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_rr : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, repo::kwd("name")[g.name] / repo::kwd("age")[g.age] / repo::kwd("sex")[g.sex], qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_r : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.kwd_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct nabialek_d : test::base
    {
        void benchmark()
        {
            char* first = &source[0];

            // alternative and keyword don't have a choice but to build their "symbol table"
            // within the benchmark, so it's fair to let the nabialek operator create it's
            // symbol table here (although users usually would create that once and use it
            // often)
            qi::symbols<char, int> sym;
            sym.add("name", 1)("age", 2)("sex", 3);

            bool success = qi::phrase_parse(first, last,
                +nabialek(sym,
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
    struct nabialek_dd : test::base
    {
        void benchmark()
        {
            char* first = &source[0];

            // Opposite to than nabialek_d we use a predefined symbol table here
            bool success = qi::phrase_parse(first, last,
                +nabialek(g.sym3,
                    qi::lit('=') >> qi::lexeme[+qi::alpha],
                    qi::lit('=') >> qi::int_,
                    qi::lit('=') >> qi::char_("mf"))
                , qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_rr : test::base
    {
        void benchmark()
        {
            char* first = &source[0];

            // alternative and keyword don't have a choice but to build their "symbol table"
            // within the benchmark, so it's fair to let the nabialek operator create it's
            // symbol table here (although users usually would create that once and use it
            // often)

            qi::symbols<char, int> sym;
            sym.add("name", 1)("age", 2)("sex", 3);
            bool success = qi::phrase_parse(first, last, +nabialek(sym, g.name, g.age, g.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_rrr : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            // Opposite to than nabialek_rr we use a predefined symbol table here
            bool success = qi::phrase_parse(first, last, +nabialek(g.sym3, g.name, g.age, g.sex), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_ni : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.nabialek_ni, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };
    struct nabialek_r : test::base
    {
        void benchmark()
        {
            char* first = &source[0];
            bool success = qi::phrase_parse(first, last, g.nabialek_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };
}


int main(int argc, char **argv)
{
//    srand(int(time(0)));
    char* last = &source[strlen(source)];

    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//   nabialek_trick_org: nabialek trick with rules" << std::endl;
        std::cout << "//   nabialek_trick_ptr: nabialek trick with pointers to rules" << std::endl;
        std::cout << "// " << std::endl;
        std::cout << "// In both cases the nabialek trick is defined as a rule contained by " << std::endl;
        std::cout << "// a grammar." << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK,     
            (nabialek_trick_org)(nabialek_trick_ptr)
        )
    }
    std::cout << std::endl;
    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//    qi alternative      : alternative " << std::endl;
        std::cout << "//    qi::repo::kwd       : keyword " << std::endl;
        std::cout << "//    nabialek operator   : nabialek " << std::endl;
        std::cout << "//" << std::endl;
        std::cout << "// In this sequence the tested expression is written explicitly to" << std::endl;
        std::cout << "// the phrase_parse statement." << std::endl;
        std::cout << "// Because kwd and alternative must create their symbol table upon execution," << std::endl;
        std::cout << "// the nabialek operator direct test also creates the symbol table while " << std::endl;
        std::cout << "// executing to be comparable." << std::endl;
        std::cout << "// An additional direct test (_dd) shows the nabialek operator with" << std::endl;     
        std::cout << "// predefined symbol table (which is the more common use case)." << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_d) (keyword_d) (nabialek_d) (nabialek_dd)
        )
    }
    std::cout << std::endl;
    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//    qi alternative      : alternative " << std::endl;
        std::cout << "//    qi::repo::kwd       : keyword " << std::endl;
        std::cout << "//    nabialek operator   : nabialek " << std::endl;
        std::cout << "//" << std::endl;
        std::cout << "// In this sequence the tested expression is taken from the container." << std::endl;
        std::cout << "// The rule explicitly defines the whole expression." << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_r) (keyword_r) (nabialek_r)      
        )
    }
    std::cout << std::endl;
    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//    qi alternative      : alternative " << std::endl;
        std::cout << "//    qi::repo::kwd       : keyword " << std::endl;
        std::cout << "//    nabialek operator   : nabialek " << std::endl;
        std::cout << "//" << std::endl;
        std::cout << "// In this sequence the tested expression is written to" << std::endl;
        std::cout << "// the phrase_parse statement, but the conditional parsers are taken" << std::endl;
        std::cout << "// from the container." << std::endl;
        std::cout << "// Because kwd and alternative must create their symbol table upon execution," << std::endl;
        std::cout << "// the nabialek operator direct test also creates the symbol table while " << std::endl;
        std::cout << "// executing to be comparable." << std::endl;
        std::cout << "// An additional direct test (_rrr) shows the nabialek operator with" << std::endl;
        std::cout << "// predefined symbol table (which is the more common use case)." << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_rr) (keyword_rr) (nabialek_rr)(nabialek_rrr)
        )
    }
    std::cout << std::endl;
    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//    qi alternative      : alternative " << std::endl;
        std::cout << "//    qi::repo::kwd       : keyword " << std::endl;
        std::cout << "//    nabialek operator   : nabialek " << std::endl;
        std::cout << "//" << std::endl;
        std::cout << "// In this sequence the tested expression is taken from rule container." << std::endl;
        std::cout << "// The rule references other rules of the container for the conditional" << std::endl;
        std::cout << "// parsers." << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_ni) (keyword_ni) (nabialek_ni)   
        )
    }
    // This is ultimately responsible for preventing all the test code
        // from being optimized away.  Change this to return 0 and you
        // unplug the whole test's life support system.
        return test::live_code != 0;
}

