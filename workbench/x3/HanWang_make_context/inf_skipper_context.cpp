//Purpose:
//  1) Test driver for code in PR237.
/*
https://github.com/boostorg/spirit/pull/237
 */
//     The PR237 code solves an infinite context template
//     recursion problem when storing the
//     skipper in the context with recursive
//     rules.  This problem was 1st mentioned here:
/*
https://sourceforge.net/p/spirit/mailman/message/35963822/
 */
//  2) Allow same test driver(by #define USE_Y3) 
//     for alternative solution (in y3-scanner.hpp) 
//     to problem solved by PR237.
//
//============================================
#define USE_Y3
#ifdef USE_Y3
  #include "y3-scanner.hpp"
  namespace z3 = y3;
#else
  #include <boost/spirit/home/x3.hpp>
  #include "../trace_scope.hpp"
  template<unsigned Id>
  struct type_id{};
  
  namespace z3 = boost::spirit::x3;
  namespace boost::spirit::x3
  {
      template <typename Parser>
      inline bool
      phrase_parse
        ( std::string input
        , Parser const& p
        )
        {
          auto first=input.begin();
          auto last=input.end();
          bool result=phrase_parse(first,last,p,space,unused,skip_flag::post_skip);
          return result && (first==last);
        }
  }
#endif//USE_Y3
struct tester 
{
  unsigned ntests;
  bool all_pass;
  tester()
    : ntests(0)
    , all_pass(true)
    {
    }
  template< typename Parser>
  void operator()( std::string const input, Parser const& p, bool expected=true) 
  {
    std::cout 
      << "Input["<<ntests<<"]:\n"<<input
      << "\n";
    bool const this_pass=z3::phrase_parse(input, p); 
    std::cout << "Parsed["<<ntests<<"]:"
      << std::boolalpha << this_pass
      << "\n";
    ++ntests;
    all_pass=all_pass&&(this_pass==expected);
   }
};

namespace list1_parser 
{
    struct id0{};
    z3::rule< id0 > const lst0;
    
    auto const com0 = z3::lit(":0") | z3::space;
    auto const lst0_def
      =     z3::lit("a") 
         >> 
            (    z3::lit(",")
              >> lst0
            |    z3::lit(";0") 
              >> z3::skip(com0)[lst0]
            | z3::lit(".")
            )
      ;
    BOOST_SPIRIT_DEFINE
    ( lst0
    )

    bool tests()
    {
        boost::trace_scope ts("list1_parser");
        tester test;
        //test( "  a. ", lst0 );
        //test( "  a,a,a. ", lst0 );
        test( "  a,a;0 a, :0 :0 a, a, :0 a. ", lst0 );
        std::cout<<"all_pass="<<std::boolalpha<<test.all_pass<<"\n";
        return test.all_pass;
    }
}//list1_parser namespace

int main()
{

    boost::iostreams::indent_scoped_ostreambuf<char>
  indent_outbuf(std::cout,2);
  std::cout<<"defined(USE_Y3)="
#ifdef USE_Y3
  << "yes"
#else    
  << "not"
#endif  
  <<"\n";
  std::cout<<"defined(USE_SCAN_SKIP)="
#ifdef USE_SCAN_SKIP
  << "yes"
#else    
  << "not"
#endif  
  <<"\n";
  bool all_pass=true;
  {
    boost::trace_scope ts("tests");
    all_pass=all_pass && list1_parser::tests();
  }
  std::cout<<"tests::all_pass="<<std::boolalpha<<all_pass<<"\n";
  return 0;
}  
