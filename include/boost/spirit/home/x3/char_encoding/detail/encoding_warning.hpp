/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_X3_CHAR_ENCODING_DETAIL_ENCODING_WARNING_HPP
#define BOOST_SPIRIT_X3_CHAR_ENCODING_DETAIL_ENCODING_WARNING_HPP

#define BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING \
    "Hard-coding ASCII or Latin-1 assumptions is almost always wrong " \
    "in modern applications and can introduce security vulnerabilities. " \
    "Unless the input is provably ASCII-only (e.g., fixed-format data from " \
    "the 1980s), the only correct interpretation uses the data's *original* " \
    "encoding. Prefer transcoding to Unicode or to the implementation-defined " \
    "execution character set before passing text to general-purpose parsers. " \
    "For more information, refer to authoritative sources on C++ character " \
    "encodings and Unicode fundamentals."

#endif
