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

// Tests, if macro expansion eats up follow up tokens under certain conditions
// (which it shouldn't).

BEGINNING OF THE FILE
//E BEGINNING OF THE FILE

#define SCAN(x) x

#define BUG BUG_2
#define BUG_2

SCAN(BUG) 1 2 3 4 5
//E  1 2 3 4 5

END OF THE FILE
//E END OF THE FILE

//R 0
