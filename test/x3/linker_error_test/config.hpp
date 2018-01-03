#ifndef LINKER_ERROR_EXAMPLE_CONFIG_HPP
#define LINKER_ERROR_EXAMPLE_CONFIG_HPP

#include <boost/spirit/home/x3.hpp>

namespace client{
namespace parser{

namespace x3 = boost::spirit::x3;


typedef std::string::const_iterator iterator_type;
typedef x3::phrase_parse_context<x3::ascii::space_type>::type context_type;

}
}

#endif //LINKER_ERROR_EXAMPLE_CONFIG_HPP
