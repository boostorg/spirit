#!/bin/sh
SRCDIR=`dirname $0`
./test_lexer $SRCDIR/lextest.in | diff - $SRCDIR/lextest.output || exit 1
./test_lexer $SRCDIR/test_lexer.in | diff - $SRCDIR/test_lexer.output || exit 1
