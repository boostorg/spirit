//  Copyright (c) 2014 Tomoki Imai
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/detail/lightweight_test.hpp>
#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

int main()
{
    typedef boost::spirit::line_pos_iterator<std::string::const_iterator>
                pos_iterator_t;
    typedef boost::iterator_range<pos_iterator_t>
                iterator_range;
    {
        std::string test = "012\n345\n678";
        pos_iterator_t begin(test.begin()),
                       end(test.end());

        for (pos_iterator_t iter = begin; iter != end; ++iter) {
            // ignore newline because newline's line or column is ambiguous.
            if (*iter == '\n') continue;
            int n = *iter - '0';
            int line = n / 3 + 1;
            int column = n % 3 + 1;

            BOOST_TEST(line == get_line(iter) &&
                       column == get_column(begin,iter));

            iterator_range range = get_current_line(begin,iter,end);
            std::string current_line(range.begin(),range.end());
            if (line == 1) {
                BOOST_TEST(current_line == "012");
            } else if(line == 2) {
                BOOST_TEST(current_line == "345");
            } else {
                BOOST_TEST(current_line == "678");
            }
        }
    }
    return 0;
}
