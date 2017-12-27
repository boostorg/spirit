# ﻿spirit::get_rhs-branch

## Purpose

The code in this branch should solve the problems enumerated
in the next section.  The code solving each problem is
delimited by "Guard" macros of the form:

  `BOOST_SPIRIT_X3_EXPERIMENTAL_<tail>`

where `<tail>` is one of:

  * GET_RHS_NS
  * GET_RHS_CRTP
  * ATTR_XFORM_IN_RULE
  
These "Guard" macros for a particular problem can be switched 
on or off by setting the values of `0` (for off) or `1` (for
on).  This allows:

  * Reviewers to clearly see which code is intended to solve
    which problem.

  * Future users, if they encounter problems with code, to
    revert back to existing `spirit` code.
    
    *HOWEVER*, that last is not *entirely* true in the case
     of the `ATTR_XFORM_IN_RULE` macro.  In this case, in
     order to avoid code duplication, a new function,
     `rule_attr_transform_f` was created, which abstracts
     parts of the code in the existing `spirit` code in the
     `call_rule_defintion` function in
     `nonterminal/detail/rule.hpp`.
     
In each section below, there's a listing of:

  * The "Guard" macro which switches the change on or off.
  
  * The problem the change solves.
  
  * An abstract of how the `get_rhs` branch solves problem.
  
  * Interest in solution.  This may include references to the
    mailing list posts where the problem is reported or
    discussed.
    
  * Optionally, examples.
    
  * test/benchmark code links
  
### Problems solved:

#### Excessive compile times when BOOST_SPIRIT_DEFINE not used.
  * Guard Macro: BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NS
  * Problem:
  
    The existing `spirit` uses 2 methods to "associate" the `rule<ID,Attribute>`
    `LHS`(left-hand-side) with the rule `RHS`(right-hand-side):
      
      * BOOST_SPIRIT_DEFINE macros:
        These macros are defined starting here:
        [rule.hpp macros](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/rule.hpp#L142)
      * make_rule_context method:
        If the `BOOST_SPIRIT_DEFINE macro` method is not
        used and a `rule_definition` appears on the `RHS` of
        some rule, then the `LHS -> RHS` association is
        stored in the context
        argument to the `parse` function.  This is done by
        calling, indirectly,
        [make_rule_context](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp#L149),
        from `rule_definition::parse`.
          
    The `make_rule_context method` causes the excessive
    compile times mentioned in the posts
    under the `Interest in solution` item below.  The reason
    is probably that template meta-programming is known to
    tax the compiler and the `make_rule_context` does
    template meta-programming.  Benchmark code mentioned
    below supports this conclusion.
        
  * get_rhs solution.
  
    Get_rhs simply eliminates `make_rule_context method` to
    associate the rule's `LHS` with it's `RHS`.
    
      * Pro's
      
        Compile time does not increase nearly as fast as the
        number of rules increases.
        
      * Con's
      
        Breaks code relying on `make_rule_context` to
        associate the rule's `LHS` with its `RHS`.  However,
        at some inconvenience, such code can be fixed by
        simply using the macro method and placing the rule
        definitions in a namespace.
            
  * Interest in solution.
   
      * [Arun.compilation never finishes](https://sourceforge.net/p/spirit/mailman/message/35799862/)
      * [Soren.wccs_parser](https://sourceforge.net/p/spirit/mailman/message/36091645/)
            
  * example code:
      * [rec_ex_org.no_attr](https://github.com/cppljevans/spirit/blob/get_rhs/workbench/x3/rec_ex_orig/rec_ex_orig.no_attr.cpp)
      * [wccs_parser](https://github.com/cppljevans/spirit/blob/get_rhs/workbench/x3/rec_ex_orig/wccs_parser.cpp)
  * test code:
      Search for `BOOSTT_SPIRIT_DEFINE` in the following code:
      * [test3/rule1](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/test/x3/rule1.cpp)
  * benchmark results:
      Benchmark results are produced by
      
      [bench_make](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/workbench/x3/rule_defns/Makefile#L102)
      This Makefile target benchmarks a *prototype* of the
      `get_rhs` code.  This prototype code does not use
      attributes (since the purpose is to measure
      *compile-time* of *grammar recursion* implementation,
      not how *attribute processing* affects *compile-time*)
      and only has the minimum grammar operators needed to
      generate the benchmark grammar.  The results:
      
      [bench_results][bench_ref]
        
      clearly show that as the number of rules increases,
      the compile time, using the current `spirit` context
      method (designated with the `RULE2RHS_CTX_LIST` tag in
      the results) for associating the `LHS` to the `RHS` of
      a rule, gets much slower relative to *all* the other
      methods.
      
      The descriptions for all the methods in [bench_results][bench_ref]
      are in the comments in:
  
      [RULE2RHS_METHODS.hpp](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/workbench/x3/rule_defns/RULE2RHS_METHODS.hpp)
        
#### x3 like qi-better encapsulaton

  * Guard Macro: BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
  * Problem:
      Using BOOST_SPIRIT_DEFINE requires rule definitions in
      some namespace.  In contrast, `Qi` encapsulates rule
      definitions in class.  `X3` should be more like `Qi` to
      enable better grammar encapsulation.
      
  * get_rhs solution.
  
    Get_rhs solves this using the
    [crtp](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern#General_form)
    design pattern.  The base class, `gram_base`, of this
    pattern contains a nested template class, `rule_b`,
    which plays a role similar to that of the `rule`
    template class in the existing `spirit` code.  The base
    class also has a default `get_rhs` template function
    which plays a role similar to that of the default
    `parse_rule` function in the existing `spirit` code in
    `nonterminal/rule.hpp`.
    
    The derived class uses the `BOOST_SPIRIT_DEFINE_CRTP`
    macro to generate a `get_rhs` member function which
    overrides the default `get_rhs` member function in the
    base class for a specific `rule_b<...>::id`, much the
    way `parse_rule` function generated by the
    `BOOST_SPIRIT_DEFINE` macro overrides the default
    `parse_rule` function defined near the top of
    `nonterminal/rule.hpp`.
      
  * Interest in solution.
      * [Pirkweiser.Design X3 like Qi parser](http://boost.2283326.n4.nabble.com/Design-structure-X3-parser-more-like-Qi-parser-td4690205i20.html)
        
  * example code:
      * [wb3/parse_rule_crtp](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/workbench/x3/rule_defns/parse_rule_crtp.hpp#L145)
      * [wb3/bench](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/workbench/x3/rule_defns/rule_defns_bench.cpp#L235)
        
  * test code:
    Search for `BOOSTT_SPIRIT_CRTP_DEFINE` in the following code:
      * [test3/rule1](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/test/x3/rule1.cpp)
      * [test3/debug](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/test/x3/debug.cpp)
      * [test3/get_rhs](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/test/x3/get_rhs/)
  * benchmark results:
  
    The benchmark results, [bench_results][bench_ref], shows
    this method, the one with the `RULE2RHS_GET_RHS_CRTP`
    tag, is *marginally* slower than the fasted method.
            
#### BOOST_SPIRIT_INSTANTIATE link error

  * Guard Macro: BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE
  * Problem:
  
      Separate compilation of grammar rules using the
      `BOOST_SPIRIT_INSTANTIATE` macro, under some
      circumstances( maybe when attribute is *not* of type
      unused_type *and* there's no semantic action?) leads
      to link error.
      
      The problem occurs because the
      `BOOST_SPIRIT_INSTANTIATE` macro generates a
      `parse_rule` instantiation requiring a specific type,
      `rule_type::attribute_type`, for the rule's attribute
      argument. In contrast, the `rule<ID,Attribute>::parse`
      function takes an arugment `ActualAttribute& attr` and
      passes that to the `parse_rule` function.  When
      `Attribute != ActualAttribute`, the link error occurs.
      
  * Interest in solution.
      * [Exagon.linking-error-when](http://boost.2283326.n4.nabble.com/Linking-error-when-changing-to-tp4689820.html)
      * [OlafPeter.linkerr-separate-tu](https://stackoverflow.com/questions/43791079/x3-linker-error-with-separate-tu)
      * [Halvorsen.instant-linkerr](http://boost.2283326.n4.nabble.com/X3-declare-define-instantiate-linker-error-tp4694282.html)

  * Method of solution.
  
    In the current `spirit` method, the `ActualAttribute&
    attr` that is passed to the `parse_rule` function that
    is called by `rule<ID,Attribute>::parse` is transformed,
    after a few intervening function calls, into `Attribute&
    _attr` by code at the beginning of the:
    
      > `detail::rule_parser<...>::call_rule_definition` 
    
    function.  This `attribute transform code` extends
    [from here](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp#L306)
    [to here](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp#L347), 
    execept for the `parsing/debugging code` actually doing the
    parsing.  This `parsing/debugging code` extends
    [from here](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp#L319)
    [to here](https://github.com/boostorg/spirit/blob/master/include/boost/spirit/home/x3/nonterminal/detail/rule.hpp#L341)
    and separates the `attribute pre-transform code` from
    the `attribute post-transform code`.
      
    The link problem solution requires doing the attribute
    transformation in different places.
    To avoid code duplicaton, the combination of the `attribute transform code`
    and the `parsing/debugging code` mentioned in the
    previous paragraph has been replaced by the
    `rule_attr_transform_f` function in `detail/rule.hpp`.
    The `parsing` part of the `parsing/debugging code`
    mentioned in the previous paragraph is passed to this
    function by the `parser` argument.
    
    The link problem is solved by calling the
    `rule_attr_transform_f` in `rule<...>::parse` instead of
    calling `parse_rule` and removing it from
    `detail::rule_parser<...>::call_rule_definition`.
    However, without this transformation called from within,
    indirectly, `rule_definition<...>::parse`, using an
    instance of `rule_definition<...>` on the rhs of a
    grammar expression would result in no attribute
    transformation, which of course would be invalid.  But
    the transformation cannot be put in
    `rule_definition<...>::parse` because that would
    duplicate the transformation in `rule<...>::parse` when
    it calls, indirectly, the `rule_definition<...>::parse`
    via the `parse_rule` generated by the
    `BOOST_SPIRIT_DEFINE` macro.  The solution is to have
    this generated `parse_rule` call instead the new
    function `rule_definition<...>::parse_no_xform` which
    does no attribute transform.
    
  * test code:
      * [test3/linker_error_example](https://github.com/cppljevans/spirit-experiments/blob/get_rhs/test/x3/linker_error_example/)
    
[bench_ref]: https://github.com/cppljevans/spirit-experiments/blob/get_rhs/workbench/x3/rule_defns/bench.tmp


