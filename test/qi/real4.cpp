/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2001-2010 Hartmut Kaiser

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "real.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    
    ///////////////////////////////////////////////////////////////////////////
    //  Custom data type
    ///////////////////////////////////////////////////////////////////////////
    {
        using boost::math::concepts::real_concept;
        using boost::spirit::qi::real_parser;
        using boost::spirit::qi::real_policies;
        using boost::spirit::qi::parse;

        real_parser<real_concept, real_policies<real_concept> > custom_real;
        real_concept d;

        BOOST_TEST(test("-1234", custom_real));
        BOOST_TEST(test_attr("-1234", custom_real, d) && compare(d, -1234));

        BOOST_TEST(test("-1.2e3", custom_real));
        BOOST_TEST(test_attr("-1.2e3", custom_real, d) && compare(d, -1.2e3));

        BOOST_TEST(test("+1.2e3", custom_real));
        BOOST_TEST(test_attr("+1.2e3", custom_real, d) && compare(d, 1.2e3));

        BOOST_TEST(test("-0.1", custom_real));
        BOOST_TEST(test_attr("-0.1", custom_real, d) && compare(d, -0.1));

        BOOST_TEST(test("-1.2e-3", custom_real));
        BOOST_TEST(test_attr("-1.2e-3", custom_real, d) && compare(d, -1.2e-3));

        BOOST_TEST(test("-1.e2", custom_real));
        BOOST_TEST(test_attr("-1.e2", custom_real, d) && compare(d, -1.e2));

        BOOST_TEST(test("-.2e3", custom_real));
        BOOST_TEST(test_attr("-.2e3", custom_real, d) && compare(d, -.2e3));

        BOOST_TEST(test("-2e3", custom_real));
        BOOST_TEST(test_attr("-2e3", custom_real, d) && compare(d, -2e3));

        BOOST_TEST(!test("-e3", custom_real));
        BOOST_TEST(!test_attr("-e3", custom_real, d));

        BOOST_TEST(!test("-1.2e", custom_real));
        BOOST_TEST(!test_attr("-1.2e", custom_real, d));
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  custom real tests
    ///////////////////////////////////////////////////////////////////////////
    //~ {
        //~ using boost::spirit::qi::double_;
        //~ custom_real n;

        //~ BOOST_TEST(test_attr("-123456e6", double_, n));
    //~ }

    // this should pass, but currently doesn't because of the way the real 
    // parser handles the fractional part of a number
    //{
        //using boost::spirit::qi::float_;

        //float f;
        //BOOST_TEST(test_attr("123233.4124", float_, f)  && f == 123233.4140625f);
    //}

    return boost::report_errors();
}
