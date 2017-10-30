//Purpose:
//  Building on parse_rule_def.cpp, see if crtp
//  can be used so that call to parse_rule in rule::parser
//  invokes the parse_rule in derived grammar.
//Conclusion:
//  Can be done; however, not sure what's gained
//  by doing it this way.  This way, each rule
//  and rhs are still unchanging; hence, it would
//  seem using a namespace instead of a crtp class
//  would be simpler and just as powerful.
//==================================================
#include "../type_name.hpp"
#include "operators.hpp"

      template 
      < typename ID
      , typename RHS
      >
    struct rule_definition 
      {
        using rule_id=ID;
        rule_definition(RHS a_rhs): rhs(a_rhs){}
        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
          bool result=rhs.parse( first, last, ctx);
          return result;
        }
        RHS rhs;
      };
      
#define BOOST_SPIRIT_DEFINE_(r, data, rule_def)  \
      auto const&                                \
    parse_rule                                   \
      ( typename decltype(rule_def)::rule_id     \
      )                                          \
    {                                            \
        static auto const def(rule_def);         \
        return def;                              \
    }                                            \
    /***/
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#define BOOST_SPIRIT_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(                         \
    BOOST_SPIRIT_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    /***/
    
  template
  < typename GramDeriv
  >      
struct gram_base
  {
    template <typename ID>
    struct rule : parser<rule<ID>>
      {
        rule(){}
        
        template <typename Derived>
        rule_definition<ID, Derived>
        operator=(parser<Derived> const& definition) const
        {
            return rule_definition<ID, Derived>(definition.derived());
        }

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
          auto def=GramDeriv().parse_rule(ID{});
          bool result=def.parse(first, last, ctx);
    	  return result;
        }
        
        friend std::ostream&
	operator<<(std::ostream& os, rule<ID>const&p)
        {
          os<<type_name<ID>();
          return os;
        }
      };
  };
///////////////////////////////////////////////////////////////////////////////
// test code
struct empty_context{};

template <typename Iterator, typename Derived, typename Context>
inline bool parse(parser<Derived> const& p, Iterator& first, Iterator last, Context ctx)
{
   return p.derived().parse(first, last, ctx);
}

template <typename Parser, typename Context=empty_context>
void test_parse(Parser const& p, char const* in, Context ctx=Context())
{
    std::cout<<__func__<<":in.beg="<<in<<"\n";
    char const*const end=in + std::strlen(in);
    auto result=parse(p, in, end, ctx);
    while(result && in != end) result=parse(p, in, end, ctx);
    std::cout<<__func__<<":in.end="<<in<<"\n";
    std::cout<<__func__<<":result="<<result<<"\n";
}

namespace simple_gram
{
//#define SIMPLE_GRAM
#if defined(SIMPLE_GRAM)
  struct gram_deriv: gram_base<gram_deriv>
  {
    class x_rul{};
    rule<x_rul> const x;
  #if 1
      auto const&
    parse_rule
      ( x_rul
      )
      {
        auto const rhs=char_('x') | char_('a') >> x;
        using RHS=decltype(rhs);
        static rule_definition<x_rul,RHS> const def(rhs);
        return def;
      }
  #else
    //The following generates what's in the #if part
    //of this #if...#endif
    BOOST_SPIRIT_DEFINE(
      x=char_('x') | char_('a') >> x
    )
  #endif
  };
  
    void run()
    {
      trace_scope ts("simple_gram");
      gram_deriv g;
      //test_parse(g.x, "x", ctx) ;
      //test_parse(g.x, "ax", ctx) ;
      test_parse(g.x, "aaaaax") ;
      //test_parse(g.x, "aaz", ctx) ;
      std::cout << "==========================================" << std::endl;
    }
#else
    void run(){}  
#endif //SIMPLE_GRAM
}//exit simple_gram namespace
namespace calc1_gram
{
#define CALC1_GRAM
#if defined(CALC1_GRAM)
//The following is pretty much copied from [calc1]:
    template
    < typename Ops
    >
  struct gram_deriv
  : gram_base<gram_deriv<Ops>>
  , Ops
  {
    using base_t=gram_base<gram_deriv<Ops>>;
    template <typename ID>
    using rule = typename base_t::template rule<ID>;
    using Ops::plus;
    using Ops::times;
    
    class expr_rul{};
    rule<expr_rul> const expr;
    class fact_rul{};
    rule<fact_rul> const fact;
    class term_rul{};
    rule<term_rul> const term;

    BOOST_SPIRIT_DEFINE
      ( ( expr=
            term
            >> *( plus() >> term
                )
        )
      , ( term=
            fact
            >> *( times() >> fact
                )
        )
      , ( fact=
                char_('0')
            |   char_('1')
            |   char_('(') >> expr >> char_(')')
            //For simplicity, no unary + operator.
        )
      )
  };//gram_deriv
    struct ops
    {
      static auto plus(){ return char_('+');}
      static auto times(){ return char_('*');}
    };
  void run()
  {
    trace_scope ts("calc1_gram");
    gram_deriv<ops> g;
//    test_parse(g.expr, "1", ctx) ;
//    test_parse(g.expr, "1+0", ctx) ;
//    test_parse(g.expr, "1+0+1*0*0*0", ctx) ;
    test_parse(g.expr, "1*(0+1)*0") ;
//    test_parse(g.expr, "1+0/1", ctx) ;
    std::cout << "==========================================" << std::endl;
  }
#else
    void run(){}  
#endif //CALC1_GRAM
}//exit calc1_gram namespace

int main()
{

    iostreams::indent_scoped_ostreambuf<char>
  indent_outbuf(std::cout,2);
  #ifdef USE_IOSTREAMS_UTILITY
  std::cout<<"yes defined(USE_IOSTREAMS_UTILITY)\n";
  #else
  std::cout<<"not defined(USE_IOSTREAMS_UTILITY)\n";
  #endif
  #ifdef USE_TRACING
  std::cout<<"yes defined(USE_TRACING)\n";
  #else
  std::cout<<"not defined(USE_TRACING)\n";
  #endif
  simple_gram::run();
  calc1_gram::run();
  return 0;
}
