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

// Tests the correctness of parameter replacement, if the parameter is
// adjacent to an operator '##'.

#define CONCAT(a, b) PRIMITIVE_CONCAT(a, b)
#define PRIMITIVE_CONCAT(a, b) a ## b

CONCAT(1, PRIMITIVE_CONCAT(2, 3))
//E 123
CONCAT(1, CONCAT(2, 3))
//E 123
PRIMITIVE_CONCAT(1, CONCAT(2, 3))
//E 1CONCAT(2, 3)
PRIMITIVE_CONCAT(1, PRIMITIVE_CONCAT(2, 3))
//E 1PRIMITIVE_CONCAT(2, 3)
//R 0
