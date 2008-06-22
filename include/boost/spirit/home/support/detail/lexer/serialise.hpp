// examples/serialise.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_SERIALISE_HPP
#define BOOST_LEXER_SERIALISE_HPP

#include "state_machine.hpp"
#include <boost/serialization/vector.hpp>

namespace boost
{
namespace lexer
{
// IMPORTANT! This won't work if you don't enable RTTI!
template<class Archive>
void serialise (Archive &ar_, state_machine &sm_, unsigned int version_)
{
    ar_ & version_;
    ar_ & *sm_._lookup;
    ar_ & sm_._dfa_alphabet;
    ar_ & *sm_._dfa;
    ar_ & sm_._seen_BOL_assertion;
    ar_ & sm_._seen_EOL_assertion;
}
}
}

#endif
