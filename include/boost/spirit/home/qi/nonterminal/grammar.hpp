/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_GRAMMAR_FEB_19_2007_0236PM)
#define BOOST_SPIRIT_GRAMMAR_FEB_19_2007_0236PM

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/qi/nonterminal/nonterminal.hpp>
#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/nonterminal_director.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost { namespace spirit { namespace qi
{
    template <typename Definition>
    struct grammar
      : nonterminal<
            grammar<Definition>
          , typename Definition::sig_type
          , typename Definition::locals_type
        >, noncopyable
    {
        typedef typename Definition::sig_type sig_type;
        typedef typename Definition::locals_type locals_type;
        typedef typename Definition::skipper_type skipper_type;
        typedef typename Definition::start_type start_type;
        typedef typename Definition::iterator_type iterator_type;

        template <typename Definition_>
        explicit grammar(Definition_ const& definition)
          : definition(definition)
          , start(definition.start)
        {
        }

        template <typename Definition_, typename Start>
        grammar(Definition_ const& definition, Start const& start)
          : definition(definition)
          , start(start)
        {
        }

    private:

        template <typename Iterator, typename Context, typename Skipper>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper) const
        {
            return start.parse(first, last, context, skipper);
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

    template <
        typename Iterator
      , typename T0 = unused_type
      , typename T1 = unused_type
      , typename T2 = unused_type
    >
    struct grammar_def : noncopyable
    {
        typedef Iterator iterator_type;
        typedef qi::rule<Iterator, T0, T1, T2> start_type;
        typedef typename start_type::sig_type sig_type;
        typedef typename start_type::locals_type locals_type;
        typedef typename start_type::skipper_type skipper_type;

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

    template <
        typename Iterator
      , typename T0 = unused_type
      , typename T1 = unused_type
      , typename T2 = unused_type
    >
    struct grammar_
      : nonterminal<
            grammar_<Iterator, T0, T1, T2>
          , typename qi::rule<Iterator, T0, T1, T2>::sig_type
          , typename qi::rule<Iterator, T0, T1, T2>::locals_type
        >, noncopyable
    {
        typedef Iterator iterator_type;
        typedef qi::rule<Iterator, T0, T1, T2> start_type;
        typedef typename start_type::sig_type sig_type;
        typedef typename start_type::locals_type locals_type;
        typedef typename start_type::skipper_type skipper_type;
        typedef grammar_<Iterator, T0, T1, T2> base_type;

        grammar_(start_type const& start, std::string const& name_ = std::string())
          : start(start), name_(name_) {}

        std::string name() const
        {
            return name_;
        }

        void name(std::string const& name__)
        {
            name_ = name__;
        }

        std::string name_;
        start_type const& start;

    private:

        template <typename Iterator_, typename Context, typename Skipper>
        bool parse(
            Iterator_& first, Iterator_ const& last
          , Context& context, Skipper const& skipper) const
        {
            return start.parse(first, last, context, skipper);
        }

        std::string what() const
        {
            if (name().empty())
            {
                return start.what();
            }
            else
            {
                return name();
            }
        }

        friend struct nonterminal_director;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The grammar_class template
    ///////////////////////////////////////////////////////////////////////////
    template <template <typename, typename> class Def>
    struct grammar_class {};

    template <typename Iterator, template <typename, typename> class Def>
    inline bool
    parse(
        Iterator& first
      , Iterator last
      , grammar_class<Def>& gc)
    {
        Def<Iterator, unused_type> def(gc);
        grammar<Def<Iterator, unused_type> > g(def);
        return parse(first, last, g);
    }

    template <typename Iterator
      , template <typename, typename> class Def, typename Attr>
    inline bool
    parse(
        Iterator& first
      , Iterator last
      , grammar_class<Def>& gc
      , Attr& attr)
    {
        Def<Iterator, unused_type> def(gc);
        grammar<Def<Iterator, unused_type> > g(def);
        return parse(first, last, g, attr);
    }

    template <typename Iterator
      , template <typename, typename> class Def, typename Skipper>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , grammar_class<Def>& gc
      , Skipper const& skipper_)
    {
        typedef typename
            result_of::as_component<qi::domain, Skipper>::type
        skipper_type;

        skipper_type skipper = spirit::as_component(qi::domain(), skipper_);

        Def<Iterator, skipper_type> def(gc);
        grammar<Def<Iterator, skipper_type> > g(def);
        return phrase_parse(first, last, g, skipper);
    }

    template <typename Iterator
      , template <typename, typename> class Def, typename Attr, typename Skipper>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , grammar_class<Def>& gc
      , Attr& attr
      , Skipper const& skipper_)
    {
        typedef typename
            result_of::as_component<qi::domain, Skipper>::type
        skipper_type;

        skipper_type skipper = spirit::as_component(qi::domain(), skipper_);

        Def<Iterator, skipper_type> def(gc);
        grammar<Def<Iterator, skipper_type> > g(def);
        return phrase_parse(first, last, g, attr, skipper);
    }

}}}

#endif
