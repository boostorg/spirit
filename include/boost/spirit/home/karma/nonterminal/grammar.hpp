//  Copyright (c) 2001-2007 Joel de Guzman
//  Copyright (c) 2001-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_GRAMMAR_MAR_05_2007_0542PM)
#define BOOST_SPIRIT_KARMA_GRAMMAR_MAR_05_2007_0542PM

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/nonterminal/nonterminal.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/nonterminal/rule.hpp>
#include <boost/spirit/home/karma/nonterminal/nonterminal_director.hpp>
#include <boost/function_types/is_function.hpp>
#include <boost/noncopyable.hpp>

namespace boost { namespace spirit { namespace karma
{
    template <typename Definition>
    struct grammar
      : nonterminal<
            grammar<Definition>, 
            typename Definition::sig_type,
            typename Definition::locals_type
        >
    {
        typedef typename Definition::sig_type sig_type;
        typedef typename Definition::locals_type locals_type;
        typedef typename Definition::delimiter_type delimiter_type;
        typedef typename Definition::start_type start_type;
        typedef typename Definition::iterator_type iterator_type;

        explicit grammar(Definition const& definition_)
          : definition(definition_), start(definition_.start)
        {
        }

        template <typename Definition_, typename Start>
        grammar(Definition_ const& definition_, Start const& start_)
          : definition(definition_), start(start_)
        {
        }

    private:
        template <typename OutputIterator, typename Context, typename Delimiter>
        bool generate(OutputIterator& sink, Context& context, 
            Delimiter const& delim) const
        {
            return start.generate(sink, context, delim);
        }

        std::string what() const
        {
            if (definition.name().empty())
            {
                return start.what();
            }
            else
            {
                return definition.name();
            }
        }

        friend struct nonterminal_director;
        Definition const& definition;
        start_type const& start;
    };

    template <typename OutputIterator, typename T0 = unused_type,
        typename T1 = unused_type, typename T2 = unused_type>
    struct grammar_def : noncopyable
    {
        typedef karma::rule<OutputIterator, T0, T1, T2> start_type;
        typedef typename start_type::iterator_type iterator_type;
        typedef typename start_type::sig_type sig_type;
        typedef typename start_type::locals_type locals_type;
        typedef typename start_type::delimiter_type delimiter_type;

        grammar_def(std::string const& name_ = std::string())
          : name_(name_) {}

        std::string name() const
        {
            return name_;
        }

        void name(std::string const& name__)
        {
            name_ = name__;
        }

        std::string name_;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Generator functions helping to construct a proper grammar object 
    //  instance
    ///////////////////////////////////////////////////////////////////////////
    template <typename Definition>
    inline grammar<Definition> 
    make_generator(Definition const& def)
    {
        return grammar<Definition>(def);
    }
    
    template <typename Definition, typename Start>
    inline grammar<Definition> 
    make_generator(Definition const& def, Start const& start)
    {
        return grammar<Definition>(def, start);
    }
    
}}}

#endif
