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

// Tests, if universal character values to generated accidently by 
// concatenation

#define CONCAT(a, b) PRIMITIVE_CONCAT(a, b)
#define PRIMITIVE_CONCAT(a, b) a ## b
#define STRINGIZE(x) STRINGIZE_D(x)
#define STRINGIZE_D(x) # x

STRINGIZE( CONCAT(\, a) )	// expands to "\a"
STRINGIZE( CONCAT(\, u00ff) )	// expands to "\u00ff"
STRINGIZE( CONCAT(\u00, ff) )	// expands to "\ u00ff"
STRINGIZE( CONCAT(\u00ff, 56) )	// expands to "\u00ff56"
CONCAT(\, a)	        // expands to \a
CONCAT(\, u00ff)        // expands to \u00ff
CONCAT(\u00, ff)        // expands to \ u00ff
CONCAT(\u00ff, 56)      // expands to \u00ff56

STRINGIZE( CONCAT(\, u0061) )	// reports an error
