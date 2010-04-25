/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/config/warning_disable.hpp>

#include <input/sexpr.hpp>
#include <input/parse_sexpr_impl.hpp>
#include <scheme/compiler.hpp>
#include <utree/io.hpp>
#include <boost/spirit/include/qi.hpp>

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // parser
    ///////////////////////////////////////////////////////////////////////////
    using boost::spirit::unused;
    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;

    typedef boost::iterator_range<char const*> source_type;

    template <typename Parser, bool no_attr>
    struct qi_parser_function
      : actor<qi_parser_function<Parser, no_attr> >
    {
        Parser p;
        qi_parser_function(Parser const& p)
          : p(p) {}

        utree eval(scope const& env) const
        {
            // the source input
            source_type rng = env[0].get<source_type>();
            char const* iter = rng.begin();

            // the skipper
            utree const& skipper = env[1];

            // do a pre-skip
            while (skipper.eval(env) == true)
                ;

            // the attribute
            utree& attribute = env[2];
            bool r = false;
            if (r = p.parse(iter, rng.end(), unused, unused, attribute))
                env[0] = source_type(iter, rng.end());

            return r;
        }
    };

    template <typename Parser>
    struct qi_parser_function<Parser, true> // unused attribute
      : actor<qi_parser_function<Parser, true> >
    {
        Parser p;
        qi_parser_function(Parser const& p)
          : p(p) {}

        utree eval(scope const& env) const
        {
            // the source input
            source_type rng = env[0].get<source_type>();
            char const* iter = rng.begin();

            // the skipper
            utree const& skipper = env[1];

            // do a pre-skip
            while (skipper.eval(env) == true)
                ;

            bool r = false;
            if (r = p.parse(iter, rng.end(), unused, unused, unused))
                env[0] = source_type(iter, rng.end());

            return r;
        }
    };

    struct qi_parser
    {
        typedef function result_type;

        template <typename Expression>
        function operator()(Expression const& expr) const
        {
            typedef typename
                boost::spirit::result_of::compile<
                    qi::domain, Expression>::type
            parser_type;

            typedef boost::is_same<
                typename spirit::traits::attribute_of<parser_type>::type,
                spirit::unused_type>
            is_unused;

            return function(qi_parser_function<parser_type, is_unused::value>(
                qi::compile<qi::domain>(expr)));
        }
    };

    qi_parser const primitive_parser = {};
    function const space = primitive_parser(qi::space);
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    using scheme::interpreter;
    using scheme::utree;

    utree src = "(define (factorial n) (if (<= n 0) 1 (* n (factorial (- n 1)))))";
    scheme::interpreter program(src);
    BOOST_TEST(program["factorial"](10) == 3628800);

    return boost::report_errors();
}


