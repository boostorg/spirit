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

            std::cout<<"Start"<<std::endl;
            RepeatIterator i{};
            for (/**/; !got_min(i); ++i)
            {
                std::cout<<"Debug "<<i<<std::endl;
                if (!detail::parse_into_container(
                      this->subject, first, last, context, attr))
                   return false;
            }

            // parse some more up to the maximum specified
            Iterator save = first;
            for (/**/; !got_max(i); ++i)
            {
                std::cout<<"Debug 2 "<<i<<std::endl;
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
#if 0
/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_X3_REPEAT_APRIL_16_2014_0848AM)
#define SPIRIT_X3_REPEAT_APRIL_16_2014_0848AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>

namespace boost { namespace spirit { namespace x3
{
        // silence MSVC warning C4512: assignment operator could not be generated
        infinite_iterator& operator= (infinite_iterator const&);
    };

    template <typename Subject, typename LoopIter>
    struct repeat_parser : unary_parser<repeat_parser<Subject, LoopIter> >
    {
        typedef Subject subject_type;

        template <typename Context, typename Iterator>
        struct attribute
        {
            // Build a std::vector from the subject's attribute. Note
            // that build_std_vector may return unused_type if the
            // subject's attribute is an unused_type.
            typedef typename
                traits::build_std_vector<
                    typename traits::attribute_of<
                        Subject, Context, Iterator>::type
                >::type
            type;
        };

        repeat_parser(Subject const& subject_, LoopIter const& iter_)
          : subject(subject_), iter(iter_) {}

        template <typename F>
        bool parse_container(F f) const
        {
            typename LoopIter::type i = iter.start();
            for (/**/; !iter.got_min(i); ++i)
            {
                if (f (subject))
                    return false;
            }

            // parse some more up to the maximum specified
            typename F::iterator_type save = f.f.first;
            for (/**/; !iter.got_max(i); ++i)
            {
                if (f (subject))
                    break;
                save = f.f.first;
            }

            f.f.first = save;
            return true;
        }

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr_) const
        {
            typedef detail::fail_function<Iterator, Context, Skipper>
                fail_function;

            // ensure the attribute is actually a container type
            traits::make_container(attr_);

            Iterator iter_local = first;
            fail_function f(iter_local, last, context, skipper);
            if (!parse_container(detail::make_pass_container(f, attr_)))
                return false;

            first = f.first;
            return true;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("repeat", subject.what(context));
        }

        Subject subject;
        LoopIter iter;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        repeat_parser& operator= (repeat_parser const&);
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::repeat, Subject, Modifiers>
    {
        typedef kleene<Subject> result_type;
        result_type operator()(unused_type, Subject const& subject, unused_type) const
        {
            return result_type(subject);
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat, fusion::vector1<T> >, Subject, Modifiers>
    {
        typedef exact_iterator<T> iterator_type;
        typedef repeat_parser<Subject, iterator_type> result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject, fusion::at_c<0>(term.args));
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat, fusion::vector2<T, T> >, Subject, Modifiers>
    {
        typedef finite_iterator<T> iterator_type;
        typedef repeat_parser<Subject, iterator_type> result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject,
                iterator_type(
                    fusion::at_c<0>(term.args)
                  , fusion::at_c<1>(term.args)
                )
            );
        }
    };

    template <typename T, typename Subject, typename Modifiers>
    struct make_directive<
        terminal_ex<tag::repeat
        , fusion::vector2<T, inf_type> >, Subject, Modifiers>
    {
        typedef infinite_iterator<T> iterator_type;
        typedef repeat_parser<Subject, iterator_type> result_type;

        template <typename Terminal>
        result_type operator()(
            Terminal const& term, Subject const& subject, unused_type) const
        {
            return result_type(subject, fusion::at_c<0>(term.args));
        }
    };
}}}
#endif
#endif
