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

// Tests #line functionality with out a given file name

#line 5 

#error This error should occur at line 6 of "test20.cpp"
//L (6): fatal error: encountered #error directive: This error should occur at line 6 of "test20.cpp"
//R 1
