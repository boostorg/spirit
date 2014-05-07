/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2014 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_X3_REPEAT_APRIL_16_2014_0848AM)
#define SPIRIT_X3_REPEAT_APRIL_16_2014_0848AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>

namespace boost { namespace spirit { namespace x3
{
    template<typename Subject, typename RepeatIterator>
    struct repeat_directive : unary_parser<Subject, repeat_directive<Subject,RepeatIterator>>
    {
        typedef unary_parser<Subject, repeat_directive<Subject,RepeatIterator>> base_type;
        typedef std::function<bool(RepeatIterator const)> range_check_function;
        static bool const is_pass_through_unary = true;
        static bool const handles_container = true;

        repeat_directive(Subject const& subject, range_check_function min_check, range_check_function max_check) :
            base_type(subject),
            got_min(min_check),
            got_max(max_check)
            {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {

            RepeatIterator i{};
            for (/**/; !got_min(i); ++i)
            {
                if (!detail::parse_into_container(
                      this->subject, first, last, context, attr))
                   return false;
            }

            // parse some more up to the maximum specified
            Iterator save = first;
            for (/**/; !got_max(i); ++i)
            {
                if (!detail::parse_into_container(
                      this->subject, first, last, context, attr))
                    break;
                save = first;
            }

            first = save;
            return true;
        }

        range_check_function got_min;
        range_check_function got_max;
    };

    // Infinite loop tag type
    struct inf_type {};
    const inf_type inf = inf_type();

    struct repeat_gen
    {
        template<typename Subject>
        kleene<typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return {as_parser(subject)};
        }
        template <typename RepeatIterator> 
        struct repeat_gen_lvl1
        {
                typedef std::function<bool(RepeatIterator const)> range_check_function;
                repeat_gen_lvl1(range_check_function const& min_check, range_check_function const& max_check) 
                        : min_check(min_check), max_check(max_check)
                {}
                
                template<typename Subject>
                repeat_directive< typename extension::as_parser<Subject>::value_type, RepeatIterator>
                operator[](Subject const& subject) const
                {
                        return {as_parser(subject),min_check,max_check};
                }
                range_check_function min_check;
                range_check_function max_check;
        };

        template <typename T>
        repeat_gen_lvl1< T >
        operator()(T const exact) const
        {
           return {[=](T const i) { return i>=exact;},[=](T const i) { return i>=exact;}};
        }

        template <typename T>
        repeat_gen_lvl1< T >
        operator()(T const min_val, T const max_val) const
        {
           return {[=](T const i) { return i>=min_val;},[=](T const i) { return i>=max_val;}};
        }
        
        template <typename T>
        repeat_gen_lvl1< T >
        operator()(T const min_val, inf_type const &) const
        {
           return {[=](T const i) { return i>=min_val;},[=](T const i) { return false;}};
        }
    };

    repeat_gen const repeat = repeat_gen();
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename Subject, typename RepeatIterator, typename Context>
    struct attribute_of<x3::repeat_directive<Subject,RepeatIterator>, Context>
        : build_container<
            typename attribute_of<Subject, Context>::type> {};
}}}}


#endif
