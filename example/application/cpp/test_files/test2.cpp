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

// Tests macro expansion order in conjunction with the need to skip expansion 
// of the same macro as it is currently expanded.

#define CONCAT(a, b) a ## b
#define CONCAT_INDIRECT() CONCAT

CONCAT(CON, CAT)(1, 2)
//E CONCAT(1, 2)
CONCAT(CON, CAT(1, 2))
//E CONCAT(1, 2)
CONCAT(1, CONCAT(2, 3))
//E 1CONCAT(2, 3)
CONCAT(1, CONCAT_INDIRECT()(2, 3))
//E 1CONCAT_INDIRECT()(2, 3)
CONCAT(CONCAT_, INDIRECT)()(1, 2)
//E 12
CONCAT(CONCAT_, INDIRECT())(1, 2)
//E CONCAT(1, 2)
//R 0
