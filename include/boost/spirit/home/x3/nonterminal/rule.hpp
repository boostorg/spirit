/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#ifndef BOOST_SPIRIT_GET_RHS_CRTP
  #define BOOST_SPIRIT_GET_RHS_CRTP 1
#endif
#ifndef BOOST_SPIRIT_ATTR_XFORM_IN_RULE
  #define BOOST_SPIRIT_ATTR_XFORM_IN_RULE 1
#endif  
#if BOOST_SPIRIT_ATTR_XFORM_IN_RULE
#else
  #pragma message "deprecated.  May cause link error when using BOOST_SPIRIT_INSTANTIATE."
#endif//BOOST_SPIRIT_ATTR_XFORM_IN_RULE
#if BOOST_SPIRIT_GET_RHS_CRTP
#else
  #pragma message "deprecated.  May cause excessive compile times when many rules."
#endif//BOOST_SPIRIT_ATTR_XFORM_IN_RULE

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
  #if !BOOST_SPIRIT_GET_RHS_CRTP
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
    bool
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
        template <typename Iterator>
        bool parse(Iterator& first, Iterator last) const
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
     *  ID is not define.
     *  Used to overload operator for get_rhs function below.
     */
    {};
    
    //default get_rhs.
    template< typename ID>
    auto const& get_rhs(get_id<ID>)
    {
        static_assert
          ( always_false<ID>::value
            //Using just false in the above causes assert to
            //always fire.  Using the above template
            //metafunction call avoids that.
          , "Undefined rule.  Use BOOST_SPIRIT_DEFINE to define rule."
          );
        static rule_undefined const result;
        return result;
    }
  #endif//!BOOST_SPIRIT_GET_RHS_CRTP
    template <typename ID, typename RHS, typename Attribute, bool force_attribute_>
    struct rule_definition 
    #if 1 || !BOOST_SPIRIT_GET_RHS_CRTP
      : parser<rule_definition<ID, RHS, Attribute, force_attribute_>>
    #else
      //Don't allow rule_definition on rhs of rule definition.
      //Instead, must use rule itself.
    #endif
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
          #if BOOST_SPIRIT_ATTR_XFORM_IN_RULE
            using rat_t=typename detail::rule_parser<Attribute,ID>
                    ::template rule_attr_transform<ActualAttribute>;
            rat_t rat_v(attr);
            auto attr_ = rat_v.pre();
            
            bool ok_parse
              //Creates a place to hold the result of parse_rhs
              //called inside the following scope.
              ;
            {
             // Create a scope to cause the dbg variable below (within
             // the #if...#endif) to call it's DTOR before any
             // modifications are made to the attribute, attr_ passed
             // to parse_rhs (such as might be done in
             // traits::post_transform when, for example,
             // ActualAttribute is a recursive variant).
#if defined(BOOST_SPIRIT_X3_DEBUG)
                char const* rule_name=name;
                if(!rule_name) 
                { //why does this happen?  
                  rule_name="***unknown***";
                }
                using make_attribute=traits::make_attribute<Attribute,ActualAttribute>;
                typedef typename make_attribute::type dbg_attribute_type;
                detail::context_debug<Iterator, dbg_attribute_type>
                dbg(rule_name, first, last, dbg_attribute_type(attr_), ok_parse);
#endif
                ok_parse = parse_rule(*this, first, last, context, attr_);
            }
            rat_v.post(ok_parse,attr);
          #else
            bool ok_parse = parse_rule(*this, first, last, context, attr);
          #endif//BOOST_SPIRIT_ATTR_XFORM_IN_RULE
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

#define BOOST_SPIRIT_DEFINE_(r, data, rule_name)                                \
    template <typename Iterator, typename Context, typename Attribute>          \
    inline bool parse_rule(                                                     \
        decltype(rule_name) /* rule_ */                                         \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, Attribute& attr)                                \
    {                                                                           \
        using boost::spirit::x3::unused;                                        \
        static auto const def_ = (rule_name = BOOST_PP_CAT(rule_name, _def));   \
        return def_.parse(first, last, context, unused, attr);                  \
    }                                                                           \
    /***/

#define BOOST_SPIRIT_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(                         \
    BOOST_SPIRIT_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    /***/

#define BOOST_SPIRIT_INSTANTIATE(rule_type, Iterator, Context)                  \
    template bool parse_rule<Iterator, Context, rule_type::attribute_type>(     \
        rule_type rule_                                                         \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, rule_type::attribute_type& attr);               \
    /***/
    
#if BOOST_SPIRIT_GET_RHS_CRTP

      template
      < typename GramDeriv
      >      
    struct gram_base
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
                auto const& def=GramDeriv().get_rhs(get_id<ID>{});
              #if BOOST_SPIRIT_ATTR_XFORM_IN_RULE
                using rat_t=typename detail::rule_parser<Attribute,ID>
                        ::template rule_attr_transform<ActualAttribute>;
                rat_t rat_v(attr);
                auto& attr_ = rat_v.pre();
                
                bool ok_parse
                  //Creates a place to hold the result of parse_rhs
                  //called inside the following scope.
                  ;
                {
                 // Create a scope to cause the dbg variable below (within
                 // the #if...#endif) to call it's DTOR before any
                 // modifications are made to the attribute, attr_ passed
                 // to parse_rhs (such as might be done in
                 // traits::post_transform when, for example,
                 // ActualAttribute is a recursive variant).
                #if defined(BOOST_SPIRIT_X3_DEBUG)
                    char const* rule_name=name;
                    if(!rule_name) 
                    { //why does this happen?  
                      rule_name="***unknown***";
                    }
                    using make_attribute=traits::make_attribute<Attribute,ActualAttribute>;
                    typedef typename make_attribute::type dbg_attribute_type;
                    detail::context_debug<Iterator, dbg_attribute_type>
                    dbg(rule_name, first, last, dbg_attribute_type(attr_), ok_parse);
                #endif
                    ok_parse=def.parse(first, last, ctx, unused, attr_);
                }
                rat_v.post(ok_parse,attr);
              #else
                bool ok_parse=def.parse(first, last, ctx, unused, attr);
              #endif
                return ok_parse;
              }
            
            char const* name;
          };

        template<typename ID> 
        struct rule_declaration_crtp : GramDeriv
          { 
            template<typename Iterator, typename Context, typename Attribute> 
            bool 
            parse(Iterator&first, Iterator last, Context const&, unused_type, Attribute&)const 
            ; 
          }; 
      };
      
#define BOOST_SPIRIT_DER_DECLARE_(r, scope, rule_name)                    \
    inline auto get_rhs( get_id<typename decltype(rule_name)::id>)const   \
    { using rule_id=typename decltype(rule_name)::id;                     \
      static const gram_base<scope>::template rule_declaration_crtp<rule_id> def; \
      return def; \
    } \
    /***/
#define BOOST_SPIRIT_DER_DEFINE_(r, data, rule_def)               \
    inline auto get_rhs( get_id<typename decltype(rule_def)::id>)const \
    {                                                             \
        static auto const def(rule_def);                          \
        return def;                                               \
    }                                                             \
    /***/
    
#define BOOST_SPIRIT_DER_DECLARE(scope,...) BOOST_PP_SEQ_FOR_EACH(            \
    BOOST_SPIRIT_DER_DECLARE_, scope, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))  \
    /***/
#define BOOST_SPIRIT_DER_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(             \
    BOOST_SPIRIT_DER_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    /***/
      
#define BOOST_SPIRIT_DER_INSTANTIATE(rule_name,rule_value,scope,Iterator,Context,Attribute) \
template<>/*base*/\
  template<>/*rule_declaration_crtp*/\
    template<>/*parse*/\
    bool \
gram_base<scope>::rule_declaration_crtp<typename decltype(scope::rule_name)::id>:: \
    parse(Iterator&first, Iterator last, Context context, unused_type \
      , Attribute attr)const \
    { static auto const def=(rule_name=(rule_value)); \
      return def.parse(first, last, context, unused, attr); \
    } \
  /***/
#endif//BOOST_SPIRIT_GET_RHS_CRTP

}}}

#endif
