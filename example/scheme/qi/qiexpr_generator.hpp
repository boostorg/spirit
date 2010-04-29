//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QIEXPR_GENERATOR)
#define BOOST_SPIRIT_QIEXPR_GENERATOR

#include <string>

#include <boost/cstdint.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <utree/utree.hpp>
#include <utree/operators.hpp>
#include <output/utree_traits.hpp>
#include <qi/component_names.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    template <typename Out>
    void print_attribute(Out& out, scheme::utree const& val);
}}}

///////////////////////////////////////////////////////////////////////////////
namespace scheme { namespace qi
{
    using boost::spirit::karma::grammar;
    using boost::spirit::karma::rule;
    using boost::spirit::karma::space_type;
    using boost::spirit::karma::symbols;

    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator>
    struct qiexpr_generator : grammar<OutputIterator, space_type, utree()>
    {
        qiexpr_generator() : qiexpr_generator::base_type(start)
        {
            namespace phoenix = boost::phoenix;

            using boost::spirit::karma::eps;
            using boost::spirit::karma::ascii::string;
            using boost::spirit::karma::omit;
            using boost::spirit::karma::repeat;
            using boost::spirit::karma::_r1;
            using boost::spirit::karma::strict;
            using boost::spirit::karma::eol;

            start = 
                    nil 
                |   rule_ % eol
                |   rule_
                ;

            rule_ =
                    &symbol(std::string("define")) 
                        << repeat(1)[rule_name] << '=' << repeat(1)[alternative]
                |   alternative
                ;

            alternative = 
                    &symbol(std::string("qi:|")) 
                        << '(' << strict[permutation % '|'] << ')'
                |   permutation
                ;

            permutation = 
                    &symbol(std::string("qi:^")) 
                        << '(' << strict[sequence % '^'] << ')'
                |   sequence
                ;

            sequence = 
                    &symbol(std::string("qi:>>")) 
                        << '(' << strict[term % ">>"] << ')'
                |   term
                ;

            term = strict[
                        unary << '(' << repeat(1)[alternative] << ')'
                    |   primitive2 << '(' << literal << ',' << literal << ')'
                    |   primitive1 << '(' << literal << ')'
                    |   primitive0_rule
                    |   directive0 << '[' << repeat(1)[alternative] << ']'
                    |   alternative_rule 
                ];

            primitive0_rule = strict[repeat(1)[primitive0]];
            alternative_rule = alternative;

            rule_name = strict[repeat(1)[any_symbol]];

            any_symbol = string;
            symbol = string(_r1);
            literal = '"' << string << '"';
            nil = eps;

            // fill the symbol tables with all known primitive parser names
            std::string name("qi:");
            for (char const* const* p = primitives0; *p; ++p)
                primitive0.add(utf8_symbol(name + *p));

            for (char const* const* p = primitives1; *p; ++p)
                primitive1.add(utf8_symbol(name + *p));

            for (char const* const* p = primitives2; *p; ++p)
                primitive2.add(utf8_symbol(name + *p));

            for (char const* const* p = unary_names; *p; ++p)
                unary.add(utf8_symbol(name + *p));

            for (char const* const* p = directives0; *p; ++p)
                directive0.add(utf8_symbol(name + *p));

            BOOST_SPIRIT_DEBUG_NODE(start);
            BOOST_SPIRIT_DEBUG_NODE(rule_);
            BOOST_SPIRIT_DEBUG_NODE(rule_name);
            BOOST_SPIRIT_DEBUG_NODE(alternative);
            BOOST_SPIRIT_DEBUG_NODE(permutation);
            BOOST_SPIRIT_DEBUG_NODE(sequence);
            BOOST_SPIRIT_DEBUG_NODE(term);
            BOOST_SPIRIT_DEBUG_NODE(nil);
            BOOST_SPIRIT_DEBUG_NODE(literal);
            BOOST_SPIRIT_DEBUG_NODE(symbol);
            BOOST_SPIRIT_DEBUG_NODE(any_symbol);
            BOOST_SPIRIT_DEBUG_NODE(primitive0_rule);
            BOOST_SPIRIT_DEBUG_NODE(alternative_rule);
        }

        typedef rule<OutputIterator, space_type, utree()> delimiting_rule_type;

        delimiting_rule_type start, alternative, permutation, sequence, term;
        delimiting_rule_type rule_, rule_name, primitive0_rule, alternative_rule;
        rule<OutputIterator, nil()> nil;
        rule<OutputIterator, scheme::utf8_string()> literal;
        rule<OutputIterator, scheme::utf8_symbol(std::string)> symbol;
        rule<OutputIterator, scheme::utf8_symbol()> any_symbol;

        symbols<scheme::utf8_symbol> unary, directive0;
        symbols<scheme::utf8_symbol> primitive0, primitive1, primitive2;
    };
}}

#endif



