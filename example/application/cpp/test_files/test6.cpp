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

// Test, if wrongly placed '##' operators are detected

#define TEST1() A ## B
#define TEST2() ## A
#define TEST3() A ##

TEST1()
//E AB
TEST2()	    // error
//L (18): error: ill formed preprocessing operator: concat ('##')
TEST3()	    // error

//R 1
