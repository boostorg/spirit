/* -*- mode: c -*-
 * $Id$
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdio.h>
#include <stdlib.h>
*/

unsigned long
fib(unsigned long n) {
    return( (n < 2) ? 1 : (fib(n-2) + fib(n-1)) );
}

int
main(int argc, char *argv[]) {
    int N = ((argc == 2) ? atoi(argv[1]) : 1);
    printf("%ld\n", fib(N));
    return(0);
}
