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

// This sample is taken from the C++ standard 16.3.5.6 [cpp.scope] and was 
// slightly modified (removed the '#include' directive)

#define str(s) # s
#define xstr(s) str(s)
#define debug(s, t) printf("x" # s "= %d, x" # t "= %s", \
x ## s, x ## t)
#define INCFILE(n) vers ## n /* from previous #include example */
#define glue(a, b) a ## b
#define xglue(a, b) glue(a, b)
#define HIGHLOW "hello"
#define LOW LOW ", world"
debug(1, 2);
fputs(str(strncmp("abc\0d?", "abc", '\4', "\u1234") /* this goes away */
== 0) str(: @\n), s);
/*#include*/ xstr(INCFILE(2).h)
glue(HIGH, LOW);
xglue(HIGH, LOW)

// should expand to 
//E printf("x"  "1" "= %d, x"  "2" "= %s", x1, x2);
//  printf("x1= %d, x2= %s", x1, x2);
//E fputs( "strncmp(\"abc\\0d\?\", \"abc\", '\\4', \"\\u1234\") == 0"  ": @\n", s);
//E   "vers2.h"
//E "hello";
//E "hello" ", world"
//R 0

