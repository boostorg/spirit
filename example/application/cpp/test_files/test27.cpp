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

// Test, if additional whitespace is inserteted at appropriate places.

#define STRINGIZE(x) STRINGIZE_D(x)
#define STRINGIZE_D(x) #x

#define X() 1
#define PLUS() +
#define MINUS() -
#define DOT() .

X()2
//E 1 2
STRINGIZE( X()2 )
//E "1 2"

X() 2
//E 1 2
STRINGIZE( X() 2 )
//E "1 2"

PLUS()MINUS()
//E +-
STRINGIZE( PLUS()MINUS() )
//E "+-"

PLUS()PLUS()
//E + +
//  not ++
STRINGIZE( PLUS()PLUS() )
//E "+ +"
//  not "++"

MINUS()MINUS()
//E - -
//  not --
STRINGIZE( MINUS()MINUS() )
//E "- -"
//  not "--"

DOT()DOT()DOT()
//E .. .
//  not ...
STRINGIZE( DOT()DOT()DOT() )
//E ".. ."
//  not "..."

//R 0

