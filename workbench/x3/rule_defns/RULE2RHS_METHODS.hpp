#ifndef RULE2RHS_METHODS_HPP_INCLUDED
#define RULE2RHS_METHODS_HPP_INCLUDED
//Purpose:
//  Define macros for various methods of
//  mapping spirit rule to rule_definition.
//References:
//  [tieknot] "tie recyle knot"
//    https://wiki.haskell.org/Tying_the_Knot
//=======================     

#define UNEXP_STRINGIZE(EXP) #EXP

#define RULE2RHS_CTX_LIST 0
#define RULE2RHS_0 UNEXP_STRINGIZE(RULE2RHS_CTX_LIST)
//Store rule->RHS link in context arg to parse function
//using a meta-programming list like structure,
//but do this incrementally in the parse_rhs_main function.
#define RULE2RHS_CTX_MI_ALL (1+RULE2RHS_CTX_LIST)
#define RULE2RHS_1 UNEXP_STRINGIZE(RULE2RHS_CTX_MI_ALL)
//Store rule->RHS link in context arg to parse function
//using multiple inheritance of rule_definition.
//
//Do this for all rules.
#define RULE2RHS_CTX_MI_REC (1+RULE2RHS_CTX_MI_ALL)
#define RULE2RHS_2 UNEXP_STRINGIZE(RULE2RHS_CTX_MI_REC)
//Store rule->RHS link in context arg to parse function
//using multiple inheritance of rule_definition.
//
//However, only do this where needed to "tie the recyle knot"[tieknow].
//For example, in a simple expression grammar, only
//store the rule_definition for the expression rule.
#define RULE2RHS_GET_RHS_ALL (1+RULE2RHS_CTX_MI_REC)
#define RULE2RHS_3 UNEXP_STRINGIZE(RULE2RHS_GET_RHS_ALL)
//Use BOOST_SPIRIT_GET_RHS macro call to create
//specialized get_rhs function to return a rule_definition
//from within the rule::parse function.
//
//Do this for all rules.
#define RULE2RHS_GET_RHS_REC (1+RULE2RHS_GET_RHS_ALL)
#define RULE2RHS_4 UNEXP_STRINGIZE(RULE2RHS_GET_RHS_REC)
//Use BOOST_SPIRIT_GET_RHS macro call to create
//specialized get_rhs function to return a rule_definition
//from within the rule::parse function.
//
//However, only do this where needed to "tie the recyle knot"[tieknot].
#define RULE2RHS_GET_RHS_CRTP (1+RULE2RHS_GET_RHS_REC)
#define RULE2RHS_5 UNEXP_STRINGIZE(RULE2RHS_GET_RHS_CRTP)
//Use BOOST_SPIRIT_GET_RHS_CRTP macro call to create
//specialized *class member* get_rhs function,
//in the CRTP pattern's derived class,
//that returns a rule_definition from within the CRTP pattern's
//base class rule_base::parse function.
//
//However, only do this where needed to "tie the recyle knot"[tieknot].
#define RULE2RHS_SIZE (1+RULE2RHS_GET_RHS_CRTP)
//The number of RULE2RHS methods.
#ifndef RULE2RHS_WHICH
  #define RULE2RHS_WHICH RULE2RHS_CTX_LIST
#endif
char const*const rule2rhs_name[RULE2RHS_SIZE]=
  { RULE2RHS_0
  , RULE2RHS_1
  , RULE2RHS_2
  , RULE2RHS_3
  , RULE2RHS_4
  , RULE2RHS_5
  };
#endif//RULE2RHS_METHODS_HPP_INCLUDED
