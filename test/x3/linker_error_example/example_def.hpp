#ifndef LINKER_ERROR_EXAMPLE_EXAMPLE_DEF_HPP
#define LINKER_ERROR_EXAMPLE_EXAMPLE_DEF_HPP

#include "example.hpp"
#include "types.hpp"

namespace client { namespace parser {

const var_dec_type& var_dec_rule()
{ static const var_dec_type rule = "var_dec";
  return rule;
}
const auto var_dec=var_dec_rule();

//#define EXAMPLE_DEF_LINK_ERR
#if defined(EXAMPLE_DEF_LINK_ERR)
#pragma message "yesdef(EXAMPLE_DEF_LINK_ERR)"
#else
#pragma message "notdef(EXAMPLE_DEF_LINK_ERR)"
#endif
auto const var_dec_def = x3::lexeme["var "]
                         > +x3::alnum
                         > ":"
                       #ifdef EXAMPLE_DEF_LINK_ERR 
                         >> type() //<- this gets linker error.
                       #else
                         > 
                         #ifdef VARIABLE_DEC_OPTIONAL
                          -type()
                         #else 
                           type() //<- This links.
                         #endif
                       #endif
                         > ";";

BOOST_SPIRIT_DEFINE(
  var_dec
)

}}

namespace client {

const parser::var_dec_type& var_dec()
{
    return parser::var_dec_rule();
}

}

#endif //LINKER_ERROR_EXAMPLE_EXAMPLE_DEF_HPP
