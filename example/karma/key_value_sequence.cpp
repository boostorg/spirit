//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  The main purpose of this example is to show how a single container type can
//  be formatted using different output grammars. 

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_stream.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>
#include <map>
#include <algorithm>
#include <cstdlib> 

namespace client
{
    namespace spirit = boost::spirit;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    template <typename OutputIterator>
    bool key_value_sequence(OutputIterator sink
      , std::map<std::string, std::string> const& m)
    {
        using spirit::karma::generate;
        using spirit::karma::eps;
        using spirit::karma::omit;
        using spirit::karma::rule;
        using spirit::karma::_1;
        using spirit::karma::_val;
        using ascii::string;

        rule<OutputIterator, std::pair<std::string const, std::string>()> param;

        param %=  eps(!phoenix::empty(phoenix::at_c<1>(_val))) 
                    << string << "=" << string
              |   string << omit[string]
              ;

        return generate(sink, param % '&', m);
    }
}

///////////////////////////////////////////////////////////////////////////////
int main()
{
    typedef std::map<std::string, std::string>::value_type value_type;

    std::map<std::string, std::string> m;
    m.insert(value_type("key1", "value1"));
    m.insert(value_type("key2", ""));
    m.insert(value_type("key3", "value3"));
    m.insert(value_type("key4", ""));

    std::string generated;
    std::back_insert_iterator<std::string> sink(generated);
    if (!client::key_value_sequence(sink, m))
    {
        std::cout << "-------------------------\n";
        std::cout << "Generating failed\n";
        std::cout << "-------------------------\n";
    }
    else
    {
        std::cout << "-------------------------\n";
        std::cout << "Generated: " << generated << "\n";
        std::cout << "-------------------------\n";
    }
    return 0;
}

