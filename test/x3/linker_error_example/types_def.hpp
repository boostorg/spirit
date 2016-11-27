#ifndef KYLE_TYPES_DEF_HPP
#define KYLE_TYPES_DEF_HPP

#include "types.hpp"


namespace client { namespace parser {
namespace x3 = boost::spirit::x3;


typedef x3::rule<struct lambda_type_class, ast::LambdaType> lambda_type_type;


const auto& class_type_rule()
{ static class_type_type rule = "class_type";
  return rule;
}
const auto& lambda_type_rule()
{ static lambda_type_type rule = "lambda_type";
  return rule;
}
const auto& type_rule()
{
  static const type_type rule = "type";
  return rule;
}
const auto class_type=class_type_rule();
const auto lambda_type=lambda_type_rule();
const auto type=type_rule();

auto const identifier = +x3::alnum;

auto const type_def =
        (lambda_type | class_type);

auto const lambda_type_def =
        ("(" > -(type % ",") > ")" > "=>" > type)
        | (x3::repeat(1)[class_type] >> "=>" > type);


auto const class_type_def =
        (identifier % "::") >> -("<" > type % "," > ">");

BOOST_SPIRIT_DEFINE(
  lambda_type,
  class_type,
  type
)

}}

namespace client {

const parser::class_type_type& class_type()
{
    return parser::class_type_rule();
}
const parser::type_type& type()
{
    return parser::type_rule();
}

}

#endif //KYLE_TYPES_DEF_HPP
