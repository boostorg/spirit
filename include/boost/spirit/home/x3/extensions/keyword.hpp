/*=============================================================================
  Copyright (c) 2014 Thomas Benard

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
  ==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_KEYWORD_NOV_29_2013)
#define BOOST_SPIRIT_X3_KEYWORD_NOV_29_2013

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/string/detail/string_parse.hpp>
#include <boost/spirit/home/x3/support/utility/utf8.hpp>
#include <boost/spirit/home/x3/support/utility/unrefcv.hpp>
#include <boost/spirit/home/x3/support/utility/integer_sequence.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>
#include <boost/spirit/home/support/char_encoding/standard.hpp>
#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

#include <boost/array.hpp>

#include <string>

namespace boost { namespace spirit { namespace x3 { namespace detail {

    // Helper to filter out unused attributes
    template <int... Is>
        struct attribute_index {};

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq;

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, bool HasAttr, int ... Is>
        struct gen_attribute_seq_detail : 
            gen_attribute_seq< Context, Keywords, AttrIdx+1, ParserIdx +1, N-1, Is ..., AttrIdx> {};

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq_detail<Context, Keywords, AttrIdx, ParserIdx, N, false, Is...> :
            gen_attribute_seq< Context, Keywords, AttrIdx, ParserIdx +1, N-1, Is..., -1 > {};

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int N, int ... Is>
        struct gen_attribute_seq : 
            gen_attribute_seq_detail<Context, Keywords, AttrIdx, ParserIdx, N, 
                traits::has_attribute< typename std::tuple_element<ParserIdx, Keywords>::type, Context >::value, Is...
                > {};

    template <typename Context, typename Keywords, int AttrIdx, int ParserIdx, int ... Is>
        struct gen_attribute_seq<Context, Keywords, AttrIdx, ParserIdx, 0, Is...> : 
            attribute_index<Is...> {};

    template <int N, int... Is>
        struct gen_unused_attribute_seq : gen_unused_attribute_seq<N - 1, - 1, Is...> {};

    template <int... Is>
        struct gen_unused_attribute_seq<0, Is...> : attribute_index<Is...> {};

    // Helper to get the attribute at a given index
    template <typename Attribute, int Index>
        struct get_attribute
        {
            typedef typename fusion::result_of::at_c<Attribute, Index>::type type;
            static typename add_reference<type>::type call(Attribute &s)
            {
                return fusion::at_c<Index>(s);
            }

        };

    template <typename Attribute>
        struct get_attribute<Attribute, -1 >
        {
            static unused_type& call(Attribute &s)
            {
                static unused_type result;
                return result;
            }
        };
    // Helper to add the keyword strings to the lookup map
    struct keyword_string
    {
        template <typename Lookup, typename Keyword>
            static void add(Lookup &lookup, Keyword const &parser, int index)
            {
                lookup.add(traits::get_string_begin<char>(parser.key.str),
                        traits::get_string_end<char>(parser.key.str),
                        index);
            }
        template <typename Lookup, typename Keyword, typename Action>
            static void add(Lookup &lookup, action<Keyword, Action> const &parser, int index)
            {
                lookup.add(traits::get_string_begin<char>(parser.subject.key.str),
                        traits::get_string_end<char>(parser.subject.key.str),
                        index);
            }
    };
    
    // Occurence checking classes 


    struct kwd_ignore_count // handles kwd[p]
    {
        bool flag_init() const { return true; }
        template <typename T>
        bool register_successful_parse(bool &flag, T &i) const {
            flag=true;
            return true;
        }
    };

    template <typename T>
    struct kwd_exact_count // handles kwd(exact)[p]
    {
        typedef T type;
        bool flag_init() const { return false; }
        bool register_successful_parse(bool &flag, T &i) const {
            i++;
            if(i<exact)
            {
                flag=false;
                return true;
            }
            else if(i==exact)
            {
                flag=true;
                return true;
            }
            else
                return flag=false;

        }
        T const exact;
    };

    template <typename T>
    struct kwd_finite_count // handles kwd(min, max)[p]
    {
        typedef T type;
        bool flag_init() const { return min_value==0; }
        bool register_successful_parse(bool &flag, T &i) const {
            i++;
            if(i<min_value)
            {
                flag=false;
                return true;
            }
            else if(i>=min_value && i<=max_value)
            {
                return flag=true;
            }
            else
                return flag=false;
        }
        T const min_value;
        T const max_value;
    };

//~    template <typename T>
//~    struct kwd_infinite_count // handles kwd(min, inf)[p]
//~    {
//~
//~        typedef T type;
//~        bool flag_init() const { return min_value==0; }
//~        bool register_successful_parse(bool &flag, T &i) const {
//~            i++;
//~            flag = i>=min_value;
//~            return true;
//~        }
//~        T const min_value;
//~    };


}}}}


namespace boost { namespace spirit { namespace x3 {

    // simple keyword parser
    template <typename Key, typename RepeatCountLimit, typename Subject>
        struct keyword_parser : unary_parser<Subject, keyword_parser<Key, RepeatCountLimit, Subject> >
    {
        typedef unary_parser<Subject, keyword_parser<Key, RepeatCountLimit, Subject> > base_type;

        static const bool is_pass_through_unary =true;

        keyword_parser(Key const &key, RepeatCountLimit const &repeat_limit, Subject const& subject)
            : base_type(subject)
            , key(key)
            , repeat_limit(repeat_limit)
        {}

        template <typename Iterator, typename Context, typename RContext, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
                    , Context const& context, RContext &rcontext, Attribute& attr) const
            {
                bool flag = repeat_limit.flag_init();
                int counter {};
                Iterator save = first;
                if (key.parse(first, last, context, rcontext, unused))
                {
                    if(this->subject.parse(first, last, context, rcontext, attr))
                    {
                        return repeat_limit.register_successful_parse(flag, counter);
                    }
                }
                first = save;
                return flag;
            }

        template <typename Iterator, typename Context, typename RContext, typename Attribute>
            bool parse_subject(Iterator& first, Iterator const& last
                    , Context const& context, RContext & rcontext
                    , bool &flag, int &counter, Attribute& attr) const
            {
                if(this->subject.parse(first, last, context, rcontext, attr))
                    return repeat_limit.register_successful_parse(flag, counter);
                return false;
            }
 
        Key key;
        RepeatCountLimit repeat_limit;
    };
    // Two stage generator for the kwd parser / directive

    template <typename Key, typename RepeatCountLimit>
        struct kwd_level_1
        {
            template <typename Subject>
                keyword_parser< 
                       Key
                      , RepeatCountLimit
                      , typename extension::as_parser<Subject>::value_type 
                              >
                operator[](Subject const& subject) const
                {
                    return {key, repeat_limit, as_parser(subject)};
                }
            Key key;
            RepeatCountLimit repeat_limit;
        };

    // Handle kwd("key")[p]
    template <typename Key>
        kwd_level_1<typename extension::as_parser<Key>::value_type, detail::kwd_ignore_count >
        kwd(Key const &key) 
        {
            return {as_parser(key), detail::kwd_ignore_count()};
        }

    // Handle kwd("key", exact)[p]
    template <typename Key, typename T>
        kwd_level_1<typename extension::as_parser<Key>::value_type, detail::kwd_exact_count<T> >
        kwd(Key const &key, T const exact)
        {
            return {as_parser(key), detail::kwd_exact_count<T>{exact}};
        }
    // Handle kwd("key", min, max)[p]
    template <typename Key, typename T>
        kwd_level_1<typename extension::as_parser<Key>::value_type, detail::kwd_finite_count<T> >
        kwd(Key const &key, T const min_value, T const max_value)
        {
            return {as_parser(key), detail::kwd_finite_count<T>{min_value, max_value}};
        }

    // Handle kwd("key", min, max)[p]
//~    template <typename Key, typename T>
//~        kwd_level_1<typename extension::as_parser<Key>::value_type, detail::kwd_infinite_count<T> >
//~        kwd(Key const &key, T const min_value, inf const )
//~        {
//~            return {as_parser(key), detail::kwd_infinite_count<T>{min_value}};
//~        }

    // keywords parser 

    template <typename ...Keywords> 
        struct keywords_parser : parser< keywords_parser< Keywords ...> >
    {             

        typedef parser< keywords_parser< Keywords...> > base_type;

        static const std::size_t nb_keywords = sizeof...(Keywords);

        // build a bool array and an integer array which will be used to
        // check that the repetition constraints of the kwd parsers are
        // met and bail out a soon as possible
        typedef boost::array<bool, nb_keywords> flags_type;
        typedef boost::array<int, nb_keywords> counters_type;


        keywords_parser(Keywords ... keywords)
            : keywords(std::forward<Keywords>(keywords)...)
        {
            add_keywords(0, keywords...);
        }

        // construct the lookup table
        void add_keywords(int index)
        {
        }

        template <typename keywordType, typename ... rest>
            void add_keywords(int index, keywordType const &keyword, rest const&... keywords)
            {
                detail::keyword_string::add(lookup, keyword, index);
                flags_init[index]=keyword.repeat_limit.flag_init();
                add_keywords(index+1, keywords ...);
            }

        // Subject parser calling function
        template <int ParserIdx, typename GetAttribute, typename Iterator, typename Context, typename RContext, typename Attribute>
            bool parse_subject(Iterator& first, Iterator const& last
                    , Context const& context, RContext &rcontext, bool &flag, int & counter, Attribute& attr) const
            {
                return std::get<ParserIdx>(keywords).parse_subject(first, last, context, rcontext, flag, counter, GetAttribute::call(attr));
            }
        // Internal dispatching function used to call the correct parser depending on the keyword
        template <typename Iterator, typename Context, typename RContext, typename Attribute, std::size_t... Is, int... As>
            bool parse_internal(Iterator& first, Iterator const& last
                    , Context const& context, RContext &rcontext, Attribute& attr, index_sequence<Is...>, detail::attribute_index<As...> ) const
            {
                typedef bool (keywords_parser::* subject_caller) (Iterator &, Iterator const&, Context const&, RContext &, bool &, int &, Attribute &) const;
                static subject_caller parse_functions[nb_keywords] =
                {
                    &keywords_parser::parse_subject<Is, detail::get_attribute<Attribute, As>, Iterator, Context, RContext, Attribute>...
                };
               
                flags_type flags(flags_init);
                counters_type counters {};
                Iterator save = first;
                Iterator local_save = save;
                x3::skip_over(first, last, context);
                while(std::size_t *subparser_idx =
                        lookup.find(first, last, tst_pass_through()))
                {
                    // Call through a the correct function pointer the subject parser
                    if( 
                       !(
                         (this->*parse_functions[*subparser_idx])
                                                                (
                                                                 first
                                                                 , last
                                                                 , context
                                                                 , rcontext
                                                                 , flags[*subparser_idx]
                                                                 , counters[*subparser_idx]
                                                                 , attr)
                        )
                      )
                    {
                        break;
                    }
                    local_save = first;
                    x3::skip_over(first, last, context);
                }
                // Reset to the last successufull parse position inside the keyword set
                first = local_save;
                for(bool flag : flags)
                {
                   if(!flag)
                   {
                       first = save;
                       return false;
                   }
                }
                return true;
            }

        // Parser entry
        template <typename Iterator, typename Context, typename RContext, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
                    , Context const& context, RContext &rcontext, Attribute& attr) const
            {
                return parse_internal(
                        first, last, context, rcontext, attr,
                        make_index_sequence<nb_keywords>{},
                        typename std::conditional< 
                        std::is_same< typename std::remove_const<Attribute>::type, unused_type >::value, 
                        detail::gen_unused_attribute_seq<nb_keywords>,
                        detail::gen_attribute_seq<Context, std::tuple<Keywords...>, 0, 0, nb_keywords> 
                        >::type{}
                        );
            }

        tst<char, std::size_t> lookup;
        flags_type flags_init;
        std::tuple<Keywords ...> keywords;
    };

    template <typename ... Keywords>
        keywords_parser<Keywords...>keywords (Keywords ... keyword_list) 
        {
            return {std::forward<Keywords>(keyword_list)...};
        }
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename Key, typename RepeatCountLimit, typename Subject, typename Context>
        struct attribute_of<x3::keyword_parser<Key, RepeatCountLimit, Subject>, Context>
        : attribute_of<Subject, Context> {};

}}}}


#endif
