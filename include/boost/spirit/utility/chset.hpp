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
#ifndef BOOST_SPIRIT_CHSET_HPP
#define BOOST_SPIRIT_CHSET_HPP

///////////////////////////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/spirit/core/primitives/primitives.hpp>
#include <boost/spirit/utility/impl/chset/basic_chset.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit {

namespace utility { namespace impl {

    // This is here because some compilers choke on out-of-line member
    // template functions.  And we don't want to put the whole algorithm
    // in the chset constructor in the class definition.
    template <typename CharT, typename CharT2>
    void construct_chset(boost::shared_ptr<basic_chset<CharT> >& ptr,
            CharT2 const* definition);

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)

    template <typename CharT, typename FakeT>
    void chset_negated_set(boost::shared_ptr<basic_chset<CharT> > &ptr, chlit<CharT> const &ch,
            FakeT);
    
    template <typename CharT, typename FakeT>
    void chset_negated_set(boost::shared_ptr<basic_chset<CharT> > &ptr,
            spirit::range<CharT> const &rng, FakeT);

#endif // BOOST_WORKAROUND(BOOST_MSVC, < 1300)
}} // namespace utility::impl

///////////////////////////////////////////////////////////////////////////////
//
//  chset class
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT = char>
class chset: public char_parser<chset<CharT> > {

public:
                    chset();
                    chset(chset const& arg);
    explicit        chset(CharT arg);
    explicit        chset(anychar_parser arg);
    explicit        chset(nothing_parser arg);
    explicit        chset(chlit<CharT> const& arg);
    explicit        chset(range<CharT> const& arg);
#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
                    template <typename ParserT>
    explicit        chset(negated_char_parser<ParserT> const& arg)
                    : ptr(new basic_chset<CharT>())
                    {
                        set(arg);
                    }
#else
    explicit        chset(negated_char_parser<chlit<CharT> > const& arg);
    explicit        chset(negated_char_parser<range<CharT> > const& arg);
#endif

                    template <typename CharT2>
    explicit        chset(CharT2 const* definition)
                    : ptr(new basic_chset<CharT>())
                    {
                        utility::impl::construct_chset(ptr, definition);
                    }
                    ~chset();

    chset&          operator=(chset const& rhs);
    chset&          operator=(CharT rhs);
    chset&          operator=(anychar_parser rhs);
    chset&          operator=(nothing_parser rhs);
    chset&          operator=(chlit<CharT> const& rhs);
    chset&          operator=(range<CharT> const& rhs);
#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
                    template <typename ParserT>
    chset&          operator=(negated_char_parser<ParserT> const& rhs)
                    {
                        utility::impl::detach_clear(ptr);
                        set(rhs);
                        return *this;
                    }
        
#else
    chset&          operator=(negated_char_parser<chlit<CharT> > const& rhs);
    chset&          operator=(negated_char_parser<range<CharT> > const& rhs);
#endif

    void            set(range<CharT> const& arg);
#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
                    template <typename ParserT>
    void            set(negated_char_parser<ParserT> const& arg)
                    {
                        utility::impl::detach(ptr);
                        utility::impl::chset_negated_set(ptr, arg.positive, 0);
                    }
#else
    void            set(negated_char_parser<chlit<CharT> > const& arg);
    void            set(negated_char_parser<range<CharT> > const& arg);
#endif
    void            clear(range<CharT> const& arg);
    void            clear(negated_char_parser<range<CharT> > const& arg);
    bool            test(CharT ch) const;
    chset&          inverse();
    void            swap(chset& x);

    chset&          operator|=(chset const& x);
    chset&          operator&=(chset const& x);
    chset&          operator-=(chset const& x);
    chset&          operator^=(chset const& x);

private:

    boost::shared_ptr<basic_chset<CharT> > ptr;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Generator functions
//
///////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline chset<wchar_t>
chset_p(chlit<CharT> const& arg)
{ return chset<CharT>(arg); }

//////////////////////////////////
template <typename CharT>
inline chset<wchar_t>
chset_p(range<CharT> const& arg)
{ return chset<CharT>(arg); }

//////////////////////////////////
#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)

template <typename ParserT>
inline chset<wchar_t>
chset_p(negated_char_parser<ParserT> const& arg)
{ return chset<typename ParserT::char_t>(arg); }

#else // BOOST_WORKAROUND(BOOST_MSVC, < 1300)

template <typename CharT>
inline chset<wchar_t>
chset_p(negated_char_parser<chlit<CharT> > const& arg)
{ return chset<CharT>(arg); }

template <typename CharT>
inline chset<wchar_t>
chset_p(negated_char_parser<range<CharT> > const& arg)
{ return chset<CharT>(arg); }

#endif // BOOST_WORKAROUND(BOOST_MSVC, < 1300)

//////////////////////////////////
inline chset<char>
chset_p(char const* init)
{ return chset<char>(init); }

//////////////////////////////////
inline chset<wchar_t>
chset_p(wchar_t const* init)
{ return chset<wchar_t>(init); }

//////////////////////////////////
inline chset<char>
chset_p(char ch)
{ return chset<char>(ch); }

//////////////////////////////////
inline chset<wchar_t>
chset_p(wchar_t ch)
{ return chset<wchar_t>(ch); }

//////////////////////////////////
inline chset<int>
chset_p(int ch)
{ return chset<int>(ch); }

//////////////////////////////////
inline chset<unsigned int>
chset_p(unsigned int ch)
{ return chset<unsigned int>(ch); }

//////////////////////////////////
inline chset<short>
chset_p(short ch)
{ return chset<short>(ch); }

#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
//////////////////////////////////
inline chset<unsigned short>
chset_p(unsigned short ch)
{ return chset<unsigned short>(ch); }
#endif
//////////////////////////////////
inline chset<long>
chset_p(long ch)
{ return chset<long>(ch); }

//////////////////////////////////
inline chset<unsigned long>
chset_p(unsigned long ch)
{ return chset<unsigned long>(ch); }

#ifdef BOOST_HAS_LONG_LONG
//////////////////////////////////
inline chset<long long>
chset_p(long long ch)
{ return chset<long long>(ch); }

//////////////////////////////////
inline chset<unsigned long long>
chset_p(unsigned long long ch)
{ return chset<unsigned long long>(ch); }
#endif

///////////////////////////////////////////////////////////////////////////////
}} // namespace boost::spirit

#endif

#include <boost/spirit/utility/impl/chset.ipp>
#include <boost/spirit/utility/chset_operators.hpp>
