#include "uint.hpp"

#include <iostream>

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    
    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 3)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 3, 1, -1>             base3_parser;

        BOOST_TEST(test("210112221200",                 base3_parser));
        BOOST_TEST(test_attr("210112221200",            base3_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("1231",                        base3_parser));
        BOOST_TEST(!test_attr("1231",                   base3_parser, u));
        
        char const* max_unsigned_base3 =                "102002022201221111210";
        char const* unsigned_overflow_base3 =           "102002022201221111211";
        char const* digit_overflow_base3 =              "1020020222012211112100";

        BOOST_TEST(test(max_unsigned_base3,             base3_parser));
        BOOST_TEST(test_attr(max_unsigned_base3,        base3_parser, u));

        BOOST_TEST(!test(unsigned_overflow_base3,       base3_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base3,  base3_parser, u));
        BOOST_TEST(!test(digit_overflow_base3,          base3_parser));
        BOOST_TEST(!test_attr(digit_overflow_base3,     base3_parser, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 4)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 4, 1, -1>             base4_parser;

        BOOST_TEST(test("1213210302",                   base4_parser));
        BOOST_TEST(test_attr("1213210302",              base4_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("1234",                        base4_parser));
        BOOST_TEST(!test_attr("1234",                   base4_parser, u));
        
        char const* max_unsigned_base4 =                "3333333333333333";
        char const* unsigned_overflow_base4 =           "33333333333333330";
        BOOST_TEST(test(max_unsigned_base4,             base4_parser));
        BOOST_TEST(test_attr(max_unsigned_base4,        base4_parser, u));
        BOOST_TEST(!test(unsigned_overflow_base4,       base4_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base4,  base4_parser, u));
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 5)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 5, 1, -1>             base5_parser;

        BOOST_TEST(test("102033432",                    base5_parser));
        BOOST_TEST(test_attr("102033432",               base5_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("2345",                        base5_parser));
        BOOST_TEST(!test_attr("2345",                   base5_parser, u));
        
        char const* max_unsigned_base5 =                "32244002423140";
        char const* unsigned_overflow_base5 =           "32244002423141";
        char const* digit_overflow_base5 =              "322440024231400";

        BOOST_TEST(test(max_unsigned_base5,             base5_parser));
        BOOST_TEST(test_attr(max_unsigned_base5,        base5_parser, u));

        BOOST_TEST(!test(unsigned_overflow_base5,       base5_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base5,  base5_parser, u));
        BOOST_TEST(!test(digit_overflow_base5,          base5_parser));
        BOOST_TEST(!test_attr(digit_overflow_base5,     base5_parser, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 6)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 6, 1, -1>             base6_parser;

        BOOST_TEST(test("13032030",                     base6_parser));
        BOOST_TEST(test_attr("13032030",                base6_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("3456",                        base6_parser));
        BOOST_TEST(!test_attr("3456",                   base6_parser, u));
        
        char const* max_unsigned_base6 =                "1550104015503";
        char const* unsigned_overflow_base6 =           "1550104015504";
        char const* digit_overflow_base6 =              "15501040155030";

        BOOST_TEST(test(max_unsigned_base6,             base6_parser));
        BOOST_TEST(test_attr(max_unsigned_base6,        base6_parser, u));

        BOOST_TEST(!test(unsigned_overflow_base6,       base6_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base6,  base6_parser, u));
        BOOST_TEST(!test(digit_overflow_base6,          base6_parser));
        BOOST_TEST(!test_attr(digit_overflow_base6,     base6_parser, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 7)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 7, 1, -1>             base7_parser;

        BOOST_TEST(test("3414600",                      base7_parser));
        BOOST_TEST(test_attr("3414600",                 base7_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("4567",                        base7_parser));
        BOOST_TEST(!test_attr("4567",                   base7_parser, u));
        
        char const* max_unsigned_base7 =                "211301422353";
        char const* unsigned_overflow_base7 =           "211301422354";
        char const* digit_overflow_base7 =              "2113014223530";

        BOOST_TEST(test(max_unsigned_base7,             base7_parser));
        BOOST_TEST(test_attr(max_unsigned_base7,        base7_parser, u));

        BOOST_TEST(!test(unsigned_overflow_base7,       base7_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base7,  base7_parser, u));
        BOOST_TEST(!test(digit_overflow_base7,          base7_parser));
        BOOST_TEST(!test_attr(digit_overflow_base7,     base7_parser, u));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  arbitrary radix test (base 7)
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        unsigned int u;

        uint_parser<unsigned int, 9, 1, -1>             base9_parser;

        BOOST_TEST(test("715850",                       base9_parser));
        BOOST_TEST(test_attr("715850",                  base9_parser, u));
        BOOST_TEST(424242 == u);

        BOOST_TEST(!test("6789",                        base9_parser));
        BOOST_TEST(!test_attr("6789",                   base9_parser, u));
        
        char const* max_unsigned_base9 =                "12068657453";
        char const* unsigned_overflow_base9 =           "12068657454";
        char const* digit_overflow_base9 =              "120686574530";

        BOOST_TEST(test(max_unsigned_base9,             base9_parser));
        BOOST_TEST(test_attr(max_unsigned_base9,        base9_parser, u));

        BOOST_TEST(!test(unsigned_overflow_base9,       base9_parser));
        BOOST_TEST(!test_attr(unsigned_overflow_base9,  base9_parser, u));
        BOOST_TEST(!test(digit_overflow_base9,          base9_parser));
        BOOST_TEST(!test_attr(digit_overflow_base9,     base9_parser, u));
    }

    return boost::report_errors();
}
