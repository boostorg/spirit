#include "derived.hpp"
using context_type=
  boost::spirit::x3::context
  < boost::spirit::x3::skipper_tag
  , boost::spirit::x3::char_class
    < boost::spirit::char_encoding::ascii
    , boost::spirit::x3::space_tag
    > const
  , boost::spirit::x3::unused_type
  >;

BOOST_SPIRIT_DER_INSTANTIATE(x,(z>>z),derived,iterator_type,context_type)
