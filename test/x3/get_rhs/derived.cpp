#include "derived.hpp"
using context_type=
#if 1
  //simple copy&past&reformat from linker error msg
  //produced if using the #else part fails to link.
  boost::spirit::x3::context
  < boost::spirit::x3::skipper_tag
  , boost::spirit::x3::char_class
    < boost::spirit::char_encoding::ascii
    , boost::spirit::x3::space_tag
    > const
  , boost::spirit::x3::unused_type
  >
#else
  unused_type
#endif  
  ;
using attribute_type=
  typename decltype(derived::x)::attribute_type;

BOOST_SPIRIT_CRTP_INSTANTIATE(x,(z>>z),derived,iterator_type,context_type const&,attribute_type&)
