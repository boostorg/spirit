/*=============================================================================
Copyright (c) 2016 Frank Hein, maxence business consulting gmbh

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <map>

#include <boost/spirit/home/qi.hpp>

namespace qi = boost::spirit::qi;

using iterator_type = std::string::const_iterator;
using result_type = std::string;

template<typename Parser>
void parse(const std::string message, const std::string& input, const std::string& rule, const Parser& parser) {
    iterator_type iter = input.begin(), end = input.end();

    std::vector<result_type> parsed_result;

    std::cout << "-------------------------\n";
    std::cout << message << "\n";
    std::cout << "Rule: " << rule << std::endl;
    std::cout << "Parsing: \"" << input << "\"\n";

    bool result = qi::phrase_parse(iter, end, parser, qi::space, parsed_result);
    if (result)
    {
        std::cout << "Parser succeeded.\n";
        std::cout << "Parsed " << parsed_result.size() << " elements:";
        for (const auto& str : parsed_result)
            std::cout << "[" << str << "]";
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Parser failed" << std::endl;
    }
    if (iter == end) {
        std::cout << "EOI reached." << std::endl;
    }
    else {
        std::cout << "EOI not reached. Unparsed: \"" << std::string(iter, end) << "\"" << std::endl;
    }
    std::cout << "-------------------------\n";

}

int main() {

     qi::rule < iterator_type, std::string(), qi::space_type>
        id = (qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_'));

    parse("Modulus Operator (%), list with several different 'delimiters' (not omitted) "
        , "item1, item2, item3; item 4"
        , "id  % qi::char_(\";,\"))"
        , id % qi::char_(";,"));

    parse("Modulus Operator (%), list with several different 'delimiters'\n Omitting the 'delimiters'."
        , "item1, item2, item3; item 4"
        , "id % (qi::lit(',') | qi::lit(';'))"
        , id % (qi::lit(',') | qi::lit(';')));


    return 0;
}