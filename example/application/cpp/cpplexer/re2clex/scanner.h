/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001 Daniel C. Nuffer
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#ifndef SCANNER_H
#define SCANNER_H

#include "cpplexer/re2clex/aq.h"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace re2clex {

struct Scanner;
typedef unsigned char uchar;
typedef int (* ReportErrorProc)(struct Scanner *, char *, ...);

typedef struct Scanner {
    int    fd;  /* file descriptor */
    uchar* first;   /* start of input buffer (if fd == -1) */
    uchar* act;     /* act position of input buffer (if fd == -1) */
    uchar* last;    /* end (one past last char) of input buffer (if fd == -1) */
    uchar* bot; /* beginning of the current buffer */
    uchar* top; /* top of the current buffer */
    uchar* eof; /* when we read in the last buffer, will point 1 past the 
                   end of the file, otherwise 0 */
    uchar* tok; /* points to the beginning of the current token */
    uchar* ptr; /* used for YYMARKER - saves backtracking info */
    uchar* cur; /* saves the cursor (maybe is redundant with tok?) */
    uchar* lim; /* used for YYLIMIT - points to the end of the buffer */
                /* (lim == top) except for the last buffer, it points to
                   the end of the input (lim == eof - 1) */
    unsigned int line; /* current line being lexed */
    ReportErrorProc error_proc;     /* if != 0 this function is called to 
                report an error */
    char const *file_name;  /* name of the lexed file */
    aq_queue eol_offsets;
} Scanner;

int scan(Scanner *s);

///////////////////////////////////////////////////////////////////////////////
}   // namespace re2clex
}   // namespace cpplexer

#endif // SCANNER_H

