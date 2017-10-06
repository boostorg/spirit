#Purpose:

* Understand *how* spirit-x3 "ties the recursive knot":

  [Tying_the_Knot](https://wiki.haskell.org/Tying_the_Knot)
  
* Explore alternative methods to do the same.

* Solve problem reported in ML post(s):

  * [compilation never finishes](https://sourceforge.net/p/spirit/mailman/message/35799862/)

    This solves the compile-time problem by avoiding storing
    the rule_definition's in the context argument to the
    parse functions.
    
  * [emulate qi](https://sourceforge.net/p/spirit/mailman/message/35561750/)
   
    This allows encapsulation of a grammar using the
    [CRTP-pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
    where the super-class of the concrete grammar class is
    here:
    [CRTP-impl](https://github.com/cppljevans/spirit/blob/develop/include/boost/spirit/home/x3/nonterminal/rule.hpp#L313)
 
#Include Modifications:

* Macro: BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP

  Defined here:
  
  * [#define](https://github.com/cppljevans/spirit/blob/develop/include/boost/spirit/home/x3/nonterminal/rule.hpp#L10)
  
  Selects the source code here:
  
  * [rule.hpp](https://github.com/cppljevans/spirit/blob/develop/include/boost/spirit/home/x3/nonterminal/rule.hpp)
  * [detail/rule.hpp](https://github.com/cppljevans/spirit/blob/develop/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp)
  
#Support Additions:

