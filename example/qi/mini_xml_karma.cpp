/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman
    Copyright (c) 2001-2007 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A mini XML-like parser, Karma is used to print out the generated AST
//
//  [ JDG March 25, 2007 ]   spirit2
//  [ HK April 02, 2007  ]   spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/function_output_iterator.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::karma;
using namespace boost::spirit::ascii;
using namespace boost::spirit::arg_names;

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;

using phoenix::at_c;
using phoenix::push_back;

///////////////////////////////////////////////////////////////////////////////
//  Our mini XML tree representation
///////////////////////////////////////////////////////////////////////////////
struct mini_xml;

typedef
    boost::variant<
        boost::recursive_wrapper<mini_xml>
      , std::string
    >
mini_xml_node;

struct mini_xml
{
    std::string name;                           // tag name
    std::vector<mini_xml_node> children;        // children
};

// We need to tell fusion about our mini_xml struct
// to make it a first-class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
    mini_xml,
    (std::string, name)
    (std::vector<mini_xml_node>, children)
)

///////////////////////////////////////////////////////////////////////////////
//  Our mini XML grammar definition
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct mini_xml_parser :
    qi::grammar_def<Iterator, mini_xml(), space_type>
{
    mini_xml_parser()
    {
        text = lexeme[+(char_ - '<')        [text.val += _1]];
        node = (xml | text)                 [node.val = _1];

        start_tag =
                '<'
            >>  lexeme[+(char_ - '>')       [start_tag.val += _1]]
            >>  '>'
        ;

        end_tag =
                "</"
            >>  lit(end_tag._1)
            >>  '>'
        ;

        xml =
                start_tag                   [at_c<0>(xml.val) = _1]
            >>  *node                       [push_back(at_c<1>(xml.val), _1)]
            >>  end_tag(at_c<0>(xml.val))
        ;
    }

    qi::rule<Iterator, mini_xml(), space_type> xml;
    qi::rule<Iterator, mini_xml_node(), space_type> node;
    qi::rule<Iterator, std::string(), space_type> text;
    qi::rule<Iterator, std::string(), space_type> start_tag;
    qi::rule<Iterator, void(std::string), space_type> end_tag;
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
template <typename String>
struct string_appender
{
    string_appender(String& s)
    :   str(s)
    {}

    template <typename T>
    void operator()(T const &x) const
    {
        str += x;
    }

    String& str;
};

template <typename String>
inline string_appender<String>
make_string_appender(String& str)
{
    return string_appender<String>(str);
}

template <typename Char>
struct output_iterator
{
    typedef std::basic_string<Char> string_type;
    typedef string_appender<string_type> appender_type;
    typedef boost::function_output_iterator<appender_type> type;
    
    static type 
    call(std::basic_string<Char>& str)
    {
        return boost::make_function_output_iterator(
            make_string_appender(str));
    }
};

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
template <typename OutputIterator>
struct mini_xml_generator
  : boost::spirit::karma::grammar_def<OutputIterator, void(mini_xml), space_type>
{
// typedef karma::grammar_def<OutputIterator, void(mini_xml), space_type> base_type;
// boost::mpl::print<typename base_type::start_type::param_types> x;

    mini_xml_generator()
    {
//             text = verbatim[lit(text._1)];
//             node = (xml | text)                 [_1 = node._1];
//
//             start_tag =
//                     '<'
//                 <<  verbatim[lit(start_tag._1)]
//                 <<  '>'
//             ;
//
//             end_tag =
//                     "</"
//                 <<  verbatim[lit(end_tag._1)]
//                 <<  '>'
//             ;
//
//         xml =
//                 start_tag(at_c<0>(xml._1))
//                 <<  (*node)                     [ref(at_c<1>(xml._1))]
//             <<  end_tag(at_c<0>(xml._1))
        ;
    }

    karma::rule<OutputIterator, void(mini_xml), space_type> xml;
//     karma::rule<OutputIterator, void(mini_xml_node), space_type> node;
//     karma::rule<OutputIterator, void(std::string), space_type> text;
//     karma::rule<OutputIterator, void(std::string), space_type> start_tag;
//     karma::rule<OutputIterator, void(std::string), space_type> end_tag;
};

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef mini_xml_parser<std::string::const_iterator> mini_xml_parser;
    mini_xml_parser def;  //  Our grammar definition
    qi::grammar<mini_xml_parser> xmlin(def, def.xml); // Our grammar
    mini_xml ast; // our tree

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = phrase_parse(iter, end, xmlin, ast, space);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";

        typedef output_iterator<char>::type outiter_type;
        typedef mini_xml_generator<outiter_type> mini_xml_generator;

        mini_xml_generator gen;                     //  Our grammar definition
        karma::grammar<mini_xml_generator> xmlout(gen, gen.xml); // Our grammar

        std::string generated;
        bool r = generate_delimited(output_iterator<char>::call(generated), 
                    xmlout(ast), space);

        if (r)
            std::cout << generated << std::endl;
        return 0;
    }
    else
    {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << context << "...\"\n";
        std::cout << "-------------------------\n";
        return 1;
    }
}


