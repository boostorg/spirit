#!/bin/sh
SRCDIR=`dirname $0`
./test_re2c_lexer $SRCDIR/lextest.in | diff - $SRCDIR/lextest.re2c.output || exit 1
./test_slex_lexer $SRCDIR/lextest.in | diff - $SRCDIR/lextest.slex.output || exit 1
