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


// Change this setting for a different number of keywords.
// Please look at your compiler spec because there are limits
// regarding nesting (mscv's limit is 127, c++ standard 
// recommends 256)
#define N_NR_KEYWORDS 8
#include "nabialek.hpp"



// enable this define to activate asserts which check if the parsers 
// succeeds and parses the whole input (for debugging)

//#define CHECK_RESULT

//change this define to set the number max number of runs of the
//benchmark

#define NABIALEK_MAX_BENCHMARK 10000000


namespace
{

    namespace qi = boost::spirit::qi;
    namespace repo = boost::spirit::repository::qi;
    using mxc::qitoo::nabialek;

    ///////////////////////////////////////////////////////////////////////////
    // nabialek trick
    struct nabialek_trick_org : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.ntrick_org, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_trick_ptr : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.ntrick_ptr, qi::space);
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
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, N_CREATE(ALTERNATIVE, EXPR), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_rr : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, N_CREATE(ALTERNATIVE,QRULE), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_ni : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.alternative_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct alternative_r : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.alternative_d, qi::space);
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
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, N_CREATE(KEYWORD, EXPR), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_ni : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.kwd_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_rr : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, N_CREATE(KEYWORD,QRULE) , qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct keyword_r : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.kwd_r, qi::space);
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
            char* first = &g.source[0];

            bool success = qi::phrase_parse(first, g.last, N_CREATE(NABIALEK,EXPR2), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };


    struct nabialek_rr : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];

            bool success = qi::phrase_parse(first, g.last, N_CREATE(NABIALEK,QRULE), qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };

    struct nabialek_ni : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.nabialek_r, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };
    struct nabialek_r : test::base
    {
        void benchmark()
        {
            char* first = &g.source[0];
            bool success = qi::phrase_parse(first, g.last, g.nabialek_d, qi::space);
#ifdef CHECK_RESULT
            BOOST_ASSERT(success && (first == last));
#endif
        }
    };
}


int main(int argc, char **argv)
{

    {
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "// Benchmark" << std::endl;
        std::cout << "//   nabialek_trick_org: nabialek trick with rules" << std::endl;
        std::cout << "//   nabialek_trick_ptr: nabialek trick with pointers to rules" << std::endl;
        std::cout << "// " << std::endl;
        std::cout << "// In both cases the nabialek trick is defined as a rule contained by " << std::endl;
        std::cout << "// a container struct." << std::endl;
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
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_d) (keyword_d) (nabialek_d)
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
        std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << std::endl;
        BOOST_SPIRIT_TEST_BENCHMARK(
            NABIALEK_MAX_BENCHMARK, (alternative_rr) (keyword_rr) (nabialek_rr)
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

