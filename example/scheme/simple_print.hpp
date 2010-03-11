#if !defined(UTREE_SIMPLE_PRINT)
#define UTREE_SIMPLE_PRINT

#include "utree.hpp"
#include <iostream>

namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    // simple utree printing facility prints the utree in a single line
    ///////////////////////////////////////////////////////////////////////////

    std::ostream& print(std::ostream& out, scheme::utree const& val);
    std::ostream& println(std::ostream& out, scheme::utree const& val);

    // simple_print visitor
    struct simple_print
    {
        typedef void result_type;

        std::ostream& out;
        simple_print(std::ostream& out) : out(out) {}

        void operator()(scheme::utree::nil) const
        {
            out << "nil";
        }

        template <typename T>
        void operator()(T val) const
        {
            out << val;
        }

        void operator()(bool b) const
        {
            out << (b ? "true" : "false");
        }

        template <typename Range> // for lists
        void print_string_or_list(Range range, boost::mpl::false_) const
        {
            typedef typename Range::const_iterator iterator;
            (*this)('(');
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                if (i != range.begin())
                    (*this)(' ');
                scheme::utree::visit(*i, *this);
            }
            (*this)(')');
        }

        template <typename Range> // for strings
        void print_string_or_list(Range range, boost::mpl::true_) const
        {
            typedef typename Range::const_iterator iterator;
            iterator i = range.begin();
            if (*i == '\1') // a 1 byte at the beginning signifies a byte stream
            {
                out << "b"; ++i;
                for (; i != range.end(); ++i)
                    out << std::setw(2) << std::setfill('0')
                        << std::hex << int((unsigned char)*i);
                out << std::dec;
            }
            else
            {
                bool const is_symbol = *i == '\0';  // a 0 byte at the beginning signifies a symbol
                if (!is_symbol)
                    out << '"';
                else
                    ++i;
                for (; i != range.end(); ++i)
                    out << *i;
                if (!is_symbol)
                    out << '"';
            }
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            // This code works for both strings and lists
            print_string_or_list(range, boost::is_pointer<Iterator>());
        }
    };

    inline std::ostream& print(std::ostream& out, scheme::utree const& val)
    {
        scheme::utree::visit(val, simple_print(out));
        return out;
    }

    inline std::ostream& println(std::ostream& out, scheme::utree const& val)
    {
        detail::print(out, val) << std::endl;
        return out;
    }
}

#endif