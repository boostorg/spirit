#if !defined(UTREE_SIMPLE_PRINT)
#define UTREE_SIMPLE_PRINT

#include "utree.hpp"
#include <iostream>

namespace
{
    ///////////////////////////////////////////////////////////////////////////
    // simple utree printing facility prints the utree in a single line
    ///////////////////////////////////////////////////////////////////////////
    void print(char ch);
    void print(scheme::utree const& val);
    void println(scheme::utree const& val);

    // simple_print visitor
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
            std::cout << (b ? "true" : "false");
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            // This code works for both strings and lists
            typedef typename boost::iterator_range<Iterator>::const_iterator iterator;
            bool const is_string = boost::is_pointer<Iterator>::value;
            char const start = is_string ? '"' : '(';
            char const end = is_string ? '"' : ')';

            print(start);
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                if (!is_string)
                {
                    if (i != range.begin())
                        print(' ');
                }
                print(*i);
            }
            print(end);
        }
    };

    inline void print(char ch)
    {
        std::cout << ch;
    }

    inline void print(scheme::utree const& val)
    {
        scheme::utree::visit(val, simple_print());
    }

    inline void println(scheme::utree const& val)
    {
        print(val);
        std::cout << std::endl;
    }
}

#endif