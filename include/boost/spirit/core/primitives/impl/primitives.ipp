/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 1998-2003 Joel de Guzman
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_PRIMITIVES_IPP)
#define BOOST_SPIRIT_PRIMITIVES_IPP

#include <cctype>
#if !defined(BOOST_NO_CWCTYPE)
#include <cwctype>
#endif

#if defined(BOOST_MSVC)
#pragma warning(disable:4800)
#endif

namespace boost { namespace spirit {

    template <typename DrivedT> struct char_parser;

    namespace impl {

    template <typename IteratorT>
    inline IteratorT
    get_last(IteratorT first)
    {
        while (*first)
            first++;
        return first;
    }

    template<
        typename RT,
        typename IteratorT,
        typename ScannerT>
    inline RT
    string_parser_parse(
        IteratorT str_first,
        IteratorT str_last,
        ScannerT& scan)
    {
        typedef typename ScannerT::iterator_t iterator_t;
        iterator_t saved = scan.first;
        int slen = str_last - str_first;

        while (str_first != str_last)
        {
            if (scan.at_end() || (*str_first != *scan))
                return scan.no_match();
            ++str_first;
            ++scan;
        }

        return scan.create_match(slen, nil_t(), saved, scan.first);
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Convenience functions
    //
    ///////////////////////////////////////////////////////////////////////////
    inline bool
    isalnum_(int c)
    { using namespace std; return isalnum(c); }

    inline bool
    isalpha_(int c)
    { using namespace std; return isalpha(c); }

    inline bool
    iscntrl_(int c)
    { using namespace std; return iscntrl(c); }

    inline bool
    isdigit_(int c)
    { using namespace std; return isdigit(c); }

    inline bool
    isgraph_(int c)
    { using namespace std; return isgraph(c); }

    inline bool
    islower_(int c)
    { using namespace std; return islower(c); }

    inline bool
    isprint_(int c)
    { using namespace std; return isprint(c); }

    inline bool
    ispunct_(int c)
    { using namespace std; return ispunct(c); }

    inline bool
    isspace_(int c)
    { using namespace std; return isspace(c); }

    inline bool
    isupper_(int c)
    { using namespace std; return isupper(c); }

    inline bool
    isxdigit_(int c)
    { using namespace std; return isxdigit(c); }

    inline char
    tolower_(int c)
    { using namespace std; return static_cast<char>(tolower(c)); }

    inline char
    toupper_(int c)
    { using namespace std; return static_cast<char>(toupper(c)); }

#if !defined(BOOST_NO_CWCTYPE)

    inline bool
    isalnum_(wchar_t c)
    { using namespace std; return iswalnum(c); }

    inline bool
    isalpha_(wchar_t c)
    { using namespace std; return iswalpha(c); }

    inline bool
    iscntrl_(wchar_t c)
    { using namespace std; return iswcntrl(c); }

    inline bool
    isdigit_(wchar_t c)
    { using namespace std; return iswdigit(c); }

    inline bool
    isgraph_(wchar_t c)
    { using namespace std; return iswgraph(c); }

    inline bool
    islower_(wchar_t c)
    { using namespace std; return iswlower(c); }

    inline bool
    isprint_(wchar_t c)
    { using namespace std; return iswprint(c); }

    inline bool
    ispunct_(wchar_t c)
    { using namespace std; return iswpunct(c); }

    inline bool
    isspace_(wchar_t c)
    { using namespace std; return iswspace(c); }

    inline bool
    isupper_(wchar_t c)
    { using namespace std; return iswupper(c); }

    inline bool
    isxdigit_(wchar_t c)
    { using namespace std; return iswxdigit(c); }

    inline wchar_t
    tolower_(wchar_t c)
    { using namespace std; return towlower(c); }

    inline wchar_t
    toupper_(wchar_t c)
    { using namespace std; return towupper(c); }

#endif // !defined(BOOST_NO_CWCTYPE)

}}} // namespace boost::spirit::impl

#endif
