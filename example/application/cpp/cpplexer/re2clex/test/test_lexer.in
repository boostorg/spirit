#include <fcntl.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#else
#include <memory.h>
#include <io.h>
#endif 
#include <stdio.h>

#include "../tokens.h"
#include "../tokennames.cpp"

#include "scanner.h"

int main(int argc, char** argv) {
    Scanner in;
    int t;
    memset((char*) &in, 0, sizeof(in));
    in.fd = -1;
    if (argc == 2)
    {
        in.fd = open(argv[1], O_RDONLY);
    }
    
    if (-1 == in.fd)
    {
        printf ("Usage: test_lexer <input>\n");
        exit(1);
    }
    in.eol_offsets = aq_create();
    while((t = scan(&in)) != T_EOF)
    {
        printf("%4d %16s Line: %5d >%.*s<\n", ID_FROM_TOKEN(t-T_FIRST_TOKEN), 
            tok_names[ID_FROM_TOKEN(t-T_FIRST_TOKEN)], in.line + 1, 
            in.cur - in.tok, in.tok);
    }
    aq_terminate(in.eol_offsets);
    free(in.bot);
    close(in.fd);

    return 0;
}
