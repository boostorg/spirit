/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    Copyright (c) 2001-2009 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <string>
#include <vector>

using namespace boost::spirit;
using namespace boost::spirit::ascii;

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;

using phoenix::at_c;

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

BOOST_FUSION_ADAPT_STRUCT(
    mini_xml,
    (std::string, name)
    (std::vector<mini_xml_node>, children)
)

template <typename OutputIterator>
struct mini_xml_generator
  : karma::grammar<OutputIterator, mini_xml()>
{
    mini_xml_generator() : mini_xml_generator::base_type(xml)
    {
        xml = 
//                string[_1 = at_c<0>(_val)]    // this compiles
               string(at_c<0>(_val))    // this doesn't compile
            ;
    }

    karma::rule<OutputIterator, mini_xml()> xml;
};

int main(int argc, char **argv)
{
    mini_xml ast;

    typedef std::back_insert_iterator<std::string> outiter_type;
    typedef mini_xml_generator<outiter_type> mini_xml_generator;

    mini_xml_generator xmlout;

    std::string generated;
    outiter_type outit(generated);
    bool r = karma::generate(outit, xmlout, ast);

    return 0;
}


