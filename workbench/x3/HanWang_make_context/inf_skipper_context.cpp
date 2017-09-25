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

namespace lit_parser 
{
//#define LIT_PARSER
#ifdef LIT_PARSER
    auto const lit_def = z3::lit("{}") | z3::lit("()");
    bool tests()
    {
        boost::trace_scope ts("lit_parser");
        tester test;
        test( "  {}  ", lit_def );
        test( "  ()  ", lit_def );
        std::cout<<"all_pass="<<std::boolalpha<<test.all_pass<<"\n";
        return test.all_pass;
    }
#else
    bool tests()
    {
        return true;
    }
#endif
}

namespace list1_parser 
{
#define LIST1_PARSER
#ifdef LIST1_PARSER
    template<unsigned Id>
    struct lsti{};
    template<unsigned Id>
    struct comi{};
    
    z3::rule< comi<0> > const com0;
    z3::rule< lsti<0> > const lst0;
    
    auto const com0_def = z3::lit(":0") | z3::space;
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
    , com0
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
#else
    bool tests()
    {
        return true;
    }
#endif//list1_parser
}//list1_parser namespace

namespace block1_parser 
{
#define BLOCK1_PARSER
#ifdef BLOCK1_PARSER
    template<unsigned Id>
    struct blki{};
    
    auto const com0 = z3::lit("/**/") | z3::space;
    z3::rule< blki<0> > const blk0;
    auto const blk0_def
      =  z3::lit("{") 
      >> *(  blk0 
          | z3::lit("skip0") >> z3::skip(com0)[blk0]
          ) 
      >> z3::lit("}")
      ;
    BOOST_SPIRIT_DEFINE
    ( blk0
    )

    bool tests()
    {
        boost::trace_scope ts("block1_parser");
        tester test;
        test( "  { skip0 { /**/ } }  ", blk0 );
        std::cout<<"all_pass="<<std::boolalpha<<test.all_pass<<"\n";
        return test.all_pass;
    }
#else
    bool tests()
    {
        return true;
    }
#endif//block1_parser
}//block1_parser namespace

namespace blockn_parser
{
#define BLOCKN_PARSER
#ifdef BLOCKN_PARSER
    template<unsigned Id>
    struct blki{};
    
    z3::rule<blki<0>> const blk0_ru;
    z3::rule<blki<1>> const blk1_ru;
    z3::rule<blki<2>> const blk2_ru;
    
    template<unsigned Id>
    struct comi{};
    
    z3::rule<comi<0>> const com0_ru;
    z3::rule<comi<1>> const com1_ru;
    z3::rule<comi<2>> const com2_ru;
    
    auto const com0_ru_def = z3::space;
    auto const com1_ru_def = z3::lit("/*1*/") | z3::space;
    auto const com2_ru_def = z3::lit("/*2*/") | z3::space;
    
    auto const blk0_ru_def 
      = z3::lit("{0") 
      >> *( blk0_ru
          |    z3::lit("skip1")
            >> z3::skip(com1_ru)[blk1_ru]
          ) 
      >> z3::lit("0}");
    auto const blk1_ru_def 
      = z3::lit("{1") 
      >> *( blk1_ru
          |    z3::lit("skip2") 
            >> z3::skip(com2_ru)[blk2_ru]
          ) 
      >> z3::lit("1}");
    auto const blk2_ru_def 
      = z3::lit("{2") 
      >> *( blk2_ru
          |    z3::lit("skip0") 
            >> z3::skip(com0_ru)[blk0_ru]
          ) 
      >> z3::lit("2}");
      
    BOOST_SPIRIT_DEFINE
    ( blk0_ru
    , blk1_ru
    , blk2_ru
    , com0_ru
    , com1_ru
    , com2_ru
    )

    bool tests()
    {
        boost::trace_scope ts("blockn_parser");
        tester test;
#if 1       
        test
        (
    R"({0
       0})"
        , blk0_ru
        );
#endif        
        test
        (
    R"({0
         skip1{1
         1}
       0})"
        , blk0_ru
        );    
        test
        (
    R"({0
         skip1
         {1
           /*1*/ 
         1}
       0})"
        , blk0_ru
        );    
        test
        (
    R"({0
         {0 0}
         {0
    	skip1
            {1
              {1 1} /*1*/ 
    	      {1 /*1*/ 1} 
            1}
         0}
       0})"
        , blk0_ru
        );
    #if 1   
        test
        (
    R"({0
         skip2
         {2
           /*2*/
         2}
       0})"
        , blk0_ru
        , false //because skip1 instead of skip2 required.
        );
        test
        (
    R"({0
         skip1
         {1 /*1*/
           skip2
           {2 /*2*/ 
           2} 
         1}
       0})"
        , blk0_ru
        );
    #endif    
        test
        (
    R"({0
         skip1
         {1 /*1*/
            skip2
            {2 /*2*/
               skip0{0 0}
               /*2*/
            2}
            /*1*/
         1}
       0})"
        , blk0_ru
        );
        std::cout<<"all_pass="<<std::boolalpha<<test.all_pass<<"\n";
        return test.all_pass;
    }//tests
#else
    bool tests()
    {
        return true;
    }
#endif//BLOCKN_PARSER
}//blockn_parser namespace

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
    all_pass=all_pass && lit_parser::tests();
    all_pass=all_pass && list1_parser::tests();
    all_pass=all_pass && block1_parser::tests();
    all_pass=all_pass && blockn_parser::tests();
  }
  std::cout<<"tests::all_pass="<<std::boolalpha<<all_pass<<"\n";
  return 0;
}  
