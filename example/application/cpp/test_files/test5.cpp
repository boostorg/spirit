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

// The follwoing is a parametized partially expanding concatenation macro.
// It is a extremly good test of expansion order and the order of operations 
// during macro expansion in general. 

#define CAT(a, b) a ## b

#define PARTIAL_CAT(i) CAT(PARTIAL_CAT_, i)

#define PARTIAL_CAT_0(a, b) CAT(a, b)
#define PARTIAL_CAT_1(a, b) CAT(PARTIAL ## a, b)
#define PARTIAL_CAT_2(a, b) CAT(a, b ## PARTIAL)
#define PARTIAL_CAT_3(a, b) CAT(PARTIAL ## a, b ## PARTIAL)

#define PARTIAL
#define PARTIALPARTIAL

#define X Token1
#define Y Token2

PARTIAL_CAT(0)( PARTIAL X, Y PARTIAL )
//E Token1Token2
PARTIAL_CAT(1)( PARTIAL X, Y PARTIAL )
//E  XToken2
PARTIAL_CAT(2)( PARTIAL X, Y PARTIAL )
//E Token1Y 
PARTIAL_CAT(3)( PARTIAL X, Y PARTIAL )
//E  XY 
//R 0
