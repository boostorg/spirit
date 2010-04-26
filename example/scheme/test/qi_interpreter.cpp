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

#include "../../../test/qi/test.hpp"

namespace scheme { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    // parser compiler
    ///////////////////////////////////////////////////////////////////////////
    namespace qi = boost::spirit::qi;
    namespace spirit = boost::spirit;

    typedef qi::rule<char const*> skipper_type;
    typedef qi::rule<char const*, utree(), skipper_type> rule_type;

    ///////////////////////////////////////////////////////////////////////////
    // All rule are stored here. Rules are held in the utree by its id;
    // i.e. it's index in the vector.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Rule>
    class rule_fragments
    {
    public:

        rule_fragments() {};

        template <typename Expr>
        int new_rule(Expr const& expr)
        {
            rules.push_back(Rule());
            rules.back() = expr;
            return rules.size()-1;
        }

        Rule const& get_rule(int id) const
        {
            return rules[id];
        }

        Rule const& get_rule(utree const& id) const
        {
            return rules[id.get<int>()];
        }

    private:

        std::vector<Rule> rules;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Composes primitive parsers held by index. Handles the compilation of
    // primitive (nullary) parsers such as int_, double_, alpha, space and all
    // those that require no arguments.
    ///////////////////////////////////////////////////////////////////////////
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
        return primitive_parser_composite(fragments.new_rule(expr));
    }

    ///////////////////////////////////////////////////////////////////////////
    // Handles the compilation of char_
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    struct char_function : actor<char_function<Fragments> >
    {
        Fragments& fragments;
        function a;
        function b;
        char_function(
            Fragments& fragments, function const& a, function const& b)
          : a(a), b(b), fragments(fragments)
        {
        }

        utree eval(utree const& a) const
        {
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a.which() == utree_type::string_type);

            utf8_string_range a_ = a.get<utf8_string_range>();
            if (a_.size() == 1)
            {
                // char_('x')
                return fragments.new_rule(qi::char_(a_[0]));
            }
            else
            {
                // char_("some-regex")
                return fragments.new_rule(
                    qi::char_(std::string(a_.begin(), a_.end())));
            }
        }

        utree eval(utree const& a, utree const& b) const
        {
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a.which() == utree_type::string_type);
            BOOST_ASSERT(b.which() == utree_type::string_type);

            utf8_string_range a_ = a.get<utf8_string_range>();
            utf8_string_range b_ = b.get<utf8_string_range>();
            // $$$ use exceptions here $$$.
            BOOST_ASSERT(a_.size() == 1);
            BOOST_ASSERT(b_.size() == 1);

            // char_('x', 'y')
            return fragments.new_rule(qi::char_(a_[0], b_[0]));
        }

        utree eval(scope const& env) const
        {
            if (b.empty())
                return eval(a(env));
            else
                return eval(a(env), b(env));
        }
    };

    template <typename Fragments>
    struct unary_char_composite
      : composite<unary_char_composite<Fragments> >
    {
        Fragments& fragments;
        unary_char_composite(Fragments& fragments)
          : fragments(fragments) {}

        function compose(actor_list const& elements) const
        {
            typedef char_function<Fragments> function_type;
            actor_list::const_iterator i = elements.begin();

            function empty;
            function const& a = *i++;
            function const& b = (i == elements.end())? empty : *i;
            return function(function_type(fragments, a, b));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Build our scheme compiler environment.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Fragments>
    void build_environment(Fragments& fragments, environment& env)
    {
        build_basic_environment(env);

        env.define("qi:space",
            make_primitive_parser_composite(fragments, qi::space), 0, true);

        env.define("qi:alpha",
            make_primitive_parser_composite(fragments, qi::alpha), 0, true);

        env.define("qi:int_",
            make_primitive_parser_composite(fragments, qi::int_), 0, true);

        env.define("qi:char_",
            unary_char_composite<Fragments>(fragments), 1, false);
    }
}}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    using scheme::utree;
    using scheme::interpreter;
    using scheme::environment;
    using scheme::qi::build_environment;
    using scheme::qi::rule_fragments;
    using scheme::qi::rule_type;
    using spirit_test::test;

    environment env;
    rule_fragments<rule_type> fragments;
    build_environment(fragments, env);

    scheme::qi::skipper_type space = boost::spirit::qi::space;

    {
        utree src =
            "(define r (qi:char_ \"x\"))"
            "(define r2 (qi:int_))";
        interpreter parser(src, "parse.scm", &env);

        BOOST_TEST(test("x", fragments.get_rule(parser["r"]()), space));
        BOOST_TEST(!test("y", fragments.get_rule(parser["r"]()), space));
        BOOST_TEST(test("1234", fragments.get_rule(parser["r2"]()), space));
        BOOST_TEST(!test("x1234", fragments.get_rule(parser["r2"]()), space));


    }

    return boost::report_errors();
}


