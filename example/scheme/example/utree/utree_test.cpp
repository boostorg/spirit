/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/config/warning_disable.hpp>

#include <utree/utree.hpp>
#include <utree/operators.hpp>
#include <utree/io.hpp>
#include <iostream>
#include <sstream>

inline void check(scheme::utree const& val, std::string expected)
{
    std::stringstream s;
    s << val;
    BOOST_TEST(s.str() == expected + " ");
}

int main()
{
    using scheme::utree;

    {
        // test the size
        std::cout << "size of utree is: "
            << sizeof(scheme::utree) << " bytes" << std::endl;
    }

    {
        utree val;
        check(val, "<nil>");
    }

    {
        utree val(true);
        check(val, "true");
    }

    {
        utree val(123);
        check(val, "123");
    }

    {
        utree val(123.456);
        check(val, "123.456");
    }

    {
        utree val("Hello, World");
        check(val, "\"Hello, World\"");
        utree val2;
        val2 = val;
        check(val2, "\"Hello, World\"");
        utree val3("Hello, World. Chuckie is back!!!");
        val = val3;
        check(val, "\"Hello, World. Chuckie is back!!!\"");

        utree val4("Apple");
        utree val5("Apple");
        BOOST_TEST(val4 == val5);

        utree val6("ApplePie");
        BOOST_TEST(val4 < val6);
    }

    {
        utree val;
        val.push_back(123);
        val.push_back("Chuckie");
        BOOST_TEST(val.size() == 2);
        utree val2;
        val2.push_back(123.456);
        val2.push_back("Mah Doggie");
        val.push_back(val2);
        BOOST_TEST(val.size() == 3);
        check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )");
        check(val.front(), "123");

        utree val3;
        val3.swap(val);
        BOOST_TEST(val3.size() == 3);
        check(val, "<nil>");
        val3.swap(val);
        check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )");
        val.push_back("another string");
        BOOST_TEST(val.size() == 4);
        check(val, "( 123 \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"another string\" )");
        val.pop_front();
        check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"another string\" )");
        utree::iterator i = val.begin();
        ++++i;
        val.insert(i, "Right in the middle");
        BOOST_TEST(val.size() == 4);
        check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"Right in the middle\" \"another string\" )");
        val.pop_back();
        check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) \"Right in the middle\" )");
        BOOST_TEST(val.size() == 3);
        utree::iterator it = val.end(); --it;
        val.erase(it);
        check(val, "( \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )");
        BOOST_TEST(val.size() == 2);

        val.insert(val.begin(), val2.begin(), val2.end());
        check(val, "( 123.456 \"Mah Doggie\" \"Chuckie\" ( 123.456 \"Mah Doggie\" ) )");
        BOOST_TEST(val.size() == 4);
    }

    {
        utree val;
        val.insert(val.end(), 123);
        val.insert(val.end(), "Mia");
        val.insert(val.end(), "Chuckie");
        val.insert(val.end(), "Poly");
        val.insert(val.end(), "Mochi");
        check(val, "( 123 \"Mia\" \"Chuckie\" \"Poly\" \"Mochi\" )");
    }

    {
        utree a, b;
        BOOST_TEST(a == b);
        a = 123;
        BOOST_TEST(a != b);
        b = 123;
        BOOST_TEST(a == b);
        a = 100.00;
        BOOST_TEST(a < b);

        b = a = utree();
        BOOST_TEST(a == b);
        a.push_back(1);
        a.push_back("two");
        a.push_back(3.0);
        b.push_back(1);
        b.push_back("two");
        b.push_back(3.0);
        BOOST_TEST(a == b);
        b.push_back(4);
        BOOST_TEST(a != b);
        BOOST_TEST(a < b);
    }

    {
        utree a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_back(4);
        a.push_back(5);
        a.push_back(6);
        a.push_back(7);
        a.push_back(8);
        a.push_back(9);
        a.push_back(10);
        a.push_back(11);
        a.push_back(12);

        BOOST_TEST(a[0] == utree(1));
        BOOST_TEST(a[1] == utree(2));
        BOOST_TEST(a[2] == utree(3));
        BOOST_TEST(a[3] == utree(4));
        BOOST_TEST(a[4] == utree(5));
        BOOST_TEST(a[5] == utree(6));
        BOOST_TEST(a[6] == utree(7));
        BOOST_TEST(a[7] == utree(8));
        BOOST_TEST(a[8] == utree(9));
        BOOST_TEST(a[9] == utree(10));
        BOOST_TEST(a[10] == utree(11));
        BOOST_TEST(a[11] == utree(12));
    }

    { // test references
        utree val(123);
        utree ref(boost::ref(val));
        check(ref, "123");
        BOOST_TEST(ref == utree(123));

        val.clear();
        val.push_back(1);
        val.push_back(2);
        val.push_back(3);
        val.push_back(4);
        check(ref, "( 1 2 3 4 )");
        BOOST_TEST(ref[0] == utree(1));
        BOOST_TEST(ref[1] == utree(2));
        BOOST_TEST(ref[2] == utree(3));
        BOOST_TEST(ref[3] == utree(4));
    }

    { // put it in an array

        utree vals[] = {
            utree(123),
            utree("Hello, World"),
            utree(123.456)
        };

        check(vals[0], "123");
        check(vals[1], "\"Hello, World\"");
        check(vals[2], "123.456");
    }

    { // operators

        BOOST_TEST((utree(true) && utree(true)) == utree(true));
        BOOST_TEST((utree(true) || utree(false)) == utree(true));
        BOOST_TEST(!utree(true) == utree(false));

        BOOST_TEST((utree(456) + utree(123)) == utree(456 + 123));
        BOOST_TEST((utree(456) + utree(123.456)) == utree(456 + 123.456));
        BOOST_TEST((utree(456) - utree(123)) == utree(456 - 123));
        BOOST_TEST((utree(456) - utree(123.456)) == utree(456 - 123.456));
        BOOST_TEST((utree(456) * utree(123)) == utree(456 * 123));
        BOOST_TEST((utree(456) * utree(123.456)) == utree(456 * 123.456));
        BOOST_TEST((utree(456) / utree(123)) == utree(456 / 123));
        BOOST_TEST((utree(456) / utree(123.456)) == utree(456 / 123.456));
        BOOST_TEST((utree(456) % utree(123)) == utree(456 % 123));
        BOOST_TEST(-utree(456) == utree(-456));

        BOOST_TEST((utree(456) & utree(123)) == utree(456 & 123));
        BOOST_TEST((utree(456) | utree(123)) == utree(456 | 123));
        BOOST_TEST((utree(456) ^ utree(123)) == utree(456 ^ 123));
        BOOST_TEST((utree(456) << utree(3)) == utree(456 << 3));
        BOOST_TEST((utree(456) >> utree(2)) == utree(456 >> 2));
        BOOST_TEST(~utree(456) == utree(~456));
    }

    return boost::report_errors();
}
