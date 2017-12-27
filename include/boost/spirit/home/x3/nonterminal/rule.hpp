/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#ifndef BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS 1
  //^Disable storage of rule_definition<ID,...> in context with ID key.
  //Instead, only use BOOST_SPIRIT_DEFINE to connect the
  //rule with it's rhs (i.e. rule_definition) using overloads
  //of parse_rule.
#endif
#ifndef BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP 1
  //^Like BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS, but instead
  //of using BOOST_SPIRIT_DEFINE, use BOOST_SPIRIT_CRTP_DEFINE
  //with the crtp design pattern:
  //  https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
  //to connect the rule with it's rhs.
#endif
#define BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT (BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS||BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP)
  //^Disables putting rule_definition in context.
#if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS
#else
  //#pragma message "deprecated.  May cause excessive compile times with many rule_definition's on rhs."
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS
#ifndef BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE 1
#endif  
#if BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
  //#pragma message "ATTR_XFORM_IN_RULE."
#else
  //#pragma message "deprecated.  May cause link error when using BOOST_SPIRIT_INSTANTIATE."
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE

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
  #if !BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
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

  #else
    /*!
     * @brief
     *   Only used to avoid firing the static_assert in
     *   default parse_rule below when the
     *   BOOST_SPIRIT_DEFINE *has* been called to define the rule.
     */
    template <typename ID>
    struct always_false
    {
        static constexpr bool value=false;
    };
    
    template <typename ID, typename Attribute, typename Iterator
      , typename Context, typename ActualAttribute>
    inline detail::default_parse_rule_result
    parse_rule(
        rule<ID, Attribute> rule_
      , Iterator& first, Iterator const& last
      , Context const& context, ActualAttribute& attr)
    {
        static_assert
          ( always_false<ID>::value
            //Using just false in the above causes assert to
            //always fire.  Using the above template
            //metafunction call avoids that.
          , "Undefined rule.  Use BOOST_SPIRIT_DEFINE to define rule."
          );
        return false;
    }
    struct rule_undefined
    /**@brief
     *  Tag value indicating a rule has not been defined.
     */
    {
        rule_undefined(){}
        
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        /*! \brief 
         *    Prevent redundant compiletime error in default get_rhs
         *    when static_assert fires.
         */
        {
            assert(false);
            return false;
        }
    };
    
    template<typename ID>
    struct get_id
    /*! \brief
     *  Thin wrapper about ID to handle case where
     *  ID is not defined.
     *  Used to overload operator for get_rhs function below.
     */
    {};
    
  #ifdef BOOST_SPIRIT_X3_EXPERIMENTAL_DEFAULT_NS_GET_RHS
    //TODO:
    //  Placeholder for future modifications.
    //  The purpose of this is to allow similar logic
    //  to be used both for BOOST_SPIRIT_DEFINE*
    //  and BOOST_SPIRIT_CRTP_DEFINE* code.
    //  IOW, instead of the parse_rule function, 
    //  defined above, use get_rhs function, as
    //  shown in the gram_base<GramDeriv> template
    //  below.
    //
    
    //default get_rhs.
    template< typename ID>
    auto const& get_rhs(get_id<ID>)
    {
        static_assert
          ( always_false<ID>::value
            //Using just false in the above causes assert to
            //always fire.  Using the above template
            //metafunction call avoids that.
          , "Undefined rule. Use BOOST_SPIRIT_DEFINE to define rule."
          );
        static rule_undefined const result;
        return result;
    }
  #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_DEFAULT_NS_GET_RHS
  #endif//!BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS
    template <typename ID, typename RHS, typename Attribute, bool force_attribute_>
    struct rule_definition 
      : parser<rule_definition<ID, RHS, Attribute, force_attribute_>>
    {
        typedef rule_definition<ID, RHS, Attribute, force_attribute_> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef Attribute attribute_type;

        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const force_attribute =
            force_attribute_;

        rule_definition(RHS const& rhs, char const* name)
          : rhs(rhs), name(name) {}

      #if BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse_no_xform(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        /**@brief
         *  *only* called from BOOST_SPIRIT_DEFINE_; hence,
         *  the attribute transform has already been done in the
         *  rule<...>::parse function; hence, no need to repeat
         *  it here.
         */
        {
            return detail::rule_parser<attribute_type, ID>
                ::call_rule_definition(
                    rhs, name, first, last
                  , context
                  , attr
                  , mpl::bool_<force_attribute>());
        }
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type rcontext, ActualAttribute& attr) const
        /**@brief
         *  *this appears in rhs of some grammar expression; hence,
         *  this parse function is *not* called from the rule::parse function
         *  (indirectly throught the parse_rule function generated
         *  by the BOOST_SPIRIT_DEFINE_ macro); 
         *  hence, rule_attr_transform_f must be called to avoid the error
         *  reported here:
         *    https://sourceforge.net/p/spirit/mailman/message/36093142/
         *  *and* also solve the link problem mentioned elsewhere in
         *  that same post:
         *    https://stackoverflow.com/questions/43791079/x3-linker-error-with-separate-tu
         */
        {
            auto parser_f=[&]
              ( Iterator& f_first, Iterator const& f_last
              , auto&_attr
              )
              {  return  this->parse_no_xform( f_first, f_last, context, rcontext, _attr);
              };
            bool ok_parse=
              detail::rule_parser<Attribute,ID>::rule_attr_transform_f
              ( name
              , first, last
              , attr
              , parser_f
              );
            return ok_parse;
        }
      #else
        template <typename Iterator, typename Context, typename ActualAttribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, ActualAttribute& attr) const
        {
            return detail::rule_parser<attribute_type, ID>
                ::call_rule_definition(
                    rhs, name, first, last
                  , context
                  , attr
                  , mpl::bool_<force_attribute>());
        }
      #endif

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
          : name(name) 
          {
          }
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
          #if BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
            auto parser_f=[&]
              ( Iterator& f_first, Iterator const& f_last
              , auto&_attr
              )
              {  return  parse_rule(*this, f_first, f_last, context, _attr);
              };
            bool ok_parse=
              detail::rule_parser<Attribute,ID>::rule_attr_transform_f
              ( name
              , first, last
              , attr
              , parser_f
              );
          #else
            bool ok_parse = parse_rule(*this, first, last, context, attr);
          #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
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
 *  Macros for connecting the lhs with rhs of a production
 *  in namespace scope.
 *  @{
 */    

/*!
  \def BOOST_SPIRIT_DEFINE_(r, data, rule_type)
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
#if BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_RDEF_PARSE parse_no_xform
#else
  #define BOOST_SPIRIT_X3_EXPERIMENTAL_RDEF_PARSE parse
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE  
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
        return def_.                                                            \
          BOOST_SPIRIT_X3_EXPERIMENTAL_RDEF_PARSE(first, last, context, unused, attr);  \
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
        return def_.                                                            \
          BOOST_SPIRIT_X3_EXPERIMENTAL_RDEF_PARSE(first, last, context, unused, attr);  \
    }                                                                           \
    /***/
#endif
//#undef BOOST_SPIRIT_X3_EXPERIMENTAL_RDEF_PARSE
//MAINTENANCE_NOTE:2017-12-06:
//  ^can't do the above #undef because it causes compile failure
//   in the test/x3/rule1.cpp.
//

/*!
  \def BOOST_SPIRIT_DEFINE(r1,r2,...rn)
    call BOOST_SPIRIT_DEFINE_(_,_,r) for each r in r1,r2,...rn.
*/    
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
    
#if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
    
      template
      < typename GramDeriv
      >      
    struct gram_base
      /**Base class for CRTP pattern.
       * Derived class actually defines the grammar
       * and contains instances of the nested rule_b's
       * and supplies the get_rhs member function,
       * preferably by using the BOOST_SPIRIT_CRTP_*
       * macros below.
       */
      {
        template <typename ID, typename Attribute=unused_type>
        struct rule_b : parser<rule_b<ID, Attribute>>
          {
            typedef ID id;
            typedef Attribute attribute_type;
            static bool const has_attribute =
                !is_same<Attribute, unused_type>::value;
            static bool const handles_container =
                traits::is_container<Attribute>::value;
            static bool const force_attribute = false;
        
            constexpr rule_b() : name("unnamed") {}
            constexpr rule_b(char const* name)
              : name(name) 
              {}
            
            template <typename RHS>
            rule_definition<
              ID, typename extension::as_parser<RHS>::value_type, Attribute, force_attribute>
            operator=(RHS const& rhs) const
            {
              return { as_parser(rhs), name };
            }
    
              template 
              < typename Iterator
              , typename Context
              , typename ActualAttribute
              >
              bool 
            parse
              ( Iterator& first
              , Iterator last
              , Context const& ctx
              , unused_type
              , ActualAttribute& attr
              ) const
              {
                  auto const& 
                def=GramDeriv().get_rhs(get_id<ID>{})
                  //Retrieve the rhs of this rule_b from a
                  //GramDeriv which contains an instance
                  //of this rule_b.
                  ;
              #if BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
                auto parser_f=[&]
                  ( Iterator& f_first, Iterator const& f_last
                  , auto&_attr
                  )
                  {  return  def.parse( f_first, f_last, ctx, unused, _attr);
                  };
                bool ok_parse=
                  detail::rule_parser<Attribute,ID>::rule_attr_transform_f
                  ( name
                  , first, last
                  , attr
                  , parser_f
                  );
              #else
                bool ok_parse=def.parse(first, last, ctx, unused, attr);
              #endif
                return ok_parse;
              }
            
            char const* name;
          };

        //default get_rhs
        //
        //(should cause compile time failure with *slighly*
        //more informative error msg).
        //To be useful, there should be a using declaration:
        //  using gram_base<GramDerived>::get_rhs;
        //in GramDerived.
        template< typename ID>
        auto const& get_rhs(get_id<ID> id)
          {
            static_assert
              ( always_false<ID>::value
                //Using just false in the above causes assert to
                //always fire.  Using the above template
                //metafunction call avoids that.
              , "Undefined rule. Use BOOST_SPIRIT_CRTP_DEFINE to define rule."
              );
            static rule_undefined const result;
            return result;
          }
        
        template<typename ID> 
        struct rule_declaration_crtp 
          : GramDeriv 
            //^Avoids compiler diagnostic about undeclared identifiers
            //when BOOST_SPIRIT_CRTP_INSTANTIATE macro is used.  The
            //undeclared identifiers are found in the 
            //rule_name and rule_value args to that macro and are
            //instance variables of GramDeriv; hence, the use
            //of GramDeriv as superclass brings those variables
            //in scope.
          { 
              template<typename Iterator, typename Context, typename Attribute> 
              bool 
            parse(Iterator&first, Iterator last, Context const&, unused_type, Attribute&)const 
              //Definition provided with help of
              //BOOST_SPIRIT_CRTP_INSTANTIATE.
              ; 
          }; 
      };//gram_base<GramDeriv> struct
      
/** @defgroup BOOST_SPIRIT_CRTP_RECUR BOOST_SPIRIT_CRTP* macros
 *
 *  Macros for connecting the lhs with rhs of a production
 *  in a class derived from gram_base using the
 *  CRTP design pattern.
 *
 *  The BOOST_SPIRIT_CRTP_* macros perform a similar function to the
 *  corresponding the BOOST_SPIRIT_* macros.
 *  @{
 */    
 
/*!
  \def BOOST_SPIRIT_CRTP_DEFINE_(r, scope, rule_name)
    \a r is ignored.
    \a scope is some typename argument to gram_base template class.
    \a rule_name is the variable name of a variable
       with type, gram_base<scope>::rule_b<ID,RuleAttribute,...>,
       for some typename's, ID and RuleAttriute.
       
    This generates a *definition* of a get_rhs(ID) function, which
    returns a static instance of rule_declaration_crtp<ID> defined in
    gram_base but with only a *declared* parse function.  This declared
    rule_declaration_crtp<ID>::parse function should be instantiated 
    using the companion BOOST_SPIRIT_CRTP_INSTANTIATE macro (see below).
*/ 
#define BOOST_SPIRIT_CRTP_DECLARE_(r, scope, rule_name)                   \
    inline auto get_rhs                                                   \
    ( ::boost::spirit::x3::get_id<typename decltype(rule_name)::id>)      \
    { using rule_id=typename decltype(rule_name)::id;                     \
        static const gram_base<scope>::template                           \
      rule_declaration_crtp<rule_id> def;                                 \
      return def;                                                         \
    }                                                                     \
    /***/
#define BOOST_SPIRIT_CRTP_DEFINE_(r, data, rule_def)                     \
    inline auto get_rhs                                                  \
    ( ::boost::spirit::x3::get_id<typename decltype(rule_def)::id>)      \
    {                                                                    \
        static auto const def(rule_def);                                 \
        return def;                                                      \
    }                                                                    \
    /***/
    
#define BOOST_SPIRIT_CRTP_DECLARE(scope,...) BOOST_PP_SEQ_FOR_EACH(            \
    BOOST_SPIRIT_CRTP_DECLARE_, scope, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))  \
    /***/
#define BOOST_SPIRIT_CRTP_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(             \
    BOOST_SPIRIT_CRTP_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    /***/
      
/*!
  \def BOOST_SPIRIT_CRTP_INSTANTIATE(rule_name,rule_value,scope,Iterator,Context,Attribute)
    \a rule_name variable declared as rule_b<ID,Attribute>.
    \a rule_value expression for value of rule_name.
    \a scope is class where rule_name is declared.
    \a Iterator concrete type for iterator arg to parse function.
    \a Context concrete type for context arg to parse function.
    \a Attribute concrete type for attribute arg to parse function.
       
    This instantiates the rule_declaraion_crtp::parse function used by the
    get_rhs function generated by the BOOST_SPIRIT_CRTP_DECLARE macro.
*/ 
#define BOOST_SPIRIT_CRTP_INSTANTIATE(rule_name,rule_value,scope,Iterator,Context,Attribute) \
template<>/*gram_base*/\
  template<>/*rule_declaration_crtp*/\
    template<>/*parse*/\
    bool \
gram_base<scope>::\
  rule_declaration_crtp<typename decltype(scope::rule_name)::id>::\
    parse(Iterator&first, Iterator last, Context context, unused_type \
      , Attribute attr)const \
    { static auto const def=(rule_name=(rule_value)); \
      return def.parse(first, last, context, unused, attr); \
    } \
  /***/
/** @}*/
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP

}}}

#endif
