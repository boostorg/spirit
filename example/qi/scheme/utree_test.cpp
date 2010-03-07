#include "utree.hpp"
#include <iostream>

int main()
{
    using scheme::utree;
    using scheme::ulist;

    {
        // test the size
        std::cout << sizeof(scheme::utree) << std::endl;
    }

    {
        utree val;
        std::cout << val << std::endl;
    }

    {
        utree val(true);
        std::cout << val << std::endl;
    }

    {
        utree val(123);
        std::cout << val << std::endl;
    }

    {
        utree val(123.456);
        std::cout << val << std::endl;
    }

    {
        utree val("Hello, World");
        std::cout << val << std::endl;
        utree val2;
        val2 = val;
        std::cout << val2 << std::endl;
        utree val3("Hello, World. Chuckie is back!!!");
        val = val3;
        std::cout << val << std::endl;
    }

    {
        utree val;
        val.push_back(123);
        val.push_back("Chuckie");
        utree val2;
        val2.push_back(123.456);
        val2.push_back("Mah Doggie");
        val.push_back(val2);
        std::cout << val << std::endl;
        std::cout << val.front() << std::endl;

        utree val3;
        val3.swap(val);
        std::cout << val << std::endl;
        val3.swap(val);
        std::cout << val << std::endl;
        val.push_back("Ba Ba Black Sheep");
        std::cout << val << std::endl;
        val.pop_front();
        std::cout << val << std::endl;
        utree::iterator i = val.begin();
        ++++i;
        val.insert(i, "Right in the middle");
        BOOST_ASSERT(val.size() == 4);
        std::cout << val << std::endl;
        val.pop_back();
        std::cout << val << std::endl;
        BOOST_ASSERT(val.size() == 3);
        val.erase(val.end());
        std::cout << val << std::endl;
        BOOST_ASSERT(val.size() == 2);

        val.insert(val.begin(), val2.begin(), val2.end());
        std::cout << val << std::endl;
    }

    {
        utree a, b;
        BOOST_ASSERT(a == b);
        a = 123;
        BOOST_ASSERT(a != b);
        b = 123;
        BOOST_ASSERT(a == b);
        a = 100.00;
        BOOST_ASSERT(a < b);

        b = a = ulist();
        BOOST_ASSERT(a == b);
        a.push_back(1);
        a.push_back("two");
        a.push_back(3.0);
        b.push_back(1);
        b.push_back("two");
        b.push_back(3.0);
        BOOST_ASSERT(a == b);
        b.push_back(4);
        BOOST_ASSERT(a != b);
        BOOST_ASSERT(a < b);
    }

    {
        ulist a;
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

        BOOST_ASSERT(a[0] == utree(1));
        BOOST_ASSERT(a[1] == utree(2));
        BOOST_ASSERT(a[2] == utree(3));
        BOOST_ASSERT(a[3] == utree(4));
        BOOST_ASSERT(a[4] == utree(5));
        BOOST_ASSERT(a[5] == utree(6));
        BOOST_ASSERT(a[6] == utree(7));
        BOOST_ASSERT(a[7] == utree(8));
        BOOST_ASSERT(a[8] == utree(9));
        BOOST_ASSERT(a[9] == utree(10));
        BOOST_ASSERT(a[10] == utree(11));
        BOOST_ASSERT(a[11] == utree(12));
    }

    return 0;
}
