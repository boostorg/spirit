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
    // parser compiler
    ///////////////////////////////////////////////////////////////////////////
    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;

    typedef qi::rule<char const*> skipper_type;
    typedef qi::rule<char const*, utree(), skipper_type> rule_type;

    template <typename Rule>
    class rule_fragments
    {
    public:

        rule_fragments() {};

        template <typename Expr>
        boost::tuple<Rule const&, int>
        new_rule(Expr const& expr)
        {
            rules.push_back(Rule());
            rules.front() = expr;
            return boost::tuple<Rule const&, int>(
                rules.front(), rules.size()-1);
        }

        Rule const& get_rule(int id) const
        {
            return rules[i];
        }

    private:

        std::vector<Rule> rules;
    };

    struct primitive_parser_composite : composite<primitive_parser_composite>
    {
        int id;
        primitive_parser_composite(int id)
          : id(id)
        {
        }

        function compose(actor_list const& elements) const
        {
            return val(id);
        }
    };

    template <typename Fragments, typename Expr>
    inline primitive_parser_composite
    make_primitive_parser_composite(Fragments& fragments, Expr const& expr)
    {
        return primitive_parser_composite(
            boost::get<1>(fragments.new_rule(expr)));
    }

    template <typename Fragments>
    void build_qi_environment(Fragments& fragments, environment& env)
    {
        build_basic_environment(env);
        env.define("qi:space",
            make_primitive_parser_composite(fragments, qi::space), 0, true);
        env.define("qi:alpha",
            make_primitive_parser_composite(fragments, qi::alpha), 0, true);
        env.define("qi:int_",
            make_primitive_parser_composite(fragments, qi::int_), 0, true);
    }
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    using scheme::interpreter;
    using scheme::environment;
    using scheme::build_qi_environment;
    using scheme::rule_fragments;
    using scheme::rule_type;
    using scheme::utree;

    environment env;
    rule_fragments<rule_type> fragments;
    build_qi_environment(fragments, env);

    {
        utree src = "(define (factorial n) (if (<= n 0) 1 (* n (factorial (- n 1)))))";
        interpreter program(src, &env);
        BOOST_TEST(program["factorial"](10) == 3628800);
    }

    return boost::report_errors();
}


