#ifndef WORKBENCH_Y3_SCANNER_HPP_INCLUDED
#define WORKBENCH_Y3_SCANNER_HPP_INCLUDED
//Purpose:
//  Explore alternative to putting skipper in 
//  'context' argument to parse function.
//  The alternive places skipper in the 
//  'first' (the iterator) argument to
//  parse function.
//AlternativeCons:
//  Breaks existing code by replacing the signature
//  of all parse functions to take a range iterator
//  instead of a pair of iterators named, first and last.
//AlternativePros:
//  The advantages of putting the skipper into
//  a range iterator argument are:
//    * Modularity:
//      The wiki article:
//        https://en.wikipedia.org/wiki/Modular_programming
//      says:
/*
Modular programming is a software design technique that
emphasizes separating the functionality of a program into
independent, interchangeable modules, such that each
contains everything necessary to execute only one aspect of
the desired functionality.
 */
//       The parse's iterator argument is supposed to return
//       the next "token" to be analyzed by the parse function.
//       If those tokens are to be filtered, then aggregating
//       the filter into the iterator means the program
//       is more modular.
//
//       In addition, instead of 2 iterator arguments to the
//       parse function, the 2 iterators are consolidatee into
//       a single argument, the argument with type, scanner<Skipper>,
//       which emphasizes (just a little more) their connection.
//    * Simplicity:
//       Examining the code below delimited by
//         #ifdef USE_SCAN_SKIP...#else
//       as compared to that delimited by the corresponding
//         #else...#endif//USE_SCAN_SKIP
//       shows much code is eliminated if defined(USE_SCAN_SKIP).
//       In particular, the code here:
/*
https://github.com/think-cell/spirit/blob/673c67949981d40115ab4ddda7d54b614994cc30/include/boost/spirit/home/x3/support/context.hpp
 */
//       involves somewhat involved metaprogramming.  In addition,
//       there's 2 overload's of the skip_over function and, to
//       make these work, there's 2 more in the detail namespace. 
//
//       In contrast, the code below involves just a call to the
//       scanner<Skipper>.skip member function and, at most, 
//       1 overload skip_over function.
//
//       OTOH, the USE_SCAN_SKIP code does require the
//       scanner<Skipper> struct with it's member templates:
//         rebind_skipper<OtherSkipper>
//         skip<Context>
//       however, that involves no metaprogramming and 
//       is straightforward.
//    * Compile-time complexity:
//       Because of less metaprogramming (in particular, that here:
/*
https://github.com/think-cell/spirit/blob/673c67949981d40115ab4ddda7d54b614994cc30/include/boost/spirit/home/x3/support/context.hpp#L60
 */      
//       ) it's likely *slightly* faster.
//    * Run-time complexity:
//       The skip_directive<Subject,Skipper>::parse function
//       calls make_context, the recursive remove here:
/*
https://github.com/think-cell/spirit/blob/673c67949981d40115ab4ddda7d54b614994cc30/include/boost/spirit/home/x3/support/context.hpp#L64
 */
//       makes repeated calls to the context CTOR.
//       This causes repeated copies of the 2 reference
//       components of context here:
/*
https://github.com/think-cell/spirit/blob/673c67949981d40115ab4ddda7d54b614994cc30/include/boost/spirit/home/x3/support/context.hpp#L19
 */
//       Hence, the run-time complexity is 2*the number of
//       context CTOR calls.  Of course the number of
//       context CTOR calls will probably be small, but it
//       could matter, at least a little.
//
//       In contrast, when defined(USE_SCAN_SKIP), 1 call
//       is made to scanner<SkipperOld>::rebind_skipper(skipper)
//       which copies a reference to skipper arg and the input_iter
//       superclass part of scanner<SkipperOld>.
//
//===============
#include "../trace_scope.hpp"
#include <sstream>
#include <boost/core/demangle.hpp>
#include <typeinfo>
#include <string>
  template
  < typename Type
  >
  std::string
type_name()
  {
    return boost::core::demangle(typeid(Type).name());
  }

#include <cctype>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/declval.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/spirit/home/x3/support/context.hpp>

#include "input_iter.hpp"

namespace x3=boost::spirit::x3;
    
namespace y3
{
    using namespace x3;
    using namespace boost;
    
  #define REPORT_CONTEXT
  #ifdef REPORT_CONTEXT
      template <typename Context>
      std::string
    ctx_str(Context const& ctx)
      { return type_name<Context>();
      }
  #endif//REPORT_CONTEXT
    
    //{parser
    //  The following simplified parser from:
    //    x3/core/parser.hpp
    template <typename Derived>
    struct parser
    {
        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };
    
    template< typename ParserMaybe>
    struct is_parser
      : std::is_base_of<parser<ParserMaybe>,ParserMaybe>
      {};
    //}parser
    
    struct char_class_space
      ;
      template
      < typename Skipper=char_class_space
      >
    struct scanner
      ;
  //#define USE_SCAN_SKIP
  #ifdef USE_SCAN_SKIP
    struct fail_parser
      : parser<fail_parser>
      {
            template
            < typename Skipper
            , typename Context
            >
          inline bool parse( scanner< Skipper>&, Context const&)const
          {  return false;
          }
      };
    auto const fail = fail_parser{};

      template
      < typename SkipperThis
      >
    struct scanner
      : public input_iter
      {
            explicit
          scanner
            ( input_stream_t const&inp_strm
            , SkipperThis const&skip
            )
            : input_iter(inp_strm)
            , _skip(skip)
            {
            }
            template
            < typename SkipperOther
            >
            explicit 
          scanner
            ( scanner<SkipperOther>&scan
            , SkipperThis const&skip
            )
            : input_iter(scan)
            , _skip(skip)
            {}
            template
            < typename SkipperOther
            >
            scanner&
          operator=(scanner<SkipperOther>& inp)
            { input_iter::operator=(inp);
              return *this;
            }
            template< typename SkipperOther>
            auto
          rebind_skipper(SkipperOther const&s)
            {
              scanner&self_v=*this;
              return scanner< SkipperOther>{self_v,s};
            }
            template<typename Context>
            void
          skip( Context const& ctx)
            {
              auto new_first=this->rebind_skipper(fail);//no skipping in skip_.
              while(!at_end() && _skip.parse( new_first, ctx))
              {  //nothing
              }
              this->operator=(new_first);
            }
       private:
            SkipperThis const&
          _skip
            ;
      };
      
    template< typename Skipper, typename Context>
    inline void skip_over( scanner<Skipper>& first, Context const& context)
    {
    #ifdef USE_TRACING
      trace_scope ts("skip_over(Skipper)");
      std::cout<<":first.enter="<<first<<"\n";
    #endif//USE_TRACING  
      first.skip(context);
    #ifdef USE_TRACING
      std::cout<<":first.exit ="<<first<<"\n";
    #endif//USE_TRACING  
    }
  #if 1
    //This not really needed for correct execution because when first.skip(context)
    //in 1st skip_over is called, it immediately fails without moving first.
    //IOW, it does nothing, which is exactly what the following overload does.
    //However, it *may* run a little faster since with this overload,
    //there's no call to first.skip(context).
    template< typename Context>
    inline void skip_over( scanner<fail_parser>&, Context const&)
    {
    #ifdef USE_TRACING
      std::cout<<"skip_over(Nothing)\n";
    #endif//USE_TRACING  
    }
  #endif
  #else
      template
      < typename Skipper
        //The above is unused.  It's only
        //purpose is to allow compilation
        //when !defined(USE_SCAN_SKIP).
        //
        //With this scanner, Instead of storing 
        //Skipper within the scanner(as when defined(USE_SCAN_SKIP),
        //it's stored in the Context argument to the parse functions.
      >
    struct scanner
      : public input_iter
      {
            using input_iter::
          input_iter
            ;
            using input_iter::
          operator=
            ;
      };
    //{skip_over
    //  Following code modified from:
    //    x3/core/skip_over.hpp
    namespace detail
    {
          template 
          < typename SkipperIgnore
          , typename SkipperContext
          >
        inline void skip_over( scanner<SkipperIgnore>& first, SkipperContext const& skipper)
        //skipper=get<skipper_tag>(context) 
        //in y3::skip_over(first,context) below.
        {
          #ifdef USE_TRACING
            trace_scope ts("skip_over(Skipper)");
            std::cout<<":first.enter="<<first<<"\n";
          #endif//USE_TRACING  
            while (!first.at_end() && skipper.parse( first, unused))
                /***/;
          #ifdef USE_TRACING
            std::cout<<":first.exit ="<<first<<"\n";
          #endif//USE_TRACING  
        }

          template 
          < typename SkipperIgnore
          >
        inline void skip_over( scanner<SkipperIgnore>&, unused_type const& skipper)
        //skipper=get<skipper_tag>(context) 
        //in y3::skip_over(first,context) below.
        {
          #ifdef USE_TRACING
            std::cout<<"skip_over(Nothing)\n";
          #endif//USE_TRACING  
        }

    }

    // this tag is used to find the skipper from the context
    struct skipper_tag{};
    
    ///////////////////////////////////////////////////////////////////////////
    // If the skip-parser stored in context is neither an unused_type or
    // an unused_skipper, move the /first/ iterator to the first non-matching position
    // of that skip-parser.  Otherwise, this is a no-op.
    ///////////////////////////////////////////////////////////////////////////
    template< typename Skipper, typename Context>
    inline void skip_over( scanner<Skipper>& first, Context const& context)
    {
      auto const&skipper=x3::get<skipper_tag>( context);
      //std::cout<<"decltype(skipper)="<<type_name<decltype(skipper)>()<<"\n";
      detail::skip_over( first, skipper);
    }
  #endif//USE_SCAN_SKIP
  
    //}skip_over
    
    //{char_parser
    //  Modified from char_parser from:
    //    x3/char/char_parser.hpp
    template <typename Derived>
    struct char_parser : parser<Derived>
    {
        static unsigned const precedence=0;
        
          template 
          < typename Skipper 
          , typename Context
          >
        bool parse( scanner<Skipper>& first, Context const& context) const
        {
          //#define CHAR_PARSER_TRACE
          #if defined(USE_TRACING) && defined(CHAR_PARSER_TRACE)
    	    trace_scope ts("char_parser::parse");
            std::cout<<":first.enter="<<first<<"\n";
          #endif
            y3::skip_over( first, context);
            bool result=false;
            if (!first.at_end() && this->derived().test( *first, context))
            {
                ++first;
                result=true;
            }
          #if defined(USE_TRACING) && defined(CHAR_PARSER_TRAC)
            std::cout<<":first.exit ="<<first<<":result="<<boolalpha<<result<<"\n";
          #endif
            return result;
        }
    };
    
    //}char_parser
    
    //{char_class
    //  The following char_class_space code modified from
    //  the space_type() defined via the BOOST_SPIRIT_X3_CHAR_CLASS macro
    //  here:
    //    x3/char/char_class.hpp
    struct char_class_space
      : char_parser<char_class_space>
    {
        template<typename Context>
        bool test(int ch, Context const& context) const
        {
            return std::isspace(ch);
        }
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , char_class_space const&p
	  )
          {
	    os<<"space";
	    return os;
	  }
    };

    char_class_space const space = char_class_space{};
    
    //}char_class
  
    //{skip directive
    //  The following code modified from:
    //    x3/directive/skip.hpp
    template <typename Subject, typename Skipper>
    struct skip_directive : parser<skip_directive<Subject, Skipper>>
    {
        static unsigned const precedence=0;

        skip_directive(Subject const& subject, Skipper const& skipper)
          : subject(subject)
          , skipper(skipper)
        {}

        template < typename SkipperOld, typename Context>
        bool parse( scanner<SkipperOld>& first, Context const& context) const
        {
        #ifdef USE_TRACING
          std::ostringstream os;
          os<<"("<<*this<<")";
  	    os<<"::parse";
  	    std::string str=os.str();
  	    trace_scope ts(str);
        #endif
        #ifdef USE_SCAN_SKIP
            auto new_first=first.rebind_skipper(skipper);
            bool result=this->subject.parse( new_first, context);
            first=new_first;
        #else
            auto const new_ctx=make_context<skipper_tag>(skipper, context);
          #ifdef REPORT_CONTEXT
            std::cout<<": new_ctx="<<ctx_str(new_ctx)<<"\n";
          #endif
            bool result=this->subject.parse( first, new_ctx);
          #ifdef REPORT_CONTEXT
            std::cout<<": old_ctx="<<ctx_str(context)<<"\n";
          #endif
        #endif//USE_SCAN_SKIP
            return result;
        }

        Subject subject;
        Skipper const skipper;
        
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , skip_directive const&p
	  )
          {
	    os<<"skip_directive("<<p.skipper<<")["<<p.subject<<"]";
	    return os;
	  }
    };

    struct reskip_gen
    {
        template <typename Skipper>
        struct skip_gen
        {
            skip_gen(Skipper const& skipper)
              : skipper_(skipper) {}

            template <typename Subject>
            skip_directive<Subject, Skipper>
            operator[](Subject const& subject) const
            {
                return { subject, skipper_ };
            }

            Skipper skipper_;
        };

        template <typename Skipper>
        skip_gen<Skipper> const operator()(Skipper const& skipper) const
        {
            return { skipper };
        }
    };

    auto const skip = reskip_gen{};
    
    //}skip directive

    //{lit_parser
    //  The following parser modified from literal_string here:
    //    x3/string/literal_string.hpp
    struct lit_parser : parser<lit_parser>
    {
        static unsigned const precedence=0;
        
        using String=char const*;
        lit_parser(String str) : _str(str) {}

        template< typename Skipper, typename Context>
        bool parse( scanner<Skipper>& first, Context const& ctx) const
        {
	  bool result=true;
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
          trace_scope ts(str);
          std::cout<<":first.enter="<<first<<"\n";
        #endif
          y3::skip_over( first, ctx);//preskip
          char const* i_str=_str;
          //{string_parse
          //  The following copy&pasted&modified 
          //  from string_parse function from:
          //    x3/string/detail/string_parse.hpp
          scanner<Skipper> i_inp(first);
          char ch = *i_str;

          for (; !!ch; ++i_inp)
          {
            //#define LIT_PARSE_CHAR
            #ifdef LIT_PARSE_CHAR
              std::cout<<"{ ch="<<ch<<", *i_inp="<<*i_inp<<"}\n";
            #endif
              if ( i_inp.at_end() || (ch != *i_inp) )
              {
                  result=false;
                  break;
              }
              ch = *++i_str;
          }
          if(result)first=i_inp;
        #ifdef USE_TRACING
          std::cout<<":first.exit ="<<first<<"\n";
	  std::cout<<":result="<<std::boolalpha<<result<<"\n";
        #endif
          return result;
          //}string_parse
        }

        String _str;
	
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , lit_parser const&p
	  )
          {
	    os<<"lit_(\""<<p._str<<"\")";
	    return os;
	  }
    };

    inline lit_parser lit(char const* str)
    {
        return lit_parser(str);
    };
    //}lit_parser

    template
    < unsigned ParenParser//parent (container) parser precedence.
    , typename ChildParser//child parser contained in parent.
    >
    std::ostream&
  parens
    ( std::ostream&os
    , ChildParser const&child
    )
    {
      bool constexpr print=ParenParser<ChildParser::precedence;
      if(print)
      { os<<"( ";
      }
      os<<child;
      if(print)
      { os<<") ";
      }
      return os;
    };
    
  //{operator/*.hpp
  //  The following modified from obvious files in:
  //    x3/operator/*.hpp
    template <typename Left, typename Right>
    struct sequence_parser : parser<sequence_parser<Left, Right>>
    {
        static unsigned const precedence=7;
        
        sequence_parser(Left left, Right right)
            : left(left), right(right) {}

        template< typename Skipper, typename Context>
        bool parse( scanner<Skipper>& first, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
          std::cout<<":first="<<first<<"\n";
        #endif
	  bool result=left.parse( first, ctx);
          if(result)
            result=right.parse( first, ctx);
	  return result;
        }

        Left left;
        Right right;
	
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , sequence_parser<Left,Right> const&p
	  )
          {
	    parens<precedence>(os,p.left);
            os<<" >> ";
	    parens<precedence>(os,p.right);
	    return os;
	  }
    };

    template <typename Left, typename Right>
    inline sequence_parser<Left, Right> operator>>(
        parser<Left> const& left, parser<Right> const& right)
    {
        return sequence_parser<Left, Right>(
            left.derived(), right.derived());
    }

    template <typename Left, typename Right>
    struct alternative_parser : parser<alternative_parser<Left, Right>>
    {
        static unsigned const precedence=12;
        
        alternative_parser(Left left, Right right)
            : left(left), right(right) {}

        template< typename Skipper, typename Context>
        bool parse( scanner<Skipper>& first, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
	  bool result=left.parse( first, ctx);
	  if(!result)
            result=right.parse( first, ctx);
	  return result;
        }

        Left left;
        Right right;
	
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , alternative_parser<Left,Right> const&p
	  )
          {
	    os<<p.left<<" | "<<p.right;
	    return os;
	  }
    };

    template <typename Left, typename Right>
    inline alternative_parser<Left, Right> operator|(
        parser<Left> const& left, parser<Right> const& right)
    {
        return alternative_parser<Left, Right>(
            left.derived(), right.derived());
    }

    //The following essentially from [star].
    template <typename Subject>
    struct kleene_parser : parser<kleene_parser<Subject>>
    {
        static unsigned const precedence=3;
        
        kleene_parser(Subject const& subject)
          : subject(subject){}

        template< typename Skipper, typename Context>
        bool parse( scanner<Skipper>& first, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
          unsigned count=0;
        #endif
          while (subject.parse( first, ctx))
        #ifdef USE_TRACING
          {
            std::cout<<":kleene_parse:count="<<(++count)<<"\n";
          }
          std::cout<<":kleene_parse:end.count="<<count<<"\n";
        #else
          ;
        #endif
          return true;
        }
        
        Subject subject;
        
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , kleene_parser<Subject> const&p
	  )
          {
	    os<<"*("<<p.subject<<")";
	    return os;
	  }
    };
    
      template <typename Subject>
      inline 
      typename enable_if
      < is_parser<Subject>
      , kleene_parser<Subject>
      >::type
    operator*(Subject const& subject)
      {
        return kleene_parser<Subject>(subject.derived());
      }
  //}operator/*.hpp

  //{rule.hpp
  //  The following is a vastly simplified version of code in:
  //    x3/nonterminal/rule.hpp
  //    x3/nonterminal/detail/rule.hpp
    template <typename ID>
    struct rule
      ;
    namespace detail_rule
    {
        template <typename ID>
        struct always_false
        {
            static constexpr bool value=false;
        };
    }//detail_rule namespace
    
    /*! \brief Default parse_rule implementation.
     *  parse_rule's specialized for a particular
     *  rule<ID> are generated by using the 
     *  BOOST_SPIRIT_DEFINE macro, which is
     *  defined below.
     *
     *  If, BOOST_SPIRIT_DEFINE not used, this causes
     *  firing of the static_assert in body of this function
     *  during compilation.
     */
      template 
      < typename ID
      , typename Skipper
      , typename Context
      >
      bool
    parse_rule
      ( rule<ID>const&
      , scanner<Skipper>&
      , Context const&
      )
      {
        static_assert
          ( detail_rule::always_false<ID>::value
            //Using just false in the above causes assert to
            //always fire.  Using the above template
            //metafunction call avoids that.
          , "Undefined rule.  Use BOOST_SPIRIT_DEFINE to define rule."
          );
        return false;
      }
#ifdef USE_TRACING
  //#define PARSE_RULE_SPECIALIZED_TRACE_INOUT boost::trace_scope ts(std::string(__func__)+".specialized_yes");
  //#define PARSE_RULE_SPECIALIZED_TRACE_RESULT std::cout<<":result="<<boolalpha<<result<<"\n";
#else
  #define PARSE_RULE_SPECIALIZED_TRACE_INOUT
  #define PARSE_RULE_SPECIALIZED_TRACE_RESULT
#endif
/*!
  \def BOOST_SPIRIT_DEFINE_(r, data, rule_name)
    \a r is ignored.
    \a data is ignored.
    \a rule_name is the variable name of a variable
       with type, rule<ID>.
    The macro generates an instance of parse_rule
    specialized on decltype(rule_name) as 1st arg.  The body
    calls BOOST_PP_CAT(rule_name,_def) which, of course,
    must be defined before this macro is executed.
    ***For example, if rule_name is x, then there must be
    declared, x_def, in the scope "surrounding" the call
    to this macro.***
    This macro differs from that in [rule] because
    it does *not* call rule_name::operator=.  The reason
    it does not is because, for this file, only parsing
    and no attributes are needed.  The actual
    BOOST_SPIRIT_DEFINE_ calls rule_definitions.
    *Maybe* this is needed for attribute processing, but
    here, there are no attributes.
*/
#define BOOST_SPIRIT_DEFINE_(r, data, rule_name) \
    template                                     \
    < typename Skipper                           \
    , typename Context                           \
    >                                            \
    inline bool                                  \
    parse_rule                                   \
    ( decltype(rule_name) rule_                  \
    , y3::scanner<Skipper>& first                \
    , Context const& context                     \
    )                                            \
    {                                            \
      static auto const def_=                    \
        ( rule_ = BOOST_PP_CAT(rule_name, _def)  \
        );                                       \
      bool result=def_.parse( first, context);   \
      return result;                             \
    }                                            \
    /***/
#define BOOST_SPIRIT_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(              \
    BOOST_SPIRIT_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))  \
    /***/
  
      template
      < typename ID
      , typename RHS
      >
    struct rule_definition 
      : parser
        < rule_definition
          < ID
          , RHS
          >
        >
    {
        static unsigned const precedence=0;
        
        using lhs_id=ID;
        
        rule_definition(RHS const& rhs)
          : rhs(rhs){}
        
          template 
          < typename Skipper
          , typename Context
          >
          bool 
        parse( scanner<Skipper>& first, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<*this;
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
          std::cout<<":first="<<first<<"\n";
        #endif
        #ifdef REPORT_CONTEXT
          std::cout<<__func__
            <<" in rule_definition<"<<type_name<ID>()<<">"
            <<":ctx_str="<<ctx_str(ctx)<<"\n";
        #endif
          bool result=rhs.parse( first, ctx);
          return result;
        }
        
        RHS rhs;

        friend std::ostream&
	operator<<(std::ostream& os, rule_definition<ID,RHS>const&p)
        {
	    os<<"rule_definition<"<<type_name<ID>()<<">";
	    return os;
        }
    };//rule_definition struct

    template <typename ID>
    struct rule : parser<rule<ID>>
    {
        static unsigned const precedence=0;
        
        rule(){}
        
        template <typename Derived>
        rule_definition<ID, Derived>
        operator=(parser<Derived> const& definition) const
        {
            return rule_definition<ID, Derived>(definition.derived());
        }

          template 
          < typename Skipper
          , typename Context
          >
        bool parse( scanner<Skipper>& first, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<type_name<rule<ID> >();
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
          std::cout<<":first="<<first<<"\n";
        #endif
        #ifdef REPORT_CONTEXT
          std::cout<<__func__
            <<" in rule<"<<type_name<ID>()<<">"
            <<":ctx_str="<<ctx_str(ctx)<<"\n";
        #endif
          bool result=parse_rule( *this, first, ctx);
        #ifdef USE_TRACING
	  std::cout<<":result="<<std::boolalpha<<result<<"\n";
        #endif
	  return result;
        }
        
        friend std::ostream&
	operator<<(std::ostream& os, rule<ID>const&p)
        {
          os<<"rule<"<<type_name<ID>()<<">";
          return os;
        }
    };//rule struct
    
  //}rule.hpp
  //{phrase_parse
  //  The following modified from latter part of:
  //    x3/core/parse.hpp  
    template <typename Parser>
    inline bool
    phrase_parse
      ( input_stream_t input
      , Parser const& p
      )
      {
      #ifdef USE_SCAN_SKIP
        scanner<> first(input,space);
        auto skipper_ctx=unused;
      #else
        scanner<> first(input);
        auto skipper_ctx=make_context<skipper_tag>(space);
      #endif//USE_SCAN_SKIP
        bool result=p.parse( first, skipper_ctx);
        //std::cout<<"result="<<std::boolalpha<<result<<"\n";
        //std::cout<<"first1="<<first<<"\n";
        y3::skip_over( first, skipper_ctx);//post skip.
        //std::cout<<"first2="<<first<<"\n";
        return result && first.at_end();
      }
  //}phrase_parse
  
}//exit y3 namespace

#endif//WORKBENCH_Y3_SCANNER_HPP_INCLUDED
