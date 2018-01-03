#include "config.hpp"
#include "types_def.hpp"
#include <iostream>

namespace client {namespace parser {

BOOST_SPIRIT_INSTANTIATE(type_type, iterator_type, context_type);

BOOST_SPIRIT_INSTANTIATE(class_type_type, iterator_type, context_type);
}}


