# Purpose:

* Understand *how* spirit-x3 "ties the recursive knot".

  [Tying_the_knot](https://wiki.haskell.org/Tying_the_Knot)
  
* Explore alternative methods to do the same.

* Solve problem reported in ML post(s):

  * [compilation never finishes](https://sourceforge.net/p/spirit/mailman/message/35799862/)

    This solves the compile-time problem by avoiding the
    compile-time cost of storing the rule_definition's in
    the context argument to the parse functions.
    
  * [emulate qi](https://sourceforge.net/p/spirit/mailman/message/35561750/)
   
    This allows encapsulation of a grammar using the
    [CRTP-pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
    where the super-class of the concrete grammar class is
    here:
    [CRTP-impl](https://github.com/cppljevans/spirit/blob/get_rhs/include/boost/spirit/home/x3/nonterminal/rule.hpp#L387)
 
# Include Modifications:

* Macro: BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP

  Defined here:
  
  * [CRTP-define](https://github.com/cppljevans/spirit/blob/get_rhs/include/boost/spirit/home/x3/nonterminal/rule.hpp#L18)
  
  Selects the source code here:
  
  * [rule.hpp](https://github.com/cppljevans/spirit/blob/get_rhs/include/boost/spirit/home/x3/nonterminal/rule.hpp)
  * [detail/rule.hpp](https://github.com/cppljevans/spirit/blob/get_rhs/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp)
  
# Support Additions:

  Most the files in this directory help explain the
  solution:
  
    * [operators.hpp](operators.hpp)
      Rewrite of several normal spirit operators without
      attributes. Attributes were nonessential for
      comparing different methods to "tie the knot".
      
    * [rule_defns.cpp](rule_defne.cpp)
      This was original prototype which demonstrated various
      ways to "tie the knot"
      
    * [rule_defns.benchmark.cpp](rule_defns.benchmark.cpp)
      Benchmark code for comparing methods to "tie the knot".
      
    * [Makefile](Makefile)
      When run with target `benchmark`, times various
      methods to "tie the knot".  The results are stored in
      $(BENCH_TMP).
      
# Conclusion:

  Judging from the $(BENCH_TMP) file, the fastest
  compile-time method is the one using the
  BOOST_SPIRIT_DEFINE macros, followed closely by the
  multiple inheritance method, and then followed, at a great
  distance, by the context method.  Hence, it's a mystery
  why the context method was chosen over the multiple
  inheritance method, which, apparently it was, according
  to:
  
[mi-vs-context](http://boost.2283326.n4.nabble.com/compare-of-2-existing-methods-to-implement-grammar-recursion-tt4687574.html#a4687690)

  
    
      
