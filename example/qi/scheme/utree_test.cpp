#include "utree.hpp"
#include <iostream>

namespace
{
    void print(scheme::utree const& val, bool no_endl = false);
    void print(char ch, bool ignored)
    {
        std::cout << ch;
    }

    struct simple_print
    {
        typedef void result_type;

        void operator()(scheme::utree::nil) const
        {
            std::cout << "nil";
        }

        template <typename T>
        void operator()(T val) const
        {
            std::cout << val;
        }

        void operator()(bool b) const
        {
            std::cout << (b?"true":"false");
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            // This code works for both strings and lists
            typedef typename boost::iterator_range<Iterator>::const_iterator iterator;
            bool const is_string = boost::is_pointer<Iterator>::value;
            char const start = is_string ? '"' : '(';
            char const end = is_string ? '"' : ')';

            std::cout << start;
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                if (!is_string)
                {
                    if (i != range.begin())
                        std::cout << ", ";
                }
                print(*i, true);
            }
            std::cout << end;
        }
    };

    inline void print(scheme::utree const& val, bool no_endl)
    {
        scheme::utree::visit(val, simple_print());
        if (!no_endl)
            std::cout << std::endl;
    }
}

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
        print(val);
    }

    {
        utree val(true);
        print(val);
    }

    {
        utree val(123);
        print(val);
    }

    {
        utree val(123.456);
        print(val);
    }

    {
        utree val("Hello, World");
        print(val);
        utree val2;
        val2 = val;
        print(val2);
        utree val3("Hello, World. Chuckie is back!!!");
        val = val3;
        print(val);

        utree val4("Apple");
        utree val5("Apple");
        BOOST_ASSERT(val4 == val5);

        utree val6("ApplePie");
        BOOST_ASSERT(val4 < val6);
    }

    {
        utree val;
        val.push_back(123);
        val.push_back("Chuckie");
        utree val2;
        val2.push_back(123.456);
        val2.push_back("Mah Doggie");
        val.push_back(val2);
        print(val);
        print(val.front());

        utree val3;
        val3.swap(val);
        print(val);
        val3.swap(val);
        print(val);
        val.push_back("Ba Ba Black Sheep");
        print(val);
        val.pop_front();
        print(val);
        utree::iterator i = val.begin();
        ++++i;
        val.insert(i, "Right in the middle");
        BOOST_ASSERT(val.size() == 4);
        print(val);
        val.pop_back();
        print(val);
        BOOST_ASSERT(val.size() == 3);
        val.erase(val.end());
        print(val);
        BOOST_ASSERT(val.size() == 2);

        val.insert(val.begin(), val2.begin(), val2.end());
        print(val);
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
