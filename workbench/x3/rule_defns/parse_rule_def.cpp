//Purpose:
//  See if overloaded parse_rule template function 
//  can be changed to return rule_definition instead of
//  another parse function.
//Conclusion:
//  Yes.
//==================================================
#include "operators.hpp"
#include "../type_name.hpp"

      template<bool Result>
    struct always_parser
      : parser<always_parser<Result>>
      {
        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
          return Result;
        }
      };
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
      template 
      < typename ID
      >
      inline auto const&
    parse_rule
      ( ID
      )
      {
        using rhs=always_parser<false>;
        static rule_definition<ID,rhs> const def(rhs{});
        return def;
      }
#define BOOST_SPIRIT_DEFINE_(r, data, rule_def)  \
      inline auto const&                         \
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
          auto const& def=parse_rule(ID{});
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

///////////////////////////////////////////////////////////////////////////////
// test code
struct empty_context{};

template <typename Iterator, typename Derived, typename Context=empty_context>
inline bool parse(parser<Derived> const& p, Iterator& first, Iterator last, Context ctx=Context())
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
  class x_rul{};
  rule<x_rul> const x;
  auto const x_def=char_('x') | char_('a') >> x;
  #if 0
      auto const&
    parse_rule
      ( x_rul
      )
      {
        using RHS=decltype(x_def);
        static rule_definition<x_rul,RHS> const def(x_def);
        return def;
      }
  #else
    BOOST_SPIRIT_DEFINE(x=x_def)
  #endif
  
  auto const ctx = empty_context();
    void run()
    {
      trace_scope ts("simple_gram");
      //test_parse(x, "x", ctx) ;
      //test_parse(x, "ax", ctx) ;
      test_parse(x, "aaaaax", ctx) ;
      //test_parse(x, "aaz", ctx) ;
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
    class expr_rul{};
    rule<expr_rul> const expr;
    class fact_rul{};
    rule<fact_rul> const fact;
    class term_rul{};
    rule<term_rul> const term;

    auto const expr_def =
        term
        >> *( char_('+') >> term
            )
        ;

    auto const term_def =
        fact
        >> *( char_('*') >> fact
            )
        ;

    auto const fact_def =
            char_('0')
        |   char_('1')
        |   char_('(') >> expr >> char_(')')
        //For simplicity, no unary + operator.
        ; 
   
    BOOST_SPIRIT_DEFINE(
          (expr=expr_def)
        , (term=term_def)
        , (fact=fact_def)
        )
    auto const ctx = empty_context();
  void run()
  {
    using namespace calc1_gram;
    trace_scope ts("calc1_gram");
//    test_parse(expr_def, "1", ctx) ;
//    test_parse(expr_def, "1+0", ctx) ;
//    test_parse(expr_def, "1+0+1*0*0*0", ctx) ;
    test_parse(expr, "1*(0+1)*0", ctx) ;
//    test_parse(expr_def, "1+0/1", ctx) ;
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
