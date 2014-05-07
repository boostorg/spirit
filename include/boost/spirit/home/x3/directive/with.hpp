/*=============================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_X3_WITH_MAY_02_2014_0749AM)
#define SPIRIT_X3_WITH_MAY_02_2014_0749AM

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    // with directive injects a value into the context prior to parsing.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename ID, typename T>
    struct with_directive : unary_parser<Subject, with_directive<Subject, ID, T>>
    {
        typedef unary_parser<Subject, with_directive<Subject, ID, T> > base_type;
        static bool const is_pass_through_unary = true;
        static bool const handles_container = Subject::handles_container;

        typedef Subject subject_type;
        T val;

        with_directive(Subject const& subject, T const& val)
          : base_type(subject)
          , val(val) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return this->subject.parse(
                first, last
              , make_context<ID>(val, context)
              , attr);
        }
    };

    template <typename ID, typename T>
    struct with_gen
    {
        T val;

        with_gen(T const& val)
          : val(val) {}

        template <typename Subject>
        with_directive<typename extension::as_parser<Subject>::value_type, ID, T>
        operator[](Subject const& subject) const
        {
            return {as_parser(subject), val};
        }
    };

    template <typename ID, typename T>
    inline with_gen<ID, T> with(T const& val)
    {
        return {val};
    }
}}}

#endif
