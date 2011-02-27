/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "statement.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"

namespace client
{
    template <typename Iterator>
    statement<Iterator>::statement()
      : statement::base_type(start)
    {
        qi::_val_type _val;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;
        qi::raw_type raw;
        qi::lexeme_type lexeme;
        qi::alpha_type alpha;
        qi::alnum_type alnum;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;

        start =
            +(variable_declaration | assignment)
            ;

        identifier =
            raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

        variable_declaration =
                lexeme["var" >> !(alnum | '_')] // make sure we have whole words
            >   assignment
            ;

        assignment =
                identifier
            >   '='
            >   expr
            >   ';'
            ;

        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (start)
            (identifier)
            (variable_declaration)
            (assignment)
        );

        // Error handling: on error in start, call error_handler.
        on_error<fail>(start, error_handler(_4, _3, _2));

        // Annotation: on success in variable, call annotation.
        typedef client::annotation<Iterator> annotation_;
        typename function<annotation_>
            annotation = annotation_(expr.iters);
        on_success(assignment, annotation(_val, _3));
    }
}


