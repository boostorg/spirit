///////////////////////////////////////////////////////////////////////////////
/**
 * \defgroup link_rule_to_rhs_by_multi-inheritance
 * @{
 */
//Purpose:
//  This code uses multi-inheritance to record the link between
//  a rule<ID> and it's rule_definition.
//  It's not in current spirit (2016-07-28), AFAICT.

      template
      < typename... RuleDefinition
      >
      struct
    rule_defns
      ;
    #ifdef REPORT_CONTEXT_SIZE
        template <typename... RuleDefinition>
        std::size_t 
      context_size(rule_defns<RuleDefinition...>const& ctx)
        { return sizeof...(RuleDefinition);
        }
    #endif
      template
      < typename... ID
      , typename... RHS
      >
    struct rule_defns
      < rule_definition
        < ID
        , RHS
        >...
      >
      : rule_definition
        < ID
        , RHS
        >...
      {
      private:
          template
          < typename GetId
          >
            static
          auto constexpr
        get_def_impl
          ( ... //overload resolution prefers anything over ...
          )
          /**@brief
           *  No rule_definition< GetId, RHS> in superclasses.
           */
          {
              return detail_rule::rule_undefined();
          }
          template
          < typename GetID
          , typename GetRHS
          >
            static
          auto constexpr
        get_def_impl
          ( rule_definition
            < GetID
            , GetRHS
            >const* a_def
          )
          /**@brief
           *  Only used by member function, get_def()const.
           */
          {
              return *a_def;
          }
      public:
          template
          < typename GetID
          >
          auto  constexpr
        get_def()const
          /**@brief
           *  retrieve the definition of AnVarble.
           */
          {
            return get_def_impl<GetID>(this);
          }
        template<typename... Defs>
        rule_defns(Defs... defs)
          : rule_definition
            < ID
            , RHS
            >(defs)...
          {}
        
      };//rule_defns
      template
      < typename ID
      , typename... RuleDefinition
      >
      auto
    get
      ( rule_defns<RuleDefinition...>const& defs
      )
      {
      #ifdef USE_TRACING
        trace_scope ts(std::string(__func__)+"<"+type_name<ID>()+">");
      #endif
        return defs.template get_def<ID>();
      }
    using empty_context=rule_defns<>;
      template
      < typename... ID
      , typename... RHS
      >
      auto
    make_defs
      ( rule_definition< ID, RHS>... defs
      )
      {
        #ifdef USE_TRACING
          trace_scope ts(std::string(__func__));
        #endif
          return 
            rule_defns
            < rule_definition< ID, RHS>...
            >(defs...);
      }
    
/** @}*/
