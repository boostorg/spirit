#ifndef KYLE_PARSER_TYPES_HPP
#define KYLE_PARSER_TYPES_HPP

#include "ast.hpp"
#include <boost/spirit/home/x3.hpp>

namespace client { namespace parser {
namespace x3 = boost::spirit::x3;

struct class_type_class;
struct type_class;

typedef x3::rule<class_type_class, ast::ClassType> class_type_type;
typedef x3::rule<type_class, ast::Type> type_type;

BOOST_SPIRIT_DECLARE(class_type_type,
                     type_type)


}}


namespace client {

const parser::class_type_type& class_type();
const parser::type_type& type();

}

#endif
