//Simplest code showing bug in parse_rule_crtp.cpp
//at revision around 1.27
#include <type_traits>
#include <iostream>
template<unsigned Id>
struct rid
{
    friend
    std::ostream&
  operator<<
    ( std::ostream&os
    , rid
    )
    { return os<<Id;}
};
  template 
  < typename ID
  , typename RHS
  >
struct rule_definition 
  {
    using rule_id=ID;
    rule_definition(RHS a_rhs)
      : rhs(a_rhs){}
      template
      < typename Iterator
      >
      bool
    parse
      ( Iterator& iter
      )
      {
        return rhs.parse(iter);
      }
    RHS rhs;
  };
  
  template
  < typename GramDeriv
  >      
struct gram_base
  {
    template <typename ID>
    struct rule
      {
        using rule_id=ID;
        rule(){}
        
          template <typename RHS>
          rule_definition<ID, RHS>
        operator=(RHS const& rhs) const
          {
            return rule_definition<ID, RHS>(rhs);
          }
          template <typename Iterator>
          bool 
        parse(Iterator& iter) const
          {
            std::cout<<"rule<"<<(ID{})<<">::"<<__func__<<"\n";
            bool result=GramDeriv().parse_rule(ID{},iter);
            return result;
          }
          
      };
  };
  
#define USE_TMP
#ifdef USE_TMP
  #pragma message("yes defined(USE_TMP)")
#else
  #pragma message("not defined(USE_TMP)")
#endif

#ifdef USE_TMP
  #define TMP_CODE(code) code
#else
  #define TMP_CODE(code)
#endif  
    
  struct
always_fail_t
  {
      template
      < typename Iterator
      >
      bool
    parse
      ( Iterator&
      )
      {
        std::cout<<"always_failt_t::"<<__func__<<"\n";
        return false;
      }
  };
  always_fail_t 
always_fail_v
  ;
  
  TMP_CODE(template<typename Ops> )
struct gram_deriv
  : gram_base
    < gram_deriv
      TMP_CODE(<Ops>)
    >
  {
    using base_t=
      gram_base
      < gram_deriv
        TMP_CODE(<Ops>)
      >;
    template <typename ID>
    using rule = typename base_t::template rule<ID>;
    
    enum
    { x_rul
    , y_rul
    };
    
    rule<rid<x_rul>> const x;
    class y_rul{};
    rule<rid<y_rul>> const y;
    
      template
      < typename Iterator
      >
      bool 
    parse_rule
      ( typename decltype(x)::rule_id
      , Iterator&
      );
      template
      < typename Iterator
      >
      bool 
    parse_rule
      ( typename decltype(y = x)::rule_id
      , Iterator& iter
      )
      {
        auto def_v=(y = x);
        return def_v.parse(iter);
      }
  };
  
#define USE_DEF
#ifdef USE_DEF
  #pragma message("yes defined(USE_DEF)")
#else
  #pragma message("not defined(USE_DEF)")
#endif

  TMP_CODE(template<typename Ops>)
      template<typename Iterator>
      bool
      gram_deriv TMP_CODE(<Ops>)::
    parse_rule
      ( typename decltype
      #ifdef USE_DEF
        ( (x = always_fail_v) 
          //With clang3.8,
          //when defined(USE_TMP),
          //  fails compile with error something like:
          //    out-of-line definition of 'parse_rule' 
          //    does not match any declaration in 'gram_deriv<Ops>'.
      #else
        ( x
      #endif
        )::rule_id
      , Iterator& iter
      )
      {
        using x_id=typename decltype(x)::rule_id;
        auto def_v=(x = always_fail_v);
        using def_id=typename decltype(def_v)::rule_id;
        static_assert(std::is_same<x_id,def_id>::value,"x_id!=def_id");
        return def_v.parse(iter);
      }
      
int main()
  {
    char const*input="";
    using gram_t=gram_deriv TMP_CODE(<int>);
    gram_t gram_v;
    bool result
      =  gram_v.x.parse(input)
      || gram_v.y.parse(input)
      ;
    return result;
  }            
  
  
    
    
