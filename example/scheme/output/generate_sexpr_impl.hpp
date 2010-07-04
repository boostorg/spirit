//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SCHEME_OUTPUT_GENERATE_SEXPR_IMPL_MAR_29_2010_1210PM)
#define SCHEME_OUTPUT_GENERATE_SEXPR_MAR_IMPL_29_2010_1210PM

#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_list.hpp>
#include <boost/spirit/include/support_ostream_iterator.hpp>

#include "../output/sexpr.hpp"
#include "../output/generate_sexpr.hpp"

namespace scheme { namespace output
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputStream>
    bool generate_sexpr(OutputStream& os, utree const& tree)
    {
        typedef boost::spirit::ostream_iterator output_iterator_type;
        using boost::spirit::karma::space;

        output_iterator_type sink(os);

        scheme::output::sexpr<output_iterator_type> g;
        return generate_delimited(sink, g, space, tree);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputStream>
    bool generate_sexpr_list(OutputStream& os, utree const& tree)
    {
        typedef boost::spirit::ostream_iterator output_iterator_type;
        using boost::spirit::karma::space;
        using boost::spirit::karma::eol;

        output_iterator_type sink(os);

        scheme::output::sexpr<output_iterator_type> g;
        return generate_delimited(sink, g % eol, space, tree);
    }
}}

#endif


