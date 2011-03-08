/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM)
#define SPIRIT_KEYWORDS_DETAIL_MARCH_13_2007_1145PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace repository { namespace qi { namespace detail {
        

        // Variant visitor class which handles dispatching the parsing to the selected parser
        // This also handles passing the correct attributes and flags/counters to the subject parsers       
 
        template < typename Elements, typename Iterator ,typename Context ,typename Skipper
                  ,typename Flags ,typename Counters ,typename Attribute, typename NoCasePass>
        class parse_dispatcher
            : public boost::static_visitor<bool>
        {
            public:
            parse_dispatcher(const Elements &elements,Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Flags &flags, Counters &counters, Attribute& attr) : 
                 elements(elements), first(first), last(last)
               , context(context), skipper(skipper)
               , flags(flags),counters(counters), attr(attr)
            {}
            
            template<typename T> bool operator()(T& idx) const
            {    
                return call(idx,typename traits::not_is_unused<Attribute>::type());
                
            }
            
            template <typename Subject,typename Index> 
            bool call_subject_unused(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,unused, NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject_unused(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename traits::make_attribute<attr_type, unused_type> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(unused);
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;
                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            template <typename Subject,typename Index> 
            bool call_subject(
                  Subject const &subject, Iterator &first, Iterator const &last
                , Context& context, Skipper const& skipper
                , Index& idx ) const
            {
                Iterator save = first;
                if(subject.kwd_loop_parse(first,last,context,skipper,fusion::at_c<Index::value>(attr), NoCasePass::value ))
                {
                    if(subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                        return true;
                }
                save = save;
                return false;
            }            
 
            template <typename Subject, typename Index, typename Action> bool call_subject(
                  spirit::qi::action<Subject, Action> const &subject, Iterator &first
                , Iterator const &last, Context& context, Skipper const& skipper
                , Index &idx) const
            {

                typedef typename Subject::template attribute<Context, Iterator>::type attr_type;
                typedef typename fusion::result_of::value_at<Attribute,typename Index::value>::type subject_attribute;
                typedef typename traits::make_attribute<attr_type, subject_attribute> make_attribute;

                // create an attribute if one is not supplied
                typedef traits::transform_attribute<
                typename make_attribute::type, attr_type, spirit::qi::domain> transform;

                typename make_attribute::type made_attr = make_attribute::call(fusion::at_c<Index::value>(attr));
                typename transform::type local_attr = transform::pre(made_attr);

                Iterator save = first;
                if (subject.subject.kwd_loop_parse(first, last, context, skipper, local_attr, NoCasePass::value ))
                {
                    if(subject.subject.iter.register_successful_parse(flags[Index::value],counters[Index::value]))
                    {
                        // call the function, passing the attribute, the context.
                        // The client can return false to fail parsing.
                        if (traits::action_dispatch<Subject>()(subject.f, local_attr, context))
                            return true;

                    }
                }
                // reset iterators if semantic action failed the match 
                // retrospectively
                first = save;
                return false;
            }            
            
            // Handle unused attributes
            template <typename T> bool call(T &idx, mpl::false_) const{
                
                //if( fusion::at_c<T::value>(elements).parse_impl( first, last, context, skipper, unused, mpl::false_() ) )
                return call_subject_unused(fusion::at_c<T::value>(elements), first, last, context, skipper, idx );
            }
            // Handle normal attributes
            template <typename T> bool call(T &idx, mpl::true_) const{
                return call_subject(fusion::at_c<T::value>(elements), first, last, context, skipper, idx);
            }
            
            const Elements &elements;
            Iterator &first;
            const Iterator &last;
            Context & context;
            const Skipper &skipper;
            Flags &flags;
            Counters &counters;
            Attribute &attr;
        };
      
}}}}}

#endif

