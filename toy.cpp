#include <iostream>
#include <utility>
#include <cstring>
#include <boost/mpl/identity.hpp>

namespace boost { namespace spirit { namespace x3
{
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
      char_parser(Char ch) : ch(ch) {}

      template <typename Iterator, typename Environment>
      bool parse(Iterator& first, Iterator const& last, Environment const& env) const
      {
         if (first == last)
            return false;

         if (*first == ch)
         {
            ++first;
            return true;
         }
         return false;
      }

      Char ch;
   };

   template <typename Char>
   inline char_parser<Char> char_(Char ch)
   {
      return char_parser<Char>(ch);
   };

   template <typename Left, typename Right>
   struct sequence_parser : parser<sequence_parser<Left, Right>>
   {
      sequence_parser(Left left, Right right)
         : left(left), right(right) {}

      template <typename Iterator, typename Environment>
      bool parse(Iterator& first, Iterator const& last, Environment const& env) const
      {
         return left.parse(first, last, env)
            && right.parse(first, last, env);
      }

      Left left;
      Right right;
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

      template <typename Iterator, typename Environment>
      bool parse(Iterator& first, Iterator const& last, Environment const& env) const
      {
         Iterator save = first;
         if (left.parse(first, last, env))
            return true;
         first = save;
         return right.parse(first, last, env);
      }

      Left left;
      Right right;
   };

   template <typename Left, typename Right>
   inline alternative_parser<Left, Right> operator|(
      parser<Left> const& left, parser<Right> const& right)
   {
      return alternative_parser<Left, Right>(
         left.derived(), right.derived());
   }

   template <typename Tag, typename Subject, typename Next>
   struct environment
   {
      environment(Subject const& subject, Next const& next)
         : subject(subject), next(next) {}

      Subject const& find(mpl::identity<Tag>) const
      {
         return subject;
      }

      template <typename Tag_>
      decltype(std::declval<Next>().find(mpl::identity<Tag_>()))
      find(mpl::identity<Tag_> id) const
      {
         return next.find(id);
      }

      Subject const& subject;
      Next const& next;
   };

   struct empty_environment
   {
      struct undefined {};
      template <typename Tag>
      undefined find(Tag) const
      {
         return undefined();
      }
   };

   template <typename Tag, typename Subject>
   struct rule_definition : parser<rule_definition<Tag, Subject>>
   {
      rule_definition(Subject subject)
         : subject(subject) {}

      template <typename Iterator, typename Environment>
      bool parse(Iterator& first, Iterator const& last, Environment const& env) const
      {
         environment<Tag, Subject, Environment> new_env(subject,  env);
         return subject.parse(first, last, new_env);
      }

      Subject subject;
   };

   template <typename Tag>
   struct rule : parser<rule<Tag>>
   {
      template <typename Derived>
      rule_definition<Tag, Derived>
      operator=(parser<Derived> const& definition)
      {
         return rule_definition<Tag, Derived>(definition.derived());
      }

      template <typename Iterator, typename Environment>
      bool parse(Iterator& first, Iterator const& last, Environment const& env) const
      {
         return env.find(mpl::identity<Tag>()).parse(first, last, env);
      }
   };

   template <typename Iterator, typename Derived>
   inline bool parse(parser<Derived> const& definition, Iterator& first, Iterator const& last)
   {
      empty_environment env;
      return definition.derived().parse(first, last, env);
   }

}}}

///////////////////////////////////////////////////////////////////////////////
// test code

template <typename Parser>
bool test_parse(Parser const& p, char const* in)
{
   return parse(p, in, in + std::strlen(in));
}

int main()
{
   using namespace boost::spirit::x3;

   { // a non-recursive parser
      auto abc = char_('a') >> char_('b') >> char_('c');
      std::cout << test_parse(abc, "abc") << std::endl;
      std::cout << test_parse(abc, "abx") << std::endl;
   }

   { // a recursive rule
      rule<class x> x;
      auto ax = char_('a') >> x;
      auto start = (x = char_('x') | ax);

      std::cout << test_parse(start, "x") << std::endl;
      std::cout << test_parse(start, "ax") << std::endl;
      std::cout << test_parse(start, "aaaaax") << std::endl;
      std::cout << test_parse(start, "aaz") << std::endl;
   }

   { // a grammar
      auto g = []()
      {
         rule<class x> x;
         auto ax = char_('a') >> x;
         return x = char_('x') | ax;
      }();

      std::cout << test_parse(g, "x") << std::endl;
      std::cout << test_parse(g, "ax") << std::endl;
      std::cout << test_parse(g, "aaaaax") << std::endl;
      std::cout << test_parse(g, "aaz") << std::endl;
   }

   return 0;
}

