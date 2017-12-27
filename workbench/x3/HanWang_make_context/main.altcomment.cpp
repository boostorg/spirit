//Purpose:
//  Reproduce problem reported here:
/*
https://stackoverflow.com/questions/45282293/spirit-not-able-to-use-x3skipskippersome-recursive-rule-in-its-rule-defin
 */
//  and discussed here:
/*
https://sourceforge.net/p/spirit/mailman/message/35963822/
 */ 
//Usage:
/* 
#if BOOST_SPIRIT_X3_SKIP_MAKE_UNIQUE
  //Solves problem, sort of.
#else
  //Shows problem.  Results in infinite template recursion.
  //WARNING: 
  //  May take a while to compile.  
  //  ***Be prepared to abort compilation.***
#endif  
*/ 
//================
#include "main.altcomment.blocks.hpp"
#include <iostream>
struct tester {
  unsigned ntests;
  bool all_pass;
  tester():ntests(0),all_pass(true){}
  void operator()(std::string const input) {
    std::cout 
      << "Input["<<ntests<<"]:\n"<<input
      << "\n";
    bool const this_pass=x3::phrase_parse(input.begin(), input.end(), Parser::ruleBlock, x3::space); 
    std::cout << "Parsed["<<ntests<<"]:"
      << std::boolalpha << this_pass
      << "\n";
    ++ntests;
    all_pass=all_pass&&this_pass;
   }
};

int main() {
    tester test;
    test
    (
R"({
   })"
    );    
    test
    (
R"({
		skip1{
		}
   })"
    );    
    test
    (
R"({
		skip1{
			/*1comments*/ 
		}
   })"
    );    
    test
    (
R"({
	{}
	{
		skip1{
			{} /*1comments*/ 
			{ /*1comments*/ } 
		}
	}
   })"
    );
    test
    (
R"({
	skip2{
           /*2comments*/
	}
   })"
    );
    test
    (
R"({
     skip1
     { /*1comments*/
       skip2{ /*2comments*/ } 
       skip1{ /*1comments*/ }
     }
   })"
    );
    test
    (
R"({
     skip1
     {  /*1comments*/
        skip2
        { /*2comments*/
          skip1{ /*1comments*/ }
        }
        /*1comments*/
     }
   })"
    );
    std::cout<<"all_pass="<<std::boolalpha<<test.all_pass<<"\n";
    return 0;
}    
