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

// Tests, if a diagnostic is emitted, if a predefined macro is to be undefined.

#undef __cplusplus     // should emit a warning
// FIXME:
//L (17): warning: #undef may not be used on this predefined name: __cplusplus
//R 1
