/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    Copyright (c) 2003 Paul Mensonides
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/


#define CAT(a, b) a ## b
#define ARGS (1, 2)

CAT ARGS            // expands to CAT (1, 2) not 12

#define INVOKE(macro) macro ARGS

INVOKE(CAT)         // CAT (1, 2) not 12

#define EXPAND(x) x

EXPAND(CAT ARGS)    // expands to 12 because of rescanning
