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

// Tests the #warning directive (note: only available, if enabled during
// compilation, the macro expansion is available only, when enabled separately
// during the compilation too)

#define WARNING1 This is a 
#define WARNING2 warning
#warning WARNING1 WARNING2
//L (21): warning: encountered #warning directive: This is a warning
//R 1
