/*=============================================================================
    A Standard compliant C++ preprocessor

    Grammar for universal character validation (see C++ standard: Annex E)
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/
#if !defined(_VALIDATE_UNIVERSAL_CHAR_HPP__55F1B811_CD76_4C72_8344_CBC69CF3B339__INCLUDED_)
#define _VALIDATE_UNIVERSAL_CHAR_HPP__55F1B811_CD76_4C72_8344_CBC69CF3B339__INCLUDED_

#include <boost/spirit/core.hpp>

#include "cpplexer/cpplexer_exceptions.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace impl {

enum universal_char_type {
    universal_char_type_valid = 0,
    universal_char_type_invalid = 1,
    universal_char_type_base_charset = 2,
    universal_char_type_not_allowed = 3,
};

namespace {
    ///////////////////////////////////////////////////////////////////////////
    // 
    //  is_range is a helper function for the classification by brute force 
    //  below
    //
    ///////////////////////////////////////////////////////////////////////////
    inline bool 
    in_range(unsigned long ch, unsigned long l, unsigned long u)
    {
        return (l <= ch && ch <= u);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  classify_universal_char
//
//      This function classifies an universal character value into 4 subranges:
//      universal_char_type_valid
//          the universal character value is valid for identifiers
//      universal_char_type_invalid
//          the universal character value is not valid for its usage inside
//          identifiers (see C++ Standard: 2.2.2 [lex.charset])
//      universal_char_type_base_charset
//          the universal character value designates a character from the base
//          character set
//      universal_char_type_not_allowed
//          the universal character value is not allowed in an identifier
//
//  Implementation note:
//      This classification isn't implemented very effectively here. This
//      function should be rewritten with some range run matching algorithm.
//      
///////////////////////////////////////////////////////////////////////////////
inline universal_char_type 
classify_universal_char (unsigned long ch)
{
// test for invalid characters
    if (ch <= 0x0020 || in_range(ch, 0x007f, 0x009f))
        return universal_char_type_invalid;
    
// test for characters in the range of the base character set
    if (in_range(ch, 0x0021, 0x005f) || in_range(ch, 0x0061, 0x007e))
        return universal_char_type_base_charset;

// test for additional valid character values (see C++ Standard: Annex E)
    if (in_range(ch, '\u00c0', '\u00d6') || in_range(ch, '\u00d8', '\u00f6') ||
        in_range(ch, '\u00f8', '\u01f5') || in_range(ch, '\u01fa', '\u0217') ||
        in_range(ch, '\u0250', '\u02a8') || in_range(ch, '\u1e00', '\u1e9a') ||
        in_range(ch, '\u1ea0', '\u1ef9'))
    {
        return universal_char_type_valid;   // Latin
    }
    
    if ('\u0384' == ch || in_range(ch, '\u0388', '\u038a') ||
        '\u038c' == ch || in_range(ch, '\u038e', '\u03a1') ||
        in_range(ch, '\u03a3', '\u03ce') || in_range(ch, '\u03d0', '\u03d6') ||
        '\u03da' == ch || '\u03dc' == ch || '\u03de' == ch || '\u03e0' == ch ||
        in_range(ch, '\u03e2', '\u03f3') || in_range(ch, '\u1f00', '\u1f15') ||
        in_range(ch, '\u1f18', '\u1f1d') || in_range(ch, '\u1f20', '\u1f45') ||
        in_range(ch, '\u1f48', '\u1f4d') || in_range(ch, '\u1f50', '\u1f57') ||
        '\u1f59' == ch || '\u1f5b' == ch || '\u1f5d' == ch || 
        in_range(ch, '\u1f5f', '\u1f7d') || in_range(ch, '\u1f80', '\u1fb4') ||
        in_range(ch, '\u1fb6', '\u1fbc') || in_range(ch, '\u1fc2', '\u1fc4') ||
        in_range(ch, '\u1fc6', '\u1fcc') || in_range(ch, '\u1fd0', '\u1fd3') ||
        in_range(ch, '\u1fd6', '\u1fdb') || in_range(ch, '\u1fe0', '\u1fec') ||
        in_range(ch, '\u1ff2', '\u1ff4') || in_range(ch, '\u1ff6', '\u1ffc'))
    {
        return universal_char_type_valid;   // Greek
    }
    
    if (in_range(ch, '\u0401', '\u040d') || in_range(ch, '\u040f', '\u044f') ||
        in_range(ch, '\u0451', '\u045c') || in_range(ch, '\u045e', '\u0481') ||
        in_range(ch, '\u0490', '\u04c4') || in_range(ch, '\u04c7', '\u04c8') ||
        in_range(ch, '\u04cb', '\u04cc') || in_range(ch, '\u04d0', '\u04eb') ||
        in_range(ch, '\u04ee', '\u04f5') || in_range(ch, '\u04f8', '\u04f9'))
    {
        return universal_char_type_valid;   // Cyrillic
    }
    
    if (in_range(ch, '\u0531', '\u0556') || in_range(ch, '\u0561', '\u0587'))
        return universal_char_type_valid;   // Armenian

    if (in_range(ch, '\u05d0', '\u05ea') || in_range(ch, '\u05f0', '\u05f4'))
        return universal_char_type_valid;   // Hebrew
        
    if (in_range(ch, '\u0621', '\u063a') || in_range(ch, '\u0640', '\u0652') ||
        in_range(ch, '\u0670', '\u06b7') || in_range(ch, '\u06ba', '\u06be') ||
        in_range(ch, '\u06c0', '\u06ce') || in_range(ch, '\u06e5', '\u06e7'))
    {
        return universal_char_type_valid;   // Arabic
    }
    
    if (in_range(ch, '\u0905', '\u0939') || in_range(ch, '\u0958', '\u0962'))
        return universal_char_type_valid;   // Devanagari

    if (in_range(ch, '\u0985', '\u098c') || in_range(ch, '\u098f', '\u0990') ||
        in_range(ch, '\u0993', '\u09a8') || in_range(ch, '\u09aa', '\u09b0') ||
        '\u09b2' == ch || in_range(ch, '\09b6', '\u09b9') ||
        in_range(ch, '\u09dc', '\u09dd') || in_range(ch, '\u09df', '\u09e1') ||
        in_range(ch, '\u09f0', '\u09f1'))
    {
        return universal_char_type_valid;   // Bengali
    }
    
    if (in_range(ch, '\u0a05', '\u0a0a') || in_range(ch, '\u0a0f', '\u0a10') ||
        in_range(ch, '\u0a13', '\u0a28') || in_range(ch, '\u0a2a', '\u0a30') ||
        in_range(ch, '\u0a32', '\u0a33') || in_range(ch, '\u0a35', '\u0a36') ||
        in_range(ch, '\u0a38', '\u0a39') || in_range(ch, '\u0a59', '\u0a5c') ||
        '\u0a5e' == ch)
    {
        return universal_char_type_valid;   // Gurmukhi
    }

    if (in_range(ch, '\u0a85', '\u0a8b') || '\u0a8d' == ch ||
        in_range(ch, '\u0a8f', '\u0a91') || in_range(ch, '\u0a93', '\u0aa8') ||
        in_range(ch, '\u0aaa', '\u0ab0') || in_range(ch, '\u0ab2', '\u0ab3') ||
        in_range(ch, '\u0ab5', '\u0ab9') || '\u0ae0' == ch)
    {
        return universal_char_type_valid;   // Gujarati
    }

    if (in_range(ch, '\u0b05', '\u0b0c') || in_range(ch, '\u0b0f', '\u0b10') ||
        in_range(ch, '\u0b13', '\u0b28') || in_range(ch, '\u0b2a', '\u0b30') ||
        in_range(ch, '\u0b32', '\u0b33') || in_range(ch, '\u0b36', '\u0b39') ||
        in_range(ch, '\u0b5c', '\u0b5d') || in_range(ch, '\u0b5f', '\u0b61'))
    {
        return universal_char_type_valid;   // Oriya
    }

    if (in_range(ch, '\u0b85', '\u0b8a') || in_range(ch, '\u0b8e', '\u0b90') ||
        in_range(ch, '\u0b92', '\u0b95') || in_range(ch, '\u0b99', '\u0b9a') ||
        '\u0b9c' == ch || in_range(ch, '\u0b9e', '\u0b9f') ||
        in_range(ch, '\u0ba3', '\u0ba4') || in_range(ch, '\u0ba8', '\u0baa') ||
        in_range(ch, '\u0bae', '\u0bb5') || in_range(ch, '\u0bb7', '\u0bb9'))
    {
        return universal_char_type_valid;   // Tamil
    }

    if (in_range(ch, '\u0c05', '\u0c0c') || in_range(ch, '\u0c0e', '\u0c10') ||
        in_range(ch, '\u0c12', '\u0c28') || in_range(ch, '\u0c2a', '\u0c33') ||
        in_range(ch, '\u0c35', '\u0c39') || in_range(ch, '\u0c60', '\u0c61'))
    {
        return universal_char_type_valid;   // Telugu
    }

    if (in_range(ch, '\u0c85', '\u0c8c') || in_range(ch, '\u0c8e', '\u0c90') ||
        in_range(ch, '\u0c92', '\u0ca8') || in_range(ch, '\u0caa', '\u0cb3') ||
        in_range(ch, '\u0cb5', '\u0cb9') || in_range(ch, '\u0ce0', '\u0ce1'))
    {
        return universal_char_type_valid;   // Kannada
    }

    if (in_range(ch, '\u0d05', '\u0d0c') || in_range(ch, '\u0d0e', '\u0d10') ||
        in_range(ch, '\u0d12', '\u0d28') || in_range(ch, '\u0d2a', '\u0d39') ||
        in_range(ch, '\u0d60', '\u0d61'))
    {
        return universal_char_type_valid;   // Malayalam
    }

    if (in_range(ch, '\u0e01', '\u0e30') || in_range(ch, '\u0e32', '\u0e33') ||
        in_range(ch, '\u0e40', '\u0e46') || in_range(ch, '\u0e4f', '\u0e5b'))
    {
        return universal_char_type_valid;   // Thai
    }

    return universal_char_type_not_allowed;
}

///////////////////////////////////////////////////////////////////////////////
//
//  validate_identifier_name
//
//      The validate_identifier_name function tests a given identifier name for
//      its validity inregrads to contained universal characters. These should 
//      be in valid ranges (see the function classify_universal_char above).
//
//      If the identifier name contains invalid or not allowed universal 
//      characters a corresponding lexing_exception is thrown.
//
///////////////////////////////////////////////////////////////////////////////
inline void 
validate_identifier_name (std::string const &name, int line, int column, 
    std::string const &file_name)
{
std::string::size_type pos = name.find_first_of('\\');

    while (std::string::npos != pos) {
    // the identifier name contains a backslash (must be universal char)
        BOOST_SPIRIT_ASSERT('u' == name[pos+1] || 'U'== name[pos+1]);
        
    std::string uchar_val(name.substr(pos+2, ('u' == name[pos+1]) ? 4 : 8));
    universal_char_type type = 
        classify_universal_char(std::strtoul(uchar_val.c_str(), 0, 16));  
        
        if (type != universal_char_type_valid) {
        // an invalid char was found, so throw an exception
        std::string error_uchar(name.substr(pos, ('u' == name[pos+1]) ? 6 : 10));
        
            if (universal_char_type_invalid == type) {
                CPPLEXER_THROW(lexing_exception, universal_char_invalid, 
                    error_uchar, line, column, file_name.c_str());
            }
            else if (universal_char_type_base_charset == type) {
                CPPLEXER_THROW(lexing_exception, universal_char_base_charset, 
                    error_uchar, line, column, file_name.c_str());
            }
            else {
                CPPLEXER_THROW(lexing_exception, universal_char_not_allowed, 
                    error_uchar, line, column, file_name.c_str());
            }
        }
        
    // find next universal char (if appropriate)
        pos = name.find_first_of('\\', pos+2);
    }
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl           
}   // namespace cpplexer

#endif // !defined(_VALIDATE_UNIVERSAL_CHAR_HPP__55F1B811_CD76_4C72_8344_CBC69CF3B339__INCLUDED_)
