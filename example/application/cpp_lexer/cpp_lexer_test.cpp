//
// C++ Lexer implemented with Spirit (http://spirit.sourceforge.net/)
//
// Copyright© 2002 Juan Carlos Arevalo-Baeza, All rights reserved
// email: jcab@JCABs-Rumblings.com
// Created: 8-Nov-2002
//

#include "cpp_lexer.hpp"

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/functor_parser.hpp>

#include <boost/phoenix/primitives.hpp>
#include <boost/phoenix/composite.hpp>
#include <boost/phoenix/functions.hpp>
#include <boost/phoenix/operators.hpp>
#include <boost/phoenix/special_ops.hpp>
#include <boost/phoenix/binders.hpp>
#include <boost/phoenix/statements.hpp>

///////////////////////////////////////////////////////////////////////////////
// used namespaces
using namespace boost::spirit;

using phoenix::var;

///////////////////////////////////////////////////////////////////////////////

struct get_file_position_parser {
    boost::spirit::file_position& filePos;
    get_file_position_parser(boost::spirit::file_position& filePos_):
        filePos(filePos_)
    {}
    typedef nil_t result_t;
    template < typename ScannerT >
    int operator()(ScannerT const& scan, result_t& result) const {
        if (scan.at_end()) {
            return -1;
        }
        filePos = scan.first->filePos;
        return 0;
    }
};

boost::spirit::functor_parser<get_file_position_parser>
get_file_position_p(boost::spirit::file_position& filePos)
{
    return get_file_position_parser(filePos);
}

///////////////////////////////////////////////////////////////////////////////

struct test_grammar: grammar<test_grammar> {
    template < typename ScannerT >
    struct definition {
        typedef rule<ScannerT> rule_t;
        rule_t main;

        rule_t const& start() const {
            return main;
        }

        boost::spirit::file_position filePos;

        definition(test_grammar const& self) {
            main =
                *(
                    ch_p(Kwd_using)[var(std::cout) << "   using   "]
                  | ch_p(EOL_token) >> get_file_position_p(filePos)[var(std::cout) << "\n" << var(filePos)]
                  | anychar_p[OutToken][var(std::cout) << " "]
                );
        }
    };
};

///////////////////////////////////////////////////////////////////////////////
// main entrypoint
int
main(int argc, char* argv[])
{
    // Start grammar definition
    std::cerr << "C++ Lexer implemented with Spirit ..." << std::endl;

    char const* rule = "";
    char const* fname = "";

    // main driver code
    if (3 == argc) {
        std::cerr << "Using rule " << argv[2] << std::endl;
        fname = argv[1];
        rule = argv[2];
    } else if (2 == argc) {
        fname = argv[1];
    } else {
        std::cerr << "No filename given" << std::endl;
        fname = "cpp_lexer.cpp";
        std::cerr << "Parsing " << fname << std::endl;
    }

    FILE* f = fopen(fname, "rb");
    if (!f) {
        std::cerr << "Cannot open input file: " << fname << std::endl;
    }
    fseek(f, 0, SEEK_END);
    int const size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = new char[size];
    fread(buf, 1, size, f);
    fclose(f);

    std::cout << "File size: " << size << " bytes\n";

    Token token;

    test_grammar grammar;

    lex_iterator first(buf, buf+size, fname);
    lex_iterator last;

    parse_info<lex_iterator> result =
        parse(
            first, last,
            grammar,
            ch_p(Comment_token)
        )
    ;

    if (result.full) {
        std::cerr << "\n" << fname << " Parses OK" << std::endl;
    } else {
        std::cerr << "\n" << fname << " Fails Parsing" << std::endl;

        std::cerr << "These tokens follow:\n";

        for (int i = 0; i < 10; i++)
        {
            if (result.stop == last) {
                break;
            }
            PrintToken(*result.stop++);
        }
    }

    return 0;
}
