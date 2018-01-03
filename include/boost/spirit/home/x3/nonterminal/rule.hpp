/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#include <boost/spirit/home/x3/nonterminal/detail/rule.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
#include <typeinfo>
#endif

namespace boost { namespace spirit { namespace x3
{
    // default parse_rule implementation
    template <typename ID, typename Attribute, typename Iterator
      , typename Context, typename ActualAttribute>
    inline detail::default_parse_rule_result
    parse_rule(
        rule<ID, Attribute> /* rule_ */
      , Iterator& first, Iterator const& last
      , Context const& context, ActualAttribute& attr)
    {
        static_assert(!is_same<decltype(get<ID>(context)), unused_type>::value,
            "BOOST_SPIRIT_DEFINE undefined for this rule.");
        return get<ID>(context).parse(first, last, context, unused, attr);
    }

    template <typename ID, typename RHS, typename Attribute, bool force_attribute_>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, force_attribute_>>
    {
        typedef rule_definition<ID, RHS, Attribute, force_attribute_> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef rule<ID, Attribute> lhs_type;
        typedef Attribute attribute_type;

        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const force_attribute =
            force_attribute_;

        rule_definition(RHS const& rhs, char const* name)
          : rhs(rhs), name(name) {}

        /**@brief
         *  *only* called from BOOST_SPIRIT_DEFINE_; hence,
         *  the rule_attr_transform_f has already been done in the
         *  rule<...>::parse function; hence, no need to repeat
         *  it here.
         */
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse_no_xform(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        {
            return detail::rule_parser<attribute_type, ID>
                ::call_rule_definition(
                    rhs, name, first, last
                  , context
                  , attr
                  , mpl::bool_<force_attribute>());
        }
        
        /**@brief
         *  *this appears in rhs of some grammar expression; hence,
         *  this parse function is *not* called from the rule::parse function
         *  (i.e. indirectly through the parse_rule function generated
         *  by the BOOST_SPIRIT_DEFINE_ macro); 
         *  hence, rule_attr_transform_f must be called to avoid the error
         *  reported here:
         *    https://sourceforge.net/p/spirit/mailman/message/36093142/
         *  *and* also solve the link problem mentioned elsewhere in
         *  that same post:
         *    https://stackoverflow.com/questions/43791079/x3-linker-error-with-separate-tu
         */
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        {
            auto parser_f=[&]( Iterator& f_first, Iterator const& f_last, auto& f_attr)
              {  return  this->parse_no_xform( f_first, f_last, context, unused, f_attr);
              };
            bool ok_parse=
              detail::rule_parser<Attribute,ID>::rule_attr_transform_f(
                name
              , first, last
              , attr
              , parser_f);
            return ok_parse;
        }

        RHS rhs;
        char const* name;
    };

    template <typename ID, typename Attribute, bool force_attribute_>
    struct rule : parser<rule<ID, Attribute>>
    {
        typedef ID id;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const force_attribute = force_attribute_;

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
        rule() : name(typeid(rule).name()) {}
#else
        rule() : name("unnamed") {}
#endif

        rule(char const* name)
          : name(name) {}

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, force_attribute_>
        operator=(RHS const& rhs) const
        {
            return { as_parser(rhs), name };
        }

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, true>
        operator%=(RHS const& rhs) const
        {
            return { as_parser(rhs), name };
        }


        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        {
            auto parser_f=[&]( Iterator& f_first, Iterator const& f_last, auto& f_attr)
              {  return  parse_rule(*this, f_first, f_last, context, f_attr);
              };
            bool ok_parse=
              detail::rule_parser<Attribute,ID>::rule_attr_transform_f(
                name
              , first, last
              , attr
              , parser_f);
            return ok_parse;
        }

        char const* name;
    };

    namespace traits
    {
        template <typename T, typename Enable = void>
        struct is_rule : mpl::false_ {};

        template <typename ID, typename Attribute>
        struct is_rule<rule<ID, Attribute>> : mpl::true_ {};

        template <typename ID, typename Attribute, typename RHS, bool force_attribute>
        struct is_rule<rule_definition<ID, RHS, Attribute, force_attribute>> : mpl::true_ {};
    }

    template <typename T>
    struct get_info<T, typename enable_if<traits::is_rule<T>>::type>
    {
        typedef std::string result_type;
        std::string operator()(T const& r) const
        {
            BOOST_ASSERT_MSG(r.name, "uninitialized rule"); // static initialization order fiasco
            return r.name? r.name : "uninitialized";
        }
    };

/** @defgroup BOOST_SPIRIT_NS_RECUR BOOST_SPIRIT_* macros
 *
 *  Macros for connecting a rule in namespace scope
 *  with its rule_definition.
 *  @{
 */    

/*!
  \def BOOST_SPIRIT_DECLARE_(r, data, rule_type)
    \a r is ignored.
    \a data is ignored.
    \a rule_type is a type, rule<ID,RuleAttribute,...>.
    
    This generates a *declaration* of a parse_rule function
    specialized on rule_type as 1st arg. 
    The companion macro, BOOST_SPIRIT_INSTANTIATE, actually
    instantiates this declaration.  The INSTANTIATE macro should be
    called within a separate .cpp file which then allows separate
    compilation of the instantiation from the declaration; thereby,
    saving some overall compilation time (in theory ;).
    **IN ADDITION* the BOOST_SPIRIT_DEFINE_ macro should be called
    in the same .cpp file to provide the actual definition.
*/
#define BOOST_SPIRIT_DECLARE_(r, data, rule_type)                               \
    template <typename Iterator, typename Context, typename Attribute>          \
    bool parse_rule(                                                            \
        rule_type rule_                                                         \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, Attribute& attr);                               \
    /***/

#define BOOST_SPIRIT_DECLARE(...) BOOST_PP_SEQ_FOR_EACH(                        \
    BOOST_SPIRIT_DECLARE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))            \
    /***/

/*!
  \def BOOST_SPIRIT_DEFINE_(r, data, rule_name)
    \a r is ignored.
    \a data is ignored.
    \a rule_name is the variable name of a variable
       with type, rule<ID,RuleAttribute,...>,
       for some typename's ID and RuleAttriute.
       
    This generates a *definition* of a parse_rule function
    specialized on decltype(rule_name) as 1st arg.
    This specialized parse_rule will then be the one
    called in the rule<...>::parse function above. 
    The body creates a static instance of rule_name##_def.  
    Of course this  means rule_name##_def must be defined 
    before this macro is executed.  For example, given:
      rule_name = x
    then, something like:
      auto const& x_def = x = rhs;
    must occur in the scope in which this macro is invoked.
    Then, this macro is called as:
      BOOST_SPIRIT_DEFINE_(_,_,x)
      
    Using this obviates the need for calling the combination of
    BOOST_SPIRIT_DECLARE_ and BOOST_SPIRIT_INSTANTIATE.
*/
#if BOOST_WORKAROUND(BOOST_MSVC, < 1910)
#define BOOST_SPIRIT_DEFINE_(r, data, rule_name)                                \
    using BOOST_PP_CAT(rule_name, _synonym) = decltype(rule_name);              \
    template <typename Iterator, typename Context, typename Attribute>          \
    inline bool parse_rule(                                                     \
        BOOST_PP_CAT(rule_name, _synonym) /* rule_ */                           \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, Attribute& attr)                                \
    {                                                                           \
        using boost::spirit::x3::unused;                                        \
        static auto const def_ = (rule_name = BOOST_PP_CAT(rule_name, _def));   \
        return def_.parse_no_xform(first, last, context, unused, attr);         \
    }                                                                           \
    /***/
#else
#define BOOST_SPIRIT_DEFINE_(r, data, rule_name)                                \
    template <typename Iterator, typename Context, typename Attribute>          \
    inline bool parse_rule(                                                     \
        decltype(rule_name) /* rule_ */                                         \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, Attribute& attr)                                \
    {                                                                           \
        using boost::spirit::x3::unused;                                        \
        static auto const def_ = (rule_name = BOOST_PP_CAT(rule_name, _def));   \
        return def_.parse_no_xform(first, last, context, unused, attr);         \
    }                                                                           \
    /***/
#endif

#define BOOST_SPIRIT_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(                         \
    BOOST_SPIRIT_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    /***/

/*!
  \def BOOST_SPIRIT_INSTANTIATE(rule_type, Iterator, Context)
    Instantiates the parse_rule function specialization
    declared by the BOOST_SPIRIT_DECLARE_ macro.
    
   \design DESIGN_QUESTION:2017-11-04:
      Why shouldn't the BOOST_SPIRIT_DECLARE_ and BOOST_SPIRIT_DEFINE_ macros
      hardcode rule_type::attribute_type also as the type of the attr argument
      instead of making it a template parameter?
 */    
#define BOOST_SPIRIT_INSTANTIATE(rule_type, Iterator, Context)                  \
    template bool parse_rule<Iterator, Context, rule_type::attribute_type>(     \
        rule_type rule_                                                         \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, rule_type::attribute_type& attr);               \
    /***/

/** @}*/    

}}}

#endif
