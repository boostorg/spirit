//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QIEXPR_GENERATOR)
#define BOOST_SPIRIT_QIEXPR_GENERATOR

#include <string>

#include <boost/cstdint.hpp>
#include <boost/spirit/include/karma.hpp>

#include <utree/utree.hpp>
#include <utree/detail/utree_detail3.hpp>
#include <utree/operators.hpp>
#include <qi/component_names.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct symbols_lookup<scheme::utree, std::string>
    {
        typedef std::string type;
        static type call(scheme::utree const& t)
        {
            BOOST_ASSERT(t.which() == scheme::utree_type::list_type);
            return boost::get<scheme::utf8_symbol>(t.front());
        }
    };

    template <>
    struct symbols_value<scheme::utree, std::string>
    {
        typedef unused_type type;
        static type call(scheme::utree const& t)
        {
            return unused;
        }
    };
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
            using boost::spirit::karma::eps;

            start = term.alias();

            term = 
//                     primitives2 << '(' << string << ',' << string << ')'
//                 |   primitives1 << '(' << string << ')'
                /*|*/   primitive0
                ;

            // fill the symbol tables with all known primitive parser names
            for (char const* const* p = primitives0; *p; ++p)
                primitive0.add(*p);

            for (char const* const* p = primitives1; *p; ++p)
                primitive1.add(*p);

            for (char const* const* p = primitives2; *p; ++p)
                primitive2.add(*p);
        }

        typedef rule<OutputIterator, space_type, utree()> rule_type;

        rule_type start, term;
        symbols<std::string> primitive0, primitive1, primitive2;
    };
}}

#endif



