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

#define macro() end
#define par() ()

#define expr macro par par par()

#define scan(x) x

expr
//E macro par par ()
scan(expr)
//E macro par ()
scan(scan(expr))
//E macro ()
scan(scan(scan(expr)))
//E end
//R 0
