/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE_OPERATORS)
#define BOOST_SPIRIT_UTREE_OPERATORS

#include "utree.hpp"
#include <boost/preprocessor/cat.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace scheme
{
    // Available operators
    bool operator==(utree const& a, utree const& b);
    bool operator<(utree const& a, utree const& b);
    bool operator!=(utree const& a, utree const& b);
    bool operator>(utree const& a, utree const& b);
    bool operator<=(utree const& a, utree const& b);
    bool operator>=(utree const& a, utree const& b);
    std::ostream& operator<<(std::ostream& out, utree const& x);

    utree operator+(utree const& a, utree const& b);
    utree operator-(utree const& a, utree const& b);
    utree operator*(utree const& a, utree const& b);
    utree operator/(utree const& a, utree const& b);
    utree operator%(utree const& a, utree const& b);

    // Implementation
    struct utree_is_equal
    {
        typedef bool result_type;

        template <typename A, typename B>
        bool dispatch(const A&, const B&, boost::mpl::false_) const
        {
            return false; // cannot compare different types by default
        }

        template <typename A, typename B>
        bool dispatch(const A& a, const B& b, boost::mpl::true_) const
        {
            return a == b; // for arithmetic types
        }

        template <typename A, typename B>
        bool operator()(const A& a, const B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_arithmetic<A>,
                    boost::is_arithmetic<B> >());
        }

        template <typename T>
        bool operator()(const T& a, const T& b) const
        {
            // This code works for lists
            return a == b;
        }

        template <typename Base, utree_type::info type_>
        bool operator()(
            basic_string<Base, type_> const& a,
            basic_string<Base, type_> const& b) const
        {
            return static_cast<Base const&>(a) == static_cast<Base const&>(b);
        }

        bool operator()(nil, nil) const
        {
            return true;
        }
    };

    struct utree_is_less_than
    {
        typedef bool result_type;

        template <typename A, typename B>
        bool dispatch(const A&, const B&, boost::mpl::false_) const
        {
            return false; // cannot compare different types by default
        }

        template <typename A, typename B>
        bool dispatch(const A& a, const B& b, boost::mpl::true_) const
        {
            return a < b; // for arithmetic types
        }

        template <typename A, typename B>
        bool operator()(const A& a, const B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_arithmetic<A>,
                    boost::is_arithmetic<B> >());
        }

        template <typename T>
        bool operator()(const T& a, const T& b) const
        {
            // This code works for lists
            return a < b;
        }

        template <typename Base, utree_type::info type_>
        bool operator()(
            basic_string<Base, type_> const& a,
            basic_string<Base, type_> const& b) const
        {
            return static_cast<Base const&>(a) < static_cast<Base const&>(b);
        }

        bool operator()(nil, nil) const
        {
            BOOST_ASSERT(false);
            return false; // no less than comparison for nil
        }
    };

    struct utree_print
    {
        typedef void result_type;

        std::ostream& out;
        utree_print(std::ostream& out) : out(out) {}

        void operator()(scheme::nil) const
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

        void operator()(binary_range const& b) const
        {
            out << "b";
            out.width(2);
            out.fill('0');

            typedef binary_range::const_iterator iterator;
            for (iterator i = b.begin(); i != b.end(); ++i)
                out << std::hex << int((unsigned char)*i);
            out << std::dec;
        }

        void operator()(utf8_string_range const& str) const
        {
            typedef utf8_string_range::const_iterator iterator;
            iterator i = str.begin();
            out << '"';
            for (; i != str.end(); ++i)
                out << *i;
            out << '"';
        }

        void operator()(utf8_symbol_range const& str) const
        {
            typedef utf8_symbol_range::const_iterator iterator;
            iterator i = str.begin();
            for (; i != str.end(); ++i)
                out << *i;
        }

        template <typename Iterator>
        void operator()(boost::iterator_range<Iterator> const& range) const
        {
            typedef typename boost::iterator_range<Iterator>::const_iterator iterator;
            (*this)('(');
            for (iterator i = range.begin(); i != range.end(); ++i)
            {
                if (i != range.begin())
                    (*this)(' ');
                scheme::utree::visit(*i, *this);
            }
            (*this)(')');
        }
    };

    template <typename Base>
    struct arithmetic_function
    {
        typedef utree result_type;

        template <typename A, typename B>
        utree dispatch(A&, B&, boost::mpl::false_) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to different types
        }

        template <typename A, typename B>
        utree dispatch(A& a, B& b, boost::mpl::true_) const
        {
            return Base::eval(a, b); // for arithmetic types
        }

        template <typename A, typename B>
        utree operator()(A& a, B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_arithmetic<A>,
                    boost::is_arithmetic<B> >());
        }
    };

    template <typename Base>
    struct integral_function
    {
        typedef utree result_type;

        template <typename A, typename B>
        utree dispatch(A&, B&, boost::mpl::false_) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to different types
        }

        template <typename A, typename B>
        utree dispatch(A& a, B& b, boost::mpl::true_) const
        {
            return Base::eval(a, b); // for integral types
        }

        template <typename A, typename B>
        utree operator()(A& a, B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_integral<A>,
                    boost::is_integral<B> >());
        }
    };

#define SCHEME_CREATE_FUNCTION(name, expr, base)                                \
    struct BOOST_PP_CAT(function_impl_, name)                                   \
    {                                                                           \
        template <typename A, typename B>                                       \
        static utree eval(A& a, B& b)                                           \
        {                                                                       \
            return utree(expr);                                                 \
        }                                                                       \
    };                                                                          \
    base<BOOST_PP_CAT(function_impl_, name)> const                              \
        BOOST_PP_CAT(base, BOOST_PP_CAT(_, name)) = {};                         \
    /***/

#define SCHEME_CREATE_ARITHMETIC_FUNCTION(name, expr)                           \
    SCHEME_CREATE_FUNCTION(name, expr, arithmetic_function)                     \
    /***/

#define SCHEME_CREATE_INTEGRAL_FUNCTION(name, expr)                             \
    SCHEME_CREATE_FUNCTION(name, expr, integral_function)                       \
    /***/

#define SCHEME_CREATE_ARITHMETIC_OPERATOR(name, op)                             \
    SCHEME_CREATE_FUNCTION(name, a op b, arithmetic_function)                   \
    inline utree operator op (utree const& a, utree const& b)                   \
    {                                                                           \
        return utree::visit(a, b, BOOST_PP_CAT(arithmetic_function, name));     \
    }                                                                           \
    /***/

#define SCHEME_CREATE_INTEGRAL_OPERATOR(name, op)                               \
    SCHEME_CREATE_FUNCTION(name, a op b, integral_function)                     \
    inline utree operator op (utree const& a, utree const& b)                   \
    {                                                                           \
        return utree::visit(a, b, BOOST_PP_CAT(integral_function, name));       \
    }                                                                           \
    /***/

    inline bool operator==(utree const& a, utree const& b)
    {
        return utree::visit(a, b, utree_is_equal());
    }

    inline bool operator<(utree const& a, utree const& b)
    {
        return utree::visit(a, b, utree_is_less_than());
    }

    inline bool operator!=(utree const& a, utree const& b)
    {
        return !(a == b);
    }

    inline bool operator>(utree const& a, utree const& b)
    {
        return b < a;
    }

    inline bool operator<=(utree const& a, utree const& b)
    {
        return !(b < a);
    }

    inline bool operator>=(utree const& a, utree const& b)
    {
        return !(a < b);
    }

    inline std::ostream& operator<<(std::ostream& out, utree const& x)
    {
        utree::visit(x, utree_print(out));
        return out;
    }

    SCHEME_CREATE_ARITHMETIC_FUNCTION(plus, a+b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(minus, a-b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(times, a*b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(divides, a/b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(modulus, a%b);

    inline utree operator+(utree const& a, utree const& b)
    {
        return utree::visit(a, b, arithmetic_function_plus);
    }

    inline utree operator-(utree const& a, utree const& b)
    {
        return utree::visit(a, b, arithmetic_function_minus);
    }

    inline utree operator*(utree const& a, utree const& b)
    {
        return utree::visit(a, b, arithmetic_function_times);
    }

    inline utree operator/(utree const& a, utree const& b)
    {
        return utree::visit(a, b, arithmetic_function_divides);
    }

    inline utree operator%(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_modulus);
    }
}

#endif
