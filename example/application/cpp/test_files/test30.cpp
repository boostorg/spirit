/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

// Tests, if no universal character values are to be generated accidently by 
// concatenation

#define CONCAT(a, b) PRIMITIVE_CONCAT(a, b)
#define PRIMITIVE_CONCAT(a, b) a ## b
#define STRINGIZE(x) STRINGIZE_D(x)
#define STRINGIZE_D(x) # x

STRINGIZE( CONCAT(\, a) )
//E  "\a"
STRINGIZE( CONCAT(\, u00ff) )
//E  "\u00ff"
STRINGIZE( CONCAT(\u00, ff) )
//E  "\ u00ff"
STRINGIZE( CONCAT(\u00ff, 56) )
//E  "\u00ff56"
CONCAT(\, a)
//E \a
CONCAT(\, u00ff)
//E \u00ff
CONCAT(\u00, ff)
//E \ u00ff
CONCAT(\u00ff, 56)
//E \u00ff56

STRINGIZE( CONCAT(\, u0061) )	// reports an error
//L (40): error: a universal character name cannot designate a character in the basic character set: \u0061

//R 2
