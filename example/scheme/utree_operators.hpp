/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_UTREE_OPERATORS)
#define BOOST_SPIRIT_UTREE_OPERATORS

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4804)
# pragma warning(disable: 4805)
#endif

#include "utree.hpp"
#include <boost/preprocessor/cat.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>

namespace scheme
{
    // Relational operators
    bool operator==(utree const& a, utree const& b);
    bool operator<(utree const& a, utree const& b);
    bool operator!=(utree const& a, utree const& b);
    bool operator>(utree const& a, utree const& b);
    bool operator<=(utree const& a, utree const& b);
    bool operator>=(utree const& a, utree const& b);

    // Input and output
    std::ostream& operator<<(std::ostream& out, utree const& x);
    std::istream& operator>>(std::istream& in, utree& x);

    // Logical operators
    utree operator&&(utree const& a, utree const& b);
    utree operator||(utree const& a, utree const& b);
    utree operator!(utree const& a);

    // Arithmetic operators
    utree operator+(utree const& a, utree const& b);
    utree operator-(utree const& a, utree const& b);
    utree operator*(utree const& a, utree const& b);
    utree operator/(utree const& a, utree const& b);
    utree operator%(utree const& a, utree const& b);
    utree operator-(utree const& a);

    // Bitwise operators
    utree operator&(utree const& a, utree const& b);
    utree operator|(utree const& a, utree const& b);
    utree operator^(utree const& a, utree const& b);
    utree operator<<(utree const& a, utree const& b);
    utree operator>>(utree const& a, utree const& b);
    utree operator~(utree const& a);

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

//     struct utree_print
//     {
//         typedef void result_type;
// 
//         std::ostream& out;
//         utree_print(std::ostream& out) : out(out) {}
// 
//         void operator()(scheme::nil) const
//         {
//             out << "nil";
//         }
// 
//         template <typename T>
//         void operator()(T val) const
//         {
//             out << val;
//         }
// 
//         void operator()(bool b) const
//         {
//             out << (b ? "true" : "false");
//         }
// 
//         void operator()(binary_range const& b) const
//         {
//             out << "b";
//             out.width(2);
//             out.fill('0');
// 
//             typedef binary_range::const_iterator iterator;
//             for (iterator i = b.begin(); i != b.end(); ++i)
//                 out << std::hex << int((unsigned char)*i);
//             out << std::dec;
//         }
// 
//         void operator()(utf8_string_range const& str) const
//         {
//             typedef utf8_string_range::const_iterator iterator;
//             iterator i = str.begin();
//             out << '"';
//             for (; i != str.end(); ++i)
//                 out << *i;
//             out << '"';
//         }
// 
//         void operator()(utf8_symbol_range const& str) const
//         {
//             typedef utf8_symbol_range::const_iterator iterator;
//             iterator i = str.begin();
//             for (; i != str.end(); ++i)
//                 out << *i;
//         }
// 
//         template <typename Iterator>
//         void operator()(boost::iterator_range<Iterator> const& range) const
//         {
//             typedef typename boost::iterator_range<Iterator>::const_iterator iterator;
//             (*this)('(');
//             for (iterator i = range.begin(); i != range.end(); ++i)
//             {
//                 if (i != range.begin())
//                     (*this)(' ');
//                 scheme::utree::visit(*i, *this);
//             }
//             (*this)(')');
//         }
//     };

    template <typename Base>
    struct logical_function
    {
        typedef utree result_type;

        // binary
        utree operator()(bool a, bool b) const
        {
            return Base::eval(a, b); // for boolean types
        }

        // binary
        template <typename A, typename B>
        utree operator()(A const& a, B const& b) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to non booleans
        }

        // unary
        utree operator()(bool a) const
        {
            return Base::eval(a); // for boolean types
        }

        // unary
        template <typename A>
        utree operator()(A const& a) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to non booleans
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
            return utree(); // cannot apply to non-arithmetic types
        }

        template <typename A, typename B>
        utree dispatch(A& a, B& b, boost::mpl::true_) const
        {
            return Base::eval(a, b); // for arithmetic types
        }

        // binary
        template <typename A, typename B>
        utree operator()(A& a, B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_arithmetic<A>,
                    boost::is_arithmetic<B> >());
        }

        template <typename A>
        utree dispatch(A&, boost::mpl::false_) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to non-arithmetic types
        }

        template <typename A>
        utree dispatch(A& a, boost::mpl::true_) const
        {
            return Base::eval(a); // for arithmetic types
        }

        // unary
        template <typename A>
        utree operator()(A& a) const
        {
            return dispatch(a, boost::is_arithmetic<A>());
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
            return utree(); // cannot apply to non-integral types
        }

        template <typename A, typename B>
        utree dispatch(A& a, B& b, boost::mpl::true_) const
        {
            return Base::eval(a, b); // for integral types
        }

        // binary
        template <typename A, typename B>
        utree operator()(A& a, B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_integral<A>,
                    boost::is_integral<B> >());
        }

        template <typename A>
        utree dispatch(A&, boost::mpl::false_) const
        {
            // $$$ Throw exception here? $$$
            return utree(); // cannot apply to non-integral types
        }

        template <typename A>
        utree dispatch(A& a, boost::mpl::true_) const
        {
            return Base::eval(a); // for integral types
        }

        // unary
        template <typename A>
        utree operator()(A& a) const
        {
            return dispatch(a, boost::is_integral<A>());
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
        template <typename A>                                                   \
        static utree eval(A& a)                                                 \
        {                                                                       \
            static int b;                                                       \
            (void) b;                                                           \
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

#define SCHEME_CREATE_LOGICAL_FUNCTION(name, expr)                              \
    SCHEME_CREATE_FUNCTION(name, expr, logical_function)                        \
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

//     inline std::ostream& operator<<(std::ostream& out, utree const& x)
//     {
//         utree::visit(x, utree_print(out));
//         return out;
//     }

    SCHEME_CREATE_LOGICAL_FUNCTION(and_, a&&b);
    SCHEME_CREATE_LOGICAL_FUNCTION(or_, a||b);
    SCHEME_CREATE_LOGICAL_FUNCTION(not_, !a);

    SCHEME_CREATE_ARITHMETIC_FUNCTION(plus, a+b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(minus, a-b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(times, a*b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(divides, a/b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(modulus, a%b);
    SCHEME_CREATE_ARITHMETIC_FUNCTION(negate, -a);

    SCHEME_CREATE_INTEGRAL_FUNCTION(bitand_, a&b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(bitor_, a|b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(bitxor_, a^b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(shift_left, a<<b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(shift_right, a>>b);
    SCHEME_CREATE_INTEGRAL_FUNCTION(invert, ~a);

    inline utree operator&&(utree const& a, utree const& b)
    {
        return utree::visit(a, b, logical_function_and_);
    }

    inline utree operator||(utree const& a, utree const& b)
    {
        return utree::visit(a, b, logical_function_or_);
    }

    inline utree operator!(utree const& a)
    {
        return utree::visit(a, logical_function_not_);
    }

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

    inline utree operator-(utree const& a)
    {
        return utree::visit(a, arithmetic_function_negate);
    }

    inline utree operator&(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_bitand_);
    }

    inline utree operator|(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_bitor_);
    }

    inline utree operator^(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_bitxor_);
    }

    inline utree operator<<(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_shift_left);
    }

    inline utree operator>>(utree const& a, utree const& b)
    {
        return utree::visit(a, b, integral_function_shift_right);
    }

    inline utree operator~(utree const& a)
    {
        return utree::visit(a, integral_function_invert);
    }
}

#if defined(BOOST_MSVC)
# pragma warning(pop)
#endif

#endif
