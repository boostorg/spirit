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

#define macro() expr_2
#define macro_2() expr

#define par() ()

#define expr macro ()
#define expr_2 macro_2 par par par()

#define scan(x) x

expr
//E macro_2 par par ()
scan(expr)
//E macro_2 par ()
scan(scan(expr))
//E macro_2 ()
scan(scan(scan(expr)))
//E macro_2 par par ()
scan(scan(scan(scan(expr))))
//E macro_2 par ()
scan(scan(scan(scan(scan(expr)))))
//E macro_2 ()
scan(scan(scan(scan(scan(scan(expr))))))
//E macro_2 ()
//R 0
