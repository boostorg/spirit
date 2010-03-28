//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>

#define BOOST_SPIRIT_UNICODE

#include <iostream>
#include <fstream>
#include <iterator>

#include "input/sexpr.hpp"
#include "output/sexpr.hpp"

namespace client
{
    bool parse_sexpr_from_file(char const* filename, scheme::utree& result)
    {
        std::ifstream in(filename, std::ios_base::in);

        if (!in)
        {
            std::cerr << "Error: Could not open input file: "
                << filename << std::endl;
            exit(-1);
        }

        // Ignore the BOM marking the beginning of a UTF-8 file in Windows
        char c = in.peek();
        if (c == '\xef')
        {
            char s[3];
            in >> s[0] >> s[1] >> s[2];
            s[3] = '\0';
            if (s != std::string("\xef\xbb\xbf"))
            {
                std::cerr << "Error: Unexpected characters from input file: "
                    << filename << std::endl;
                exit(-1);
            }
        }

        std::string source_code;      // We will read the contents here.
        in.unsetf(std::ios::skipws);  // No white space skipping!
        std::copy(
            std::istream_iterator<char>(in),
            std::istream_iterator<char>(),
            std::back_inserter(source_code));

        typedef std::string::const_iterator iterator_type;
        iterator_type first(source_code.begin());
        iterator_type last(source_code.end());

        scheme::input::sexpr<iterator_type> p;
        scheme::input::white_space<iterator_type> ws;

        return phrase_parse(first, last, p, ws, result);
    }

    bool generate_sexpr_to_file(scheme::utree const& tree, char const* filename)
    {
        std::ofstream out(filename);

        if (!out)
        {
            std::cerr << "Error: Could not open output file: "
                << filename << std::endl;
            exit(-1);
        }

        typedef std::back_insert_iterator<std::string> output_iterator_type;
        using boost::spirit::karma::space;

        std::string output;
        output_iterator_type sink(output);
        scheme::output::sexpr<output_iterator_type> g;
        if (!generate_delimited(sink, g, space, tree))
        {
            return false;
        }

        out << output;
        return true;
    }
}

int main(int argc, char **argv)
{
    char const* filename_in;
    if (argc > 1)
    {
        filename_in = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return -1;
    }

    char const* filename_out;
    if (argc > 2)
    {
        filename_out = argv[2];
    }
    else
    {
        std::cerr << "Error: No output file provided." << std::endl;
        return -1;
    }

    scheme::utree result;
    if (client::parse_sexpr_from_file(filename_in, result))
    {
        if (client::generate_sexpr_to_file(result, filename_out))
        {
            std::cout << "success!" << std::endl;
        }
        else
        { 
            std::cout << "generate error" << std::endl;
        }
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}
