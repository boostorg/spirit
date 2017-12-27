#ifndef OPERATORS_HPP_INCLUDED
#define OPERATORS_HPP_INCLUDED
#include "../trace_scope.hpp"
#include <sstream>
using namespace boost;

    template <typename Derived>
    struct parser
    {
        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    template <typename Char>
    struct char_parser : parser<char_parser<Char>>
    {
        char_parser(Char ch) : _ch(ch) {}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
	  bool result=false;
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
	  if(first == last)
	  {
          #ifdef USE_TRACING
	    std::cout<<":first==last\n";
          #endif
	  }
	  else
	  {
	    char nxt_chr=*first;
          #ifdef USE_TRACING
	    std::cout<<":*first="<<nxt_chr<<"\n";
          #endif
	    if(nxt_chr==_ch)
	    {
              ++first;
              result=true;
	    }
	  }
        #ifdef USE_TRACING
	  std::cout<<":result="<<result<<"\n";
        #endif
          return result;
        }

        Char _ch;
	
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , char_parser<Char> const&p
	  )
          {
	    os<<"char_('"<<p._ch<<"')";
	    return os;
	  }
    };

    template <typename Char>
    inline char_parser<Char> char_(Char ch)
    {
        return char_parser<Char>(ch);
    };

    struct string_parser : parser<string_parser>
    {
        using String=char const*;
        string_parser(String str) : _str(str) {}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
	  bool result=true;
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
          char const* i_str=_str;
          Iterator i_inp = first;
          char ch = *i_str;

          for (; !!ch; ++i_inp)
          {
              if ( (i_inp == last) || (ch != *i_inp) )
              {
                  result=false;
                  break;
              }
              ch = *++i_str;
          }
          if(result)first=i_inp;
        #ifdef USE_TRACING
	  std::cout<<":result="<<result<<"\n";
        #endif
          return result;
        }

        String _str;
	
          friend std::ostream&
        operator<<
	  ( std::ostream& os
	  , string_parser const&p
	  )
          {
	    os<<"string_(\""<<p._str<<"\")";
	    return os;
	  }
    };

    inline string_parser string_(char const* str)
    {
        return string_parser(str);
    };

    template <typename Left, typename Right>
    struct sequence_parser : parser<sequence_parser<Left, Right>>
    {
        sequence_parser(Left left, Right right)
            : left(left), right(right) {}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
	  bool result=left.parse(first, last, ctx);
          if(result)
            result=right.parse(first, last, ctx);
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
	    os<<p.left<<" >> "<<p.right;
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
        alternative_parser(Left left, Right right)
            : left(left), right(right) {}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator last, Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
        #endif
	  bool result=left.parse(first, last, ctx);
	  if(!result)
            result=right.parse(first, last, ctx);
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
        kleene_parser(Subject const& subject)
          : subject(subject){}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator const& last
          , Context const& ctx) const
        {
        #ifdef USE_TRACING
	  std::ostringstream os;
	  os<<"("<<*this<<")";
	  os<<"::parse";
	  std::string str=os.str();
	  trace_scope ts(str);
          unsigned count=0;
        #endif
          while (subject.parse(first, last, ctx))
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
    inline kleene_parser<Subject>
    operator*(Subject const& subject)
    {
        return kleene_parser<Subject>(subject.derived());
    }
    
#endif//OPERATORS_HPP_INCLUDED
