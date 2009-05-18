/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_RULE_FEBRUARY_12_2007_1020AM)
#define BOOST_SPIRIT_RULE_FEBRUARY_12_2007_1020AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/function.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/is_function.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/as_list.hpp>
#include <boost/fusion/include/as_vector.hpp>

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/argument.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/nonterminal/locals.hpp>
#include <boost/spirit/home/qi/reference.hpp>
#include <boost/spirit/home/qi/nonterminal/detail/parser_binder.hpp>

namespace boost { namespace spirit { namespace qi
{
    BOOST_PP_REPEAT(SPIRIT_ATTRIBUTES_LIMIT, SPIRIT_USING_ATTRIBUTE, _)

    using spirit::_val;
    using spirit::_a;
    using spirit::_b;
    using spirit::_c;
    using spirit::_d;
    using spirit::_e;
    using spirit::_f;
    using spirit::_g;
    using spirit::_h;
    using spirit::_i;
    using spirit::_j;

    using spirit::info;
    using spirit::locals;

    template <typename Rule, typename Params>
    struct parameterized_rule : parser<parameterized_rule<Rule, Params> >
    {
        parameterized_rule(Rule const& rule, Params const& params)
          : ref(rule), params(params) {}

        template <typename Context, typename Iterator>
        struct attribute : Rule::template attribute<Context, Iterator> {};

        template <typename Parser, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Parser& first, Parser const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            // We pass the additional params argument to parse
            return ref.get().parse(first, last, context, skipper, attr, params);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return ref.get().what(context);
        }

        boost::reference_wrapper<Rule const> ref;
        Params params;
    };

    template <
        typename Iterator
      , typename T1 = unused_type
      , typename T2 = unused_type
      , typename T3 = unused_type
    >
    struct rule
      : proto::extends<
            typename proto::terminal<
                reference<rule<Iterator, T1, T2, T3> const>
            >::type
          , rule<Iterator, T1, T2, T3>
        >
      , parser<rule<Iterator, T1, T2, T3> >
    {
        typedef rule<Iterator, T1, T2, T3> this_type;
        typedef reference<this_type const> reference_;
        typedef typename proto::terminal<reference_>::type terminal;
        typedef proto::extends<terminal, this_type> base_type;
        typedef mpl::vector<T1, T2, T3> template_params;

        // locals_type is a sequence of types to be used as local variables
        typedef typename fusion::result_of::as_vector<
            typename detail::extract_param<
                    template_params
                  , spirit::detail::is_locals<mpl::_>
                  , locals<>
                >::type
            >::type
        locals_type;

        // The skip-parser type
        typedef typename
            result_of::compile<
                qi::domain
              , typename detail::extract_param<
                    template_params
                  , traits::matches<qi::domain, mpl::_>
                  , unused_type
                >::type
            >::type
        skipper_type;

        typedef typename
            detail::extract_param<
                template_params
              , function_types::is_function<mpl::_>
              , void()
            >::type
        sig_type;

        typedef typename function_types::result_type<sig_type>::type attr_type_;

        // This is the rule's attribute type
        typedef typename
            mpl::if_<
                is_same<attr_type_, void>
              , unused_type
              , attr_type_
            >::type
        attr_type;
        typedef typename add_reference<attr_type>::type attr_reference_type;

        // parameter_types is a sequence of types passed as parameters to the rule
        typedef typename
            function_types::parameter_types<sig_type>::type
        params_;

        typedef typename
            fusion::result_of::as_list<params_>::type
        parameter_types;

        static size_t const params_size = mpl::size<params_>::value;

        typedef context<
            fusion::cons<attr_reference_type, parameter_types>
          , locals_type>
        context_type;

        typedef function<
            bool(Iterator& first, Iterator const& last
              , context_type& context
              , skipper_type const& skipper
            )>
        function_type;

        rule(std::string const& name_ = "unnamed-rule")
          : base_type(terminal::make(alias()))
          , name_(name_)
        {
        }

        rule(rule const& rhs)
          : base_type(terminal::make(alias()))
          , name_(rhs.name_)
          , f(rhs.f)
        {
        }

        rule& operator=(rule const& rhs)
        {
            // The following assertion fires  when you try to initialize a rule
            // from an uninitialized one. Did you mean to refer to the right
            // hand side rule instead of assigning from it? In this case you 
            // should write lhs = rhs.alias();
            BOOST_ASSERT(rhs.f);

            f = rhs.f;
            name_ = rhs.name_;
            return *this;
        }

        std::string const& name() const
        {
            return name_;
        }

        void name(std::string const& str)
        {
            name_ = str;
        }

        template <typename Expr>
        rule& operator=(Expr const& expr)
        {
            // Report invalid expression error as early as possible.
            // If you got an error_invalid_expression error message here,
            // then the expression (expr) is not a valid spirit qi expression.
            BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr)

            f = detail::bind_parser<mpl::false_>(compile<qi::domain>(expr));
            return *this;
        }

        // g++ 3.3 barfs if this is a member function :(
        template <typename Expr>
        friend rule& operator%=(rule& r, Expr const& expr)
        {
            // Report invalid expression error as early as possible.
            // If you got an error_invalid_expression error message here,
            // then the expression (expr) is not a valid spirit qi expression.
            BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr)

            r.f = detail::bind_parser<mpl::true_>(compile<qi::domain>(expr));
            return r;
        }

        // non-const version needed to suppress proto's %= kicking in
        template <typename Expr>
        friend rule& operator%=(rule& r, Expr& expr)
        {
            return r %= static_cast<Expr const&>(expr);
        }

        template <typename Context, typename Iterator_>
        struct attribute
        {
            typedef attr_type type;
        };

        template <typename Context, typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& /*context*/, Skipper const& skipper
          , Attribute& attr) const
        {
            if (f)
            {
                value_initialized<attr_type> val;
                context_type context(val);
                if (f(first, last, context, skipper))
                {
                    traits::swap_impl(attr, boost::get(val));
                    return true;
                }
            }
            return false;
        }

        template <typename Context, typename Skipper
          , typename Attribute, typename Params>
        bool parse(Iterator& first, Iterator const& last
          , Context& caller_context, Skipper const& skipper
          , Attribute& attr, Params const& params) const
        {
            if (f)
            {
                value_initialized<attr_type> val;
                context_type context(val, params, caller_context);
                if (f(first, last, context, skipper))
                {
                    traits::swap_impl(attr, boost::get(val));
                    return true;
                }
            }
            return false;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info(name_);
        }

        reference_ alias() const
        {
            return reference_(*this);
        }

        typename proto::terminal<this_type>::type copy() const
        {
            typename proto::terminal<this_type>::type result = {*this};
            return result;
        }

        // bring in the operator() overloads
        rule const& get_rule() const { return *this; }
        typedef this_type rule_type;
        #include <boost/spirit/home/qi/nonterminal/detail/fcall.hpp>

        std::string name_;
        function_type f;
    };
}}}

#endif
