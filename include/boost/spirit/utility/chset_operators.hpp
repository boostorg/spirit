/*=============================================================================
    Spirit v1.6.1
    Copyright (c) 2001-2003 Joel de Guzman
    Copyright (c) 2001-2003 Daniel Nuffer
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#ifndef BOOST_SPIRIT_CHSET_OPERATORS_HPP
#define BOOST_SPIRIT_CHSET_OPERATORS_HPP

///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/utility/chset.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

///////////////////////////////////////////////////////////////////////////////
//
//  chset free operators
//
//      Where a and b are both chsets, implements:
//
//          a | b, a & b, a - b, a ^ b
//
//      Where a is a chset, implements:
//
//          ~a
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator~(chset<CharT> const& a);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
//
//  range <--> chset free operators
//
//      Where a is a chset and b is a range, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, range<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, range<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, range<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, range<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(range<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(range<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(range<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(range<CharT> const& a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
//
//  chlit <--> chset free operators
//
//      Where a is a chset and b is a chlit, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, chlit<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, chlit<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, chlit<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, chlit<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chlit<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chlit<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chlit<CharT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chlit<CharT> const& a, chset<CharT> const& b);

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)

///////////////////////////////////////////////////////////////////////////////
//
//  negated_char_parser <--> chset free operators
//
//      Where a is a chset and b is a range, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<ParserT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<ParserT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<ParserT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<ParserT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator|(negated_char_parser<ParserT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator&(negated_char_parser<ParserT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator-(negated_char_parser<ParserT> const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT, typename ParserT>
chset<CharT>
operator^(negated_char_parser<ParserT> const& a, chset<CharT> const& b);

#else // BOOST_WORKAROUND(BOOST_MSVC, < 1300)

///////////////////////////////////////////////////////////////////////////////
//
//  negated_char_parser<range> <--> chset free operators
//
//      Where a is a chset and b is a range, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
//
//  negated_char_parser<chlit> <--> chset free operators
//
//      Where a is a chset and b is a chlit, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);

#endif // BOOST_WORKAROUND(BOOST_MSVC, < 1300)

///////////////////////////////////////////////////////////////////////////////
//
//  literal primitives <--> chset free operators
//
//      Where a is a chset and b is a literal primitive,
//      and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, CharT b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, CharT b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, CharT b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, CharT b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(CharT a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(CharT a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(CharT a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(CharT a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
//
//  anychar_parser <--> chset free operators
//
//      Where a is chset and b is a anychar_parser, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, anychar_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, anychar_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, anychar_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, anychar_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(anychar_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(anychar_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(anychar_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(anychar_parser a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
//
//  nothing_parser <--> chset free operators
//
//      Where a is chset and b is nothing_parser, and vice-versa, implements:
//
//          a | b, a & b, a - b, a ^ b
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, nothing_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, nothing_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, nothing_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, nothing_parser b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator|(nothing_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator&(nothing_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator-(nothing_parser a, chset<CharT> const& b);

//////////////////////////////////
template <typename CharT>
chset<CharT>
operator^(nothing_parser a, chset<CharT> const& b);

///////////////////////////////////////////////////////////////////////////////
}} // namespace boost::spirit

#endif

#include <boost/spirit/utility/impl/chset_operators.ipp>
