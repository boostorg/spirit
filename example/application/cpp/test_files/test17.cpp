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

// Tests continuing scanning into the underlying input stream after expanding
// a macro, if this is appropriate

#define MACRO() X + Y NEXT
#define X 1
#define Y 2
#define NEXT() ...

MACRO()()
//E 1 + 2 ...
//R 0
