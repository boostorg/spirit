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

// Tests correctness of macro expansion inside #pragma directives

#define PRAGMA_BODY	preprocessed pragma body

#pragma some pragma body
#pragma PRAGMA_BODY
#pragma STDC some C99 standard pragma body
#pragma STDC PRAGMA_BODY

