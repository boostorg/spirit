#ifndef LINKER_ERROR_EXAMPLE_EXAMPLE_HPP
#define LINKER_ERROR_EXAMPLE_EXAMPLE_HPP

#include "ast.hpp"
#include <boost/spirit/home/x3.hpp>

namespace client { namespace parser {

namespace x3 = boost::spirit::x3;

class var_dec_class {};

typedef x3::rule<var_dec_class, ast::VariableDec> var_dec_type;

BOOST_SPIRIT_DECLARE(var_dec_type)

}}

namespace client {

const parser::var_dec_type& var_dec();

}
#endif //LINKER_ERROR_EXAMPLE_EXAMPLE_HPP
