/*=============================================================================
    Copyright (c) 2001-2003 Daniel Nuffer
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/utility/escape_char.hpp>

#include <iostream>
#include <cassert>
#include <cstdio>       // for sprintf

#ifndef BOOST_NO_CWCHAR
# include <cwchar>      // for swprintf
#endif

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    char c;

    // testing good C escapes
    assert(parse("a", c_escape_ch_p[assign_a(c)]).full);
    assert(c == 'a');
    assert(parse("\\b", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\b');
    assert(parse("\\t", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\t');
    assert(parse("\\n", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\n');
    assert(parse("\\f", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\f');
    assert(parse("\\r", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\r');
    assert(parse("\\\"", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\"');
    assert(parse("\\'", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\'');
    assert(parse("\\\\", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\\');
    assert(parse("\\120", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\120');
    assert(parse("\\x2e", c_escape_ch_p[assign_a(c)]).full);
    assert(c == '\x2e');

    // test bad C escapes
    assert(!parse("\\z", c_escape_ch_p[assign_a(c)]).hit);

    // testing good lex escapes
    assert(parse("a", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == 'a');
    assert(parse("\\b", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\b');
    assert(parse("\\t", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\t');
    assert(parse("\\n", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\n');
    assert(parse("\\f", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\f');
    assert(parse("\\r", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\r');
    assert(parse("\\\"", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\"');
    assert(parse("\\'", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\'');
    assert(parse("\\\\", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\\');
    assert(parse("\\120", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\120');
    assert(parse("\\x2e", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\x2e');
    assert(parse("\\z", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == 'z');
    assert(parse("\\a", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == 'a');

    // test bad lex escapes
    assert(!parse("\\xz", lex_escape_ch_p[assign_a(c)]).hit);

    // test out of range octal escape
    assert(!parse("\\777", lex_escape_ch_p[assign_a(c)]).hit);

#if CHAR_MAX == 127
    assert(!parse("\\200", lex_escape_ch_p[assign_a(c)]).hit);

    assert(parse("\\177", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\177');
#elif CHAR_MAX == 255
    assert(!parse("\\400", lex_escape_ch_p[assign_a(c)]).hit);

    assert(parse("\\377", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\377');
#endif

    // test out of range hex escape
    assert(!parse("\\xFFF", lex_escape_ch_p[assign_a(c)]).hit);

#if CHAR_MAX == 127
    assert(!parse("\\X80", lex_escape_ch_p[assign_a(c)]).hit);

    assert(parse("\\X7F", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\x7f');
#elif CHAR_MAX == 255
    assert(!parse("\\X100", lex_escape_ch_p[assign_a(c)]).hit);

    assert(parse("\\XFf", lex_escape_ch_p[assign_a(c)]).full);
    assert(c == '\xff');
#endif

#ifndef BOOST_NO_CWCHAR

    // test wide chars
    typedef escape_char_parser<lex_escapes, wchar_t> wlep_t;
    wlep_t wlep = wlep_t();

    typedef escape_char_parser<c_escapes, wchar_t> wcep_t;
    wcep_t wcep = wcep_t();

    //wchar_t const* wstr = L"a\\b\\t\\n\\f\\r\\\"\\'\\\\\\120\\x2e";
    //wchar_t const* wend(wstr + wcslen(wstr));

    wchar_t wc;
    assert(parse(L"a", wcep[assign_a(wc)]).hit);
    assert(wc == L'a');
    assert(parse(L"\\b", wcep[assign_a(wc)]).full);
    assert(wc == L'\b');
    assert(parse(L"\\t", wcep[assign_a(wc)]).full);
    assert(wc == L'\t');
    assert(parse(L"\\n", wcep[assign_a(wc)]).full);
    assert(wc == L'\n');
    assert(parse(L"\\f", wcep[assign_a(wc)]).full);
    assert(wc == L'\f');
    assert(parse(L"\\r", wcep[assign_a(wc)]).full);
    assert(wc == L'\r');
    assert(parse(L"\\\"", wcep[assign_a(wc)]).full);
    assert(wc == L'\"');
    assert(parse(L"\\'", wcep[assign_a(wc)]).full);
    assert(wc == L'\'');
    assert(parse(L"\\\\", wcep[assign_a(wc)]).full);
    assert(wc == L'\\');
    assert(parse(L"\\120", wcep[assign_a(wc)]).full);
    assert(wc == L'\120');
    assert(parse(L"\\x2e", wcep[assign_a(wc)]).full);
    assert(wc == L'\x2e');

    // test bad wc escapes
    assert(!parse(L"\\z", wcep[assign_a(wc)]).hit);

    // test out of range octal escape
    size_t const octmax_size = 16;
    wchar_t octmax[octmax_size];

    swprintf(octmax, octmax_size,
      L"\\%lo", (unsigned long)(std::numeric_limits<wchar_t>::max)());
    assert(parse(octmax, wlep[assign_a(wc)]).full);
    //assert(lex_escape_ch_p[assign_a(wc)].parse(str, end));
    assert(wc == (std::numeric_limits<wchar_t>::max)());

    swprintf(octmax, octmax_size,
      L"\\%lo", (unsigned long)(std::numeric_limits<wchar_t>::max)() + 1);
    assert(!parse(octmax, wlep[assign_a(wc)]).hit);

    // test out of range hex escape
    size_t const hexmax_size = 16;
    wchar_t hexmax[hexmax_size];

    swprintf(hexmax, hexmax_size,
      L"\\x%lx", (unsigned long)(std::numeric_limits<wchar_t>::max)());
    assert(parse(hexmax, wlep[assign_a(wc)]).full);
    assert(wc == (std::numeric_limits<wchar_t>::max)());

    swprintf(hexmax, hexmax_size,
      L"\\x%lx", (unsigned long)(std::numeric_limits<wchar_t>::max)() + 1);
    assert(!parse(hexmax, wlep[assign_a(wc)]).hit);

#endif

    return 0;
}
