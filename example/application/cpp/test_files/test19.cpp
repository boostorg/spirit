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

// Tests simple #line functionality

#line 5 "a_nonexisting_file.cpp"

#error This error should occur at line 6 of "a_nonexisting_file.cpp"
//L (6): fatal error: encountered #error directive: This error should occur at line 6 of "a_nonexisting_file.cpp"
//R 1
