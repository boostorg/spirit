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

// Test if macros at not used parameter positions are expanded correctly

#define MACRO() /**/
#define ID(x)           // parameter not used at all
#define CAT(x) X ## x   // expanded parameter not used

ID( MACRO(*) )  // error: MACRO takes no arguments (too many arguments)
//L (21): warning: too many macro arguments
CAT( MACRO(*) ) // error: MACRO takes no arguments (too many arguments)

//R 1
