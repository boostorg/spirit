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
#include <boost/spirit/home/x3.hpp>
#include <iostream>

namespace x3 = boost::spirit::x3;

namespace Parser {
    auto const ruleComment = x3::lit("/*") >> *(x3::char_ - "*/") >> "*/" | x3::space;
    
    x3::rule<struct SBlockId> const ruleBlock;
    auto const ruleBlock_def = x3::lit('{') >> *(ruleBlock | "skip_comments_tag" >> x3::skip(ruleComment)[ruleBlock]) >> '}';

    BOOST_SPIRIT_DEFINE(ruleBlock)
}

struct tester {
  unsigned ntests;
  tester():ntests(0){}
  void operator()(std::string const input) {
    std::cout 
      << "Input["<<ntests<<"]:\n"<<input
      << "\n";
    std::cout << "Parsed["<<ntests<<"]:"
      << std::boolalpha << x3::phrase_parse(input.begin(), input.end(), Parser::ruleBlock, x3::space) 
      << "\n";
    ++ntests;
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
		skip_comments_tag{
		}
   })"
    );    
    test
    (
R"({
		skip_comments_tag{
			/*comments*/ 
		}
   })"
    );    
    test
    (
R"({
	{}
	{
		skip_comments_tag{
			{} /*comments*/ 
			{ /*comments*/ } 
		}
	}
   })"
    );    
    return 0;
}    
