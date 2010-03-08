#if !defined(UTREE_SIMPLE_PRINT)
#define UTREE_SIMPLE_PRINT

#include "utree.hpp"
#include <iostream>

namespace detail
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

        template <typename Range> // for lists
        void print_string_or_list(Range range, boost::mpl::false_) const
        {
            typedef typename Range::const_iterator iterator;
            print('(');
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                if (i != range.begin())
                    detail::print(' ');
                detail::print(*i);
            }
            detail::print(')');
        }

        template <typename Range> // for strings
        void print_string_or_list(Range range, boost::mpl::true_) const
        {
            typedef typename Range::const_iterator iterator;
            iterator i = range.begin();
            bool const is_symbol = *i == '\0';  // a 0 byte at the beginning signifies a symbol
            if (!is_symbol)
                detail::print('"');
            else
                ++i;
            for (; i != range.end(); ++i)
                detail::print(*i);
            if (!is_symbol)
                detail::print('"');
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            // This code works for both strings and lists
            print_string_or_list(range, boost::is_pointer<Iterator>());
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
        detail::print(val);
        std::cout << std::endl;
    }
}

#endif