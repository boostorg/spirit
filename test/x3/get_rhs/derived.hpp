#ifndef GET_RHS_DERIVED_HPP_INCLUDED_2017_01_23_0847CST
#define GET_RHS_DERIVED_HPP_INCLUDED_2017_01_23_0847CST
#include <boost/spirit/home/x3.hpp>
#include <string>
using iterator_type = std::string::const_iterator;
using namespace boost::spirit::x3;
template<typename Enum, Enum Id>
struct rule_enum_id
{
};

struct derived:gram_base<derived>
{
  using gram_base<derived>::get_rhs;
  enum rule_enum
  { rule_x
  , rule_y
  , rule_z
  };
  template<rule_enum RuleId>
  using id=::rule_enum_id<rule_enum,RuleId>;
  
  const rule_b<id<rule_x>> x;
  const rule_b<id<rule_y>> y;
  const rule_b<id<rule_z>> z;
  using char_parser=literal_char<boost::spirit::char_encoding::standard, unused_type>;
  const char_parser c;
  derived(char c='x')
    : x{"rule x"}
    , y{"rule y"}
    , z{"rule z"}
    , c{c}
    {}
    
#ifndef USE_DEFINE_ONLY    
  #define USE_DEFINE_ONLY 0
#endif  
#if USE_DEFINE_ONLY==1
  //Only use DEFINE (IOW, don't compile&link) derived.cpp)
  #ifndef SKIP_DEFINE_X
    #define SKIP_DEFINE_X 0
  #endif
  #if SKIP_DEFINE_X==1
    //should produce error.
  #else
    BOOST_SPIRIT_CRTP_DEFINE
      ((x=z>>z)
      )
  #endif//SKIP_DEFINE
#else  
  //use DECLARE and INSTANTIATE (in derived.cpp)
  BOOST_SPIRIT_CRTP_DECLARE(derived,x)
#endif  
  BOOST_SPIRIT_CRTP_DEFINE
    ((y=x>>x)
    ,(z=c)
    )
};//derived struct
#endif//GET_RHS_DERIVED_HPP_INCLUDED_2017_01_23_0847CST
