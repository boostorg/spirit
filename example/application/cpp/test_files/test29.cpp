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

// Tests, if function-like macros buried deep inside a macro expansion of the
// same name as an object-like macro do not eat up more tokens, than expected.

#define PRIMITIVE_CAT(a, b) a ## b

#define EAT(n) PRIMITIVE_CAT(EAT_, n)
#define EAT_1(a)

EAT(1)...
//E EAT_1...
//R 0
