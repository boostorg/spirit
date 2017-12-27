//OriginalSource:
//  http://boost.2283326.n4.nabble.com/file/n4693813/rec_ex_orig.cc
//Purpose:
//  See if rm'ing attributes still produces same problem.
//Result:
//  2017-05-01.2054:
//    Works, when -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS=1.
//    However, still trying to minimize grammar to isolate
//    what is causing infinite template recursion 
//    when -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=0.
//  2017-12-10.1828:
//    compare with on/off values for *_GET_RHS_NS shows
//    the off value compiles almost 2 times slower 
//    than the on value ( 3.214s vs. 1.731s).
//======================
#include <iostream>
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace grammar {
  // Bring in the spirit parsers
  using x3::lexeme;
  using x3::alpha;

  enum rid
  { eq_kleene
  , and_kleene
  , or_kleene
  };
  
  template<rid Id>
  struct rule_id
  {};
//#define AS_WITH_ID
  template<rid Id>
  auto as = [](auto p) 
    {
    #ifdef AS_WITH_ID
      return x3::rule<rule_id<Id>>{} = as_parser(p); 
    #else
      return p;
    #endif
    };

  x3::rule<class path_expr> path_expr = "path-expr";

  auto union_expr = x3::rule<class union_expr>{"union-expr"}
                  = path_expr % '/'
                  ;

  auto equality_expr = x3::rule<class eq_expr>{"equality-expr"}
                     =  union_expr
                     >> *(as<eq_kleene>
                          ( '=' > union_expr )
                         )
                     ;

  auto and_expr = x3::rule<class and_expr>{"and-expr"}
                =  equality_expr
                >> *(as<and_kleene>
                     ( '&' > equality_expr )
                    )
                ;

  auto or_expr = x3::rule<class or_expr>{"or-expr"}
               =  and_expr 
               >> *(as<or_kleene>
                    ( '|' > and_expr )
                   )
               ;

  auto name = x3::rule<class name>{"name"}
            = x3::lexeme[+alpha]
            ;

  auto function_call = x3::rule<class func_call>{"func-call"}
                     = name > '(' > (or_expr % ',') > ')'
                     ;

  auto prim_expr = x3::rule<class prim_expr>{"prim-expr"}
                 = name
                 | ('(' > or_expr > ')')
                 | function_call
                 ;

  auto predicate = '[' > or_expr > ']';

  auto filter_expr = x3::rule<class filter_expr>{"filter-expr"}
                   = prim_expr >> *(predicate)
                   ;

  auto path_expr_def = filter_expr >> prim_expr;

#ifndef USE_DEFINE_PATH                     
  #define USE_DEFINE_PATH 1
#endif  
#if USE_DEFINE_PATH
  BOOST_SPIRIT_DEFINE (path_expr);
#endif  
}

int main() {
//#define PRINT_GET_RHS_NO_CONTEXT
#ifdef PRINT_GET_RHS_NO_CONTEXT
  std::cout<<"BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT=";
#if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
  std::cout<<"1";
#else
  std::cout<<"0";
#endif
  std::cout<<"\n";
#endif//PRINT_GET_RHS_NO_CONTEXT
  using x3::space;
  using grammar::or_expr;

  std::string input = "ab cd | ef gh";

  bool res = phrase_parse(input.begin(),
                          input.end(),
                          or_expr,
                          space
                          );

  if (!res) {
    std::cout << "Parsing failed miserably!\n";
    return 1;
  }
  std::cout << "Parsing passed superbly :))\n";
  return 0;
}
