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

auto const var_dec_def = x3::lexeme["var "]
                         > +x3::alnum
                         > ":"
                         >> type() //<- this gets linker error without patch.
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
