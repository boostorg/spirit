//  Copyright (c) 2025 Joaquin M Lopez Munoz
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Compliance with Boost.ConceptCheck/C++20 iterator concepts

#include <boost/config.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/spirit/home/classic/iterator/file_iterator.hpp>
#include <boost/spirit/home/classic/iterator/fixed_size_queue.hpp>
#include <boost/spirit/home/classic/iterator/multi_pass.hpp>
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>
#include <boost/spirit/home/karma/stream/ostream_iterator.hpp>
#include <boost/spirit/home/lex/lexer_lexertl.hpp>
#include <boost/spirit/home/support/iterators/istream_iterator.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/home/support/iterators/look_ahead.hpp>
#include <boost/spirit/home/support/iterators/multi_pass.hpp>
#include <boost/spirit/home/support/iterators/ostream_iterator.hpp>
#include <istream>
#include <ostream>
#include <string>

template<typename F> struct unparens;
template<typename T> struct unparens<void(T)> { typedef T type; };
#define UNPARENS(ParensedArg) typename unparens<void ParensedArg>::type

#if !defined(BOOST_NO_CXX20_HDR_CONCEPTS)

#include <iterator>

#define STATIC_ASSERT_OUTPUT_ITERATOR(ParensedIt, T) \
static_assert(std::output_iterator<UNPARENS(ParensedIt), T>);
#define STATIC_ASSERT_FORWARD_ITERATOR(ParensedIt) \
static_assert(std::forward_iterator<UNPARENS(ParensedIt)>);
#define STATIC_ASSERT_RANDOM_ACCESS_ITERATOR(ParensedIt) \
static_assert(std::random_access_iterator<UNPARENS(ParensedIt)>);

#else

#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>

#define STATIC_ASSERT_OUTPUT_ITERATOR(ParensedIt, T) \
BOOST_CONCEPT_ASSERT((boost::OutputIterator<UNPARENS(ParensedIt), T>));
#define STATIC_ASSERT_FORWARD_ITERATOR(ParensedIt) \
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<UNPARENS(ParensedIt)>));
#define STATIC_ASSERT_RANDOM_ACCESS_ITERATOR(ParensedIt) \
BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<UNPARENS(ParensedIt)>));

#endif

STATIC_ASSERT_RANDOM_ACCESS_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::file_iterator<>));
STATIC_ASSERT_RANDOM_ACCESS_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::fixed_size_queue<char, 10>::iterator));
STATIC_ASSERT_RANDOM_ACCESS_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::fixed_size_queue<char, 10>::const_iterator));
STATIC_ASSERT_FORWARD_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::multi_pass<std::istream_iterator<char>>));
STATIC_ASSERT_FORWARD_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::position_iterator<char*>));
STATIC_ASSERT_FORWARD_ITERATOR((
    BOOST_SPIRIT_CLASSIC_NS::position_iterator2<char*>));
STATIC_ASSERT_OUTPUT_ITERATOR(
    (boost::spirit::karma::ostream_iterator<char, char>),
    char);
STATIC_ASSERT_FORWARD_ITERATOR((
    boost::spirit::lex::lexertl::lexer<
        boost::spirit::lex::lexertl::token<
            std::string::iterator,
            boost::mpl::vector<int>,
            boost::mpl::false_>>::iterator_type));
STATIC_ASSERT_FORWARD_ITERATOR((
    boost::spirit::istream_iterator));
STATIC_ASSERT_FORWARD_ITERATOR((
    boost::spirit::line_pos_iterator<std::istream_iterator<char>>));
STATIC_ASSERT_FORWARD_ITERATOR((
    boost::spirit::look_ahead<std::istream_iterator<char>, 10>));
STATIC_ASSERT_FORWARD_ITERATOR((
    boost::spirit::multi_pass<
        std::istream_iterator<char>,
        boost::spirit::iterator_policies::default_policy<
              boost::spirit::iterator_policies::ref_counted,
              boost::spirit::iterator_policies::no_check,
              boost::spirit::iterator_policies::input_iterator,
              boost::spirit::iterator_policies::fixed_size_queue<10>>>));
STATIC_ASSERT_OUTPUT_ITERATOR(
    (boost::spirit::ostream_iterator),
    char);
