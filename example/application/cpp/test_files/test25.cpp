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

// Tests predefined macros

"__STDC__":    __STDC__
"__cplusplus": __cplusplus
"__DATE__":    __DATE__
"__TIME__":    __TIME__
"__LINE__":    __LINE__
"__FILE__":    __FILE__
"__SPIRIT_PP__":             __SPIRIT_PP__
"__SPIRIT_PP_VERSION__":     __SPIRIT_PP_VERSION__
"__SPIRIT_PP_VERSION_STR__": __SPIRIT_PP_VERSION_STR__
#line 50 "test.cpp"
__LINE__
__FILE__


__LINE__
__FILE__

// The output from preprocessing this file is quite hard to
// guess for a shell script. Therefore, it is currently not
// tested.

//Eignore
//R 0
