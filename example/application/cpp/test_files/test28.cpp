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

// Test if invalid or not allowed universal characters are rejected

#define \u00ff ...

\u00ff
//E ...

#define \u0061 weird // 0x61 == 'a'

\u0061             // should emit an error
//L (22): error: a universal character name cannot designate a character in the basic character set: \u0061

//R 2
