/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_IDS_HPP)
#define BOOST_SPIRIT_CONJURE_IDS_HPP

namespace client
{
    struct op_type
    {
        enum type
        {
            binary = 0x20000,
            unary = 0x40000,
            assign = 0x80000
        };
    };

    struct op
    {
        enum type
        {
            comma,
            assign,
            plus_assign,
            minus_assign,
            times_assign,
            divide_assign,
            mod_assign,
            bit_and_assign,
            bit_xor_assign,
            bit_or_assign,
            shift_left_assign,
            shift_right_assign,
            logical_or,
            logical_and,
            bit_or,
            bit_xor,
            bit_and,
            equal,
            not_equal,
            less,
            less_equal,
            greater,
            greater_equal,
            shift_left,
            shift_right,
            plus,
            minus,
            times,
            divide,
            mod,
            positive,
            negative,
            pre_incr,
            pre_decr,
            compl_,
            not_,
            post_incr,
            post_decr
        };
    };

    template <int type, int op, int precedence>
    struct make_op
    {
        static int const value =
                (precedence << 19)
            +   type
            +   op
            ;
    };

    template <int op, int precedence>
    struct unary_op : make_op<op_type::unary, op, precedence> {};

    template <int op, int precedence>
    struct binary_op
        : make_op<op_type::binary, op, precedence> {};

    template <int op, int precedence>
    struct assign_op
        : make_op<op_type::binary | op_type::assign, op, precedence> {};

    struct token
    {
        enum type
        {
            // pseudo tags
            invalid             = -1,
            binary              = op_type::binary,
            unary               = op_type::unary,

            // binary operators precedence 1
            comma               = binary_op<op::comma, 1>::value,

            // binary operators precedence 2
            assign              = assign_op<op::assign, 2>::value,
            plus_assign         = assign_op<op::plus_assign, 2>::value,
            minus_assign        = assign_op<op::minus_assign, 2>::value,
            times_assign        = assign_op<op::times_assign, 2>::value,
            divide_assign       = assign_op<op::divide_assign, 2>::value,
            mod_assign          = assign_op<op::mod_assign, 2>::value,
            bit_and_assign      = assign_op<op::bit_and_assign, 2>::value,
            bit_xor_assign      = assign_op<op::bit_xor_assign, 2>::value,
            bit_or_assign       = assign_op<op::bit_or_assign, 2>::value,
            shift_left_assign   = assign_op<op::shift_left_assign, 2>::value,
            shift_right_assign  = assign_op<op::shift_right_assign, 2>::value,

            // binary operators precedence 3
            logical_or          = binary_op<op::logical_or, 3>::value,

            // binary operators precedence 4
            logical_and         = binary_op<op::logical_and, 4>::value,

            // binary operators precedence 5
            bit_or              = binary_op<op::bit_or, 5>::value,

            // binary operators precedence 6
            bit_xor             = binary_op<op::bit_xor, 6>::value,

            // binary operators precedence 7
            bit_and             = binary_op<op::bit_and, 7>::value,

            // binary operators precedence 8
            equal               = binary_op<op::equal, 8>::value,
            not_equal           = binary_op<op::not_equal, 8>::value,

            // binary operators precedence 9
            less                = binary_op<op::less, 9>::value,
            less_equal          = binary_op<op::less_equal, 9>::value,
            greater             = binary_op<op::greater, 9>::value,
            greater_equal       = binary_op<op::greater_equal, 9>::value,

            // binary operators precedence 10
            shift_left          = binary_op<op::shift_left, 10>::value,
            shift_right         = binary_op<op::shift_right, 10>::value,

            // binary operators precedence 11
            plus                = binary_op<op::plus, 11>::value,
            minus               = binary_op<op::minus, 11>::value,

            // binary operators precedence 12
            times               = binary_op<op::times, 12>::value,
            divide              = binary_op<op::divide, 12>::value,
            mod                 = binary_op<op::mod, 12>::value,

            // unary operators precedence 13
            positive            = unary_op<op::positive, 13>::value,
            negative            = unary_op<op::negative, 13>::value,
            pre_incr            = unary_op<op::pre_incr, 13>::value,
            pre_decr            = unary_op<op::pre_decr, 13>::value,
            compl_              = unary_op<op::compl_, 13>::value,
            not_                = unary_op<op::not_, 13>::value,

            // unary operators precedence 14
            post_incr           = unary_op<op::post_incr, 14>::value,
            post_decr           = unary_op<op::post_decr, 14>::value,

            // misc tags
            identifier          = op::post_decr + 1,
            comment,
            whitespace,
            lit_uint,
            true_or_false
        };
    };

    inline int precedence_of(token::type op)
    {
        return (op >> 19) & 0xF;
    }
}

#endif
