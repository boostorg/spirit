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

// Tests the handling of placeholder tokens, which have to be inserted, when
// some macro expands into nothing (certainly these have to be ignored 
// afterwards :-)

#define NIL

#define A B NIL
#define B() anything

A()
// expands to B (), not 'anything'
//E B ()
//R 0
