/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/algorithm/push_back.hpp>
#include <boost/spirit/fusion/algorithm/for_each.hpp>
#include <boost/mpl/vector_c.hpp>
#include <string>

struct plus_one
{
    template <typename T>
    void operator()(T& v) const
    {
        v += 1;
    }
};

int
main()
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing push_back

    {
        char const* s = "Ruby";
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t1(1, 'x', 3.3, s);

        {
            std::cout << push_back(t1, 123456) << std::endl;
            BOOST_TEST((push_back(t1, 123456)
                == make_tuple(1, 'x', 3.3, s, 123456)));
        }

        {
            std::cout << push_back(t1, "funny") << std::endl;
            BOOST_TEST((push_back(t1, "funny")
                == make_tuple(1, 'x', 3.3, s, std::string("funny"))));
        }

        {
            std::cout << push_back(t1, t1) << std::endl;
            BOOST_TEST((push_back(t1, t1)
                == make_tuple(1, 'x', 3.3, s, t1)));
        }
    }

    {
        // This is OK -- can modify a tuple in place
        tuple<int,int,int> i(1,2,3);
        std::cout << i << '\n';
        for_each(i, plus_one());
        std::cout << i << '\n';
        
        // This should also be OK -- can modify a fusion sequence in place
        typedef tuple<> T1;
        typedef meta::push_back<T1, int>::type T2;
        typedef meta::push_back<T2, int>::type T3;
        typedef meta::push_back<T3, int>::type T4;
        
        T1 t1 = T1();
        T2 t2 = push_back(t1, 1);
        T3 t3 = push_back(t2, 2);
        T4 t4 = push_back(t3, 3);

        std::cout << "=============\n";
        std::cout << t4 << std::endl;
        for_each(t4, plus_one());
        std::cout << "=============\n";
        std::cout << t4 << std::endl;
        
        BOOST_TEST(t4 == make_tuple(2, 3, 4));
    }
    
    {
        typedef boost::mpl::vector_c<int, 1, 2, 3, 4, 5> mpl_vec;
        std::cout << boost::fusion::push_back(mpl_vec(), boost::mpl::int_<6>()) << std::endl;
        BOOST_TEST((boost::fusion::push_back(mpl_vec(), boost::mpl::int_<6>())
            == make_tuple(1, 2, 3, 4, 5, 6)));
    }

    return boost::report_errors();
}

