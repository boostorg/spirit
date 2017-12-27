//OriginalSource:
//  ./rule_defns.cpp
//  [spirit]
//    https://github.com/boostorg/spirit/tree/develop
//  [toy]
//    [spirit]/workbench/x3/toy/toy.cpp
//  [detail_rule]
//    [spirit]/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp
//  [context]
//    [spirit]/include/boost/spirit/home/x3/support/context.hpp
//  [calc1]
//    http://www.boost.org/doc/libs/1_61_0/libs/spirit/example/qi/compiler_tutorial/calc1.cpp
//Purpose:
//  A compile time benchmark for various methods of linking a rule
//  with it's RHS.
//Author:
//  Larry Joe Evans, 2016-12-31.
//===============

#include <cassert>
#include <boost/core/demangle.hpp>
#include <typeinfo>
  template
  < typename Type
  >
  std::string
type_name()
  {
    return boost::core::demangle(typeid(Type).name());
  }  
#include <utility>
#include <cstring>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include "RULE2RHS_METHODS.hpp"
#define USE_RHS_DEF \
     RULE2RHS_WHICH == RULE2RHS_CTX_LIST \
  || RULE2RHS_WHICH == RULE2RHS_CTX_MI_REC \
  || RULE2RHS_WHICH == RULE2RHS_GET_RHS_REC \
  /***/
#define RULE2RHS_GET_RHS_ANY \
     RULE2RHS_WHICH == RULE2RHS_GET_RHS_REC \
  || RULE2RHS_WHICH == RULE2RHS_GET_RHS_ALL \
  || RULE2RHS_WHICH == RULE2RHS_GET_RHS_CRTP \
  /***/
#define RULE2RHS_CTX_MI_ANY \
     RULE2RHS_WHICH == RULE2RHS_CTX_MI_REC \
  || RULE2RHS_WHICH == RULE2RHS_CTX_MI_ALL \
  /***/
//#define USE_X3_FOR_CTX_LIST
#ifdef USE_X3_FOR_CTX_LIST
  //Use this to test the CTX_LIST method
  //with real spirit x3 code.
  
  #include <boost/spirit/home/x3.hpp>
  using namespace boost::spirit::x3;
  
  auto string_(char const* s){ return standard::string(s);}
  
  template <typename ID>
  using rule_b=rule<ID,unused_type>;
  using empty_context=unused_type;
  #include "trace_scope.hpp"
  using namespace boost;
  
  template <typename Iterator, typename Parser, typename Context=empty_context>
  inline bool parse(Parser const& p, Iterator& first, Iterator last, Context ctx=Context())
  {
     unused_type unused_v;
     unused_type attr;
     return p.parse(first, last, ctx, unused_v, attr);
  }

#else  
  #include "operators.hpp"
  
      template 
      < typename ID
      , typename RHS
      >
    struct rule_definition
      ;
    template <typename ID>
    struct rule_b
      ;
    namespace detail_rule
    {
      struct rule_undefined
        /**@brief
         *  Tag value indicating a rule has not been defined.
         */
        {
            template <typename Iterator, typename Context>
            bool parse(Iterator& first, Iterator last, Context const& ctx) const
            /*! \brief 
             *    Prevent redundant compiletime error in default parse_rule
             *    when static_assert fires.
             */
            {
                assert(false);
                return false;
            }
        };
    
    }//detail_rule namespace
    
  //#define REPORT_CONTEXT_SIZE
  #ifdef REPORT_CONTEXT_SIZE
      template<typename Context>
      std::size_t 
    context_size(Context const&ctx)
      { return 0;
      }
  #endif
  #if RULE2RHS_WHICH == RULE2RHS_CTX_LIST    
    #include "RULE2RHS_CTX_LIST.hpp"
  #elif RULE2RHS_CTX_MI_ANY
    #include "RULE2RHS_CTX_MI_ANY.hpp"
  #elif RULE2RHS_GET_RHS_ANY
    #include "RULE2RHS_GET_RHS_ANY.hpp"
  #endif//#include "RULE2RHS_... .hpp"
  
      template 
      < typename ID
      , typename RHS
      >
    struct rule_definition 
      : parser
        < rule_definition
          < ID
          , RHS
          >
        >
    {
      #if RULE2RHS_GET_RHS_ANY
        using rule_id=ID;
      #endif
        
        rule_definition(RHS const& rhs)
          : rhs(rhs){}
        
          template <typename Iterator, typename Context>
          bool 
        parse
          ( Iterator& first
          , Iterator last
          , Context const& ctx
          ) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<*this;
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
        #ifdef REPORT_CONTEXT_SIZE
          std::cout<<__func__
            <<" in rule_definition<"<<ID::our_id<<">"
            <<":context_size="<<context_size(ctx)<<"\n";
        #endif
        #if RULE2RHS_WHICH == RULE2RHS_CTX_LIST
          bool result=detail_rule::parse_rhs_main<ID>
            ( rhs, first, last, ctx);
        #else
          bool result=rhs.parse( first, last, ctx);
        #endif
          return result;
        }
        
        RHS rhs;

        friend std::ostream&
	operator<<(std::ostream& os, rule_definition<ID,RHS>const&p)
        {
	    os<<"rule_definition<"<<type_name<ID>()<<">";
	    return os;
        }
    };//rule_definition struct

    template <typename ID>
    struct rule_b : parser<rule_b<ID>>
    {
        rule_b(){}
        
        template <typename Derived>
        rule_definition<ID, Derived>
        operator=(parser<Derived> const& definition) const
        {
            return rule_definition<ID, Derived>(definition.derived());
        }

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<type_name<rule_b<ID> >();
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
        #ifdef REPORT_CONTEXT_SIZE
          std::cout<<__func__
            <<"in rule_b<"<<ID::our_id<<">"
            <<":context_size="<<context_size(ctx)<<"\n";
        #endif
        #if RULE2RHS_GET_RHS_ANY
          ID id;
          auto const&def=get_rhs(id);
          bool result=def.parse(first, last, ctx);
        #elif RULE2RHS_CTX_MI_ANY
          auto const&def=get<ID>(ctx);
          bool result=def.parse(first, last, ctx);
        #else
          bool result=parse_rule(*this, first, last, ctx);
        #endif
        #ifdef USE_TRACING
	  std::cout<<":result="<<result<<"\n";
        #endif
	  return result;
        }
        
        friend std::ostream&
	operator<<(std::ostream& os, rule_b<ID>const&p)
        {
          os<<type_name<ID>();
          return os;
        }
    };//rule_b struct
  #if RULE2RHS_WHICH == RULE2RHS_GET_RHS_CRTP
    //Use similar to:
    // https://github.com/cppljevans/spirit/blob/ExagonLinkingError/workbench/x3/rule_defns/parse_rule_crtp.cpp
      template
      < typename GramDeriv
      >      
    struct gram_base
      {
        template <typename ID>
        struct rule_b : parser<rule_b<ID>>
          {
            rule_b(){}
            
            template <typename Derived>
            rule_definition<ID, Derived>
            operator=(parser<Derived> const& definition) const
            {
                return rule_definition<ID, Derived>(definition.derived());
            }
    
            template <typename Iterator, typename Context>
            bool parse(Iterator& first, Iterator last, Context const& ctx) const
            {
              auto def=GramDeriv().get_rhs(ID{});
              bool result=def.parse(first, last, ctx);
        	  return result;
            }
            
            friend std::ostream&
    	operator<<(std::ostream& os, rule_b<ID>const&p)
            {
              os<<type_name<ID>();
              return os;
            }
          };
      };
      
#define BOOST_SPIRIT_GET_RHS_(r, data, rule_def) \
      auto const&                                \
    get_rhs                                      \
      ( typename decltype(rule_def)::rule_id     \
      )                                          \
    {                                            \
        static auto const def(rule_def);         \
        return def;                              \
    }                                            \
    /***/
#define BOOST_SPIRIT_GET_RHS(...) BOOST_PP_SEQ_FOR_EACH(                         \
    BOOST_SPIRIT_GET_RHS_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    /***/
  #endif//RULE2RHS_WHICH == RHS2RHS_CRTP
  
template <typename Iterator, typename Derived, typename Context=empty_context>
inline bool parse(parser<Derived> const& p, Iterator& first, Iterator last, Context ctx=Context())
{
   return p.derived().parse(first, last, ctx);
}

#endif//RULE2RHS_WHICH == RULE2RHS_CTX_LIST  
///////////////////////////////////////////////////////////////////////////////
// test code

template <typename Parser, typename Context=empty_context>
void test_parse(unsigned parse_count, Parser const& p, char const* in, Context ctx=Context())
{
    trace_scope ts(std::string(__func__)+":parse_count="+std::to_string(parse_count));
    std::cout<<":in.beg="<<in<<"\n";
    char const*const end=in + std::strlen(in);
    auto result=parse(p, in, end, ctx);
    while(result && in != end) result=parse(p, in, end, ctx);
    std::cout<<__func__<<":in.end="<<in<<"\n";
    std::cout<<__func__<<":result="<<result<<"\n";
}

namespace gram_bench
{
#define GRAM_BENCH
#if defined(GRAM_BENCH)
    template<unsigned ID> 
    struct id //rule id's.
    {  static auto constexpr our_id=ID;
    };
    //methods where mostly r_I_def used on rhs.
  //#define USE_GRAM_FILE
  #ifdef USE_GRAM_FILE
    #include "gram_file.hpp"
  #else
    #pragma message("!defined(USE_GRAM_FILE)")
    
    //The above #pragma is to safeguard against repeated
    //runs with larger and larger number of rules in
    //gram_file.hpp ; yet, not seeing any difference in the
    //timing for the various methods.  Without this,
    //someone, such as the orig author, could spend hours
    //trying to figure out why there was no difference in
    //performance amoung the various methods :(
    
    //The following grammar is based on a simple aritmetic
    //expression grammar, such as the one in [calc1]:
    //=====================================================
    enum eid //provide more meaningful names for rule id's.
      :unsigned
      { fact
      , term
      , expr
      };
    char const*const op[expr+1]=
      { "_" //corresponds to uint_ in [calc1]
      , "1" //corresponds to * operator in [calc1]
      , "2" //corresponds to + operator in [calc1]
      };
  #if RULE2RHS_WHICH != RULE2RHS_GET_RHS_CRTP
    rule_b<id<fact>> const r_0;
    rule_b<id<term>> const r_1;
    rule_b<id<expr>> const r_2;
  #endif
  #if USE_RHS_DEF

    auto const r_0_def = r_0 =
          string_(op[0])
        |    char_('[') 
          >> r_2 //can't use r_2_def here because not yet defined.
          >> char_(']')
        ; 
   
    auto const r_1_def = r_1 =
           r_0_def
        >> *( string_(op[1]) 
              >> r_0_def
            )
        ;

    auto const r_2_def = r_2 =
           r_1_def
        >> *( string_(op[2])  
            >> r_1_def
            )
        ;
    auto const& start=r_2_def;
    #if RULE2RHS_WHICH == RULE2RHS_CTX_MI_REC
      auto const& ctx=make_defs
        ( r_2_def
        );
    #else
      auto const ctx = empty_context();
      #if RULE2RHS_WHICH == RULE2RHS_GET_RHS_REC
        BOOST_SPIRIT_DEFINE
          ( r_2_def
          );
      #else//RULE2RHS_CTX_LIST
      #endif
    #endif//RULE2RHS_CTX_MI_REC
  #elif RULE2RHS_WHICH != RULE2RHS_GET_RHS_CRTP

    auto const r_0_def = r_0 =
          string_(op[0])
        |    char_('[') 
          >> r_2 //can't use r_2_def here because not yet defined.
          >> char_(']')
        ; 
   
    auto const r_1_def = r_1 =
           r_0
        >> *( string_(op[1])
              >> r_0
            )
        ;

    auto const r_2_def = r_2 =
           r_1
        >> *( string_(op[2])
            >> r_1
            )
        ;
    auto const& start=r_2;
    #if RULE2RHS_WHICH == RULE2RHS_CTX_MI_ALL
      auto const& ctx=make_defs
        ( r_0_def
        , r_1_def
        , r_2_def
        );
    #else//RULE2RHS_GET_RHS_ALL
      auto const ctx = empty_context();
      BOOST_SPIRIT_DEFINE
        ( r_0_def
        , r_1_def
        , r_2_def
        );
    #endif//endif:RULE2RHS_CTX_MI_ALL
  #else//RULE2RHS_GET_RHS_CRTP
    struct gram_deriv: gram_base<gram_deriv>
    {
      rule_b<id<fact>> const r_0;
      rule_b<id<term>> const r_1;
      rule_b<id<expr>> const r_2;
      
      BOOST_SPIRIT_GET_RHS(
        ( r_0 =
            string_(op[0])
          |    char_('[') 
            >> r_2
            >> char_(']')
        ) 
      , ( r_1 =
             r_0
          >> *( string_(op[1])
                >> r_0
              )
        )
      , ( r_2 =
             r_1
          >> *( string_(op[2])
              >> r_1
              )
        )
      )
    };
    gram_deriv g;
    auto const& start=g.get_rhs(id<expr>{});    
    auto const ctx = empty_context();
  #endif//endif:USE_RHS_DEF
  #endif//USE_GRAM_FILE
  void run()
  {
    using namespace gram_bench;
    trace_scope ts("gram_bench");
    unsigned parse_count=0;
    //test_parse(parse_count++,start, "_", ctx) ;
    test_parse(parse_count++,start, "_1_1[_2_]1_2_", ctx) ;
    std::cout << "==========================================" << std::endl;
  }
#else
    void run(){}  
#endif //GRAM_BENCH
}//exit gram_bench namespace
int main()
{

    iostreams::indent_scoped_ostreambuf<char>
  indent_outbuf(std::cout,2);
  #ifdef USE_TRACING
  std::cout<<"yes defined(USE_TRACING)\n";
  #else
  std::cout<<"not defined(USE_TRACING)\n";
  #endif
  #ifdef USE_IOSTREAMS_UTILITY
  std::cout<<"yes defined(USE_IOSTREAMS_UTILITY)\n";
  #else
  std::cout<<"not defined(USE_IOSTREAMS_UTILITY)\n";
  #endif
  #ifdef USE_GRAM_FILE
  std::cout<<"yes defined(USE_GRAM_FILE)\n";
  #else
  std::cout<<"not defined(USE_GRAM_FILE)\n";
  #endif
  std::cout<<"RULE2RHS_WHICH="<<rule2rhs_name[RULE2RHS_WHICH]<<"\n";
  gram_bench::run();
   return 0;
}

