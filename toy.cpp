#include <iostream>

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
      Char ch;
   };

   template <typename Char>
   inline char_parser<Char> char_(Char ch)
   {
      char_parser<Char> r = { ch };
      return r;
   };

   template <typename Left, typename Right>
   struct sequence_parser : parser<sequence_parser<Left, Right>>
   {
      sequence_parser(Left left, Right right)
         : left(left), right(right) {}
      Left left;
      Right right;
   };

   template <typename Left, typename Right>
   inline sequence_parser<Left, Right> operator>>(
      parser<Left> const& left, parser<Right> const& right)
   {
      return sequence_parser<Left, Right>(left.derived(), right.derived());
   }

   template <typename Left, typename Right>
   struct alternative_parser : parser<alternative_parser<Left, Right>>
   {
      alternative_parser(Left left, Right right)
         : left(left), right(right) {}
      Left left;
      Right right;
   };

   template <typename Left, typename Right>
   inline alternative_parser<Left, Right> operator|(
      parser<Left> const& left, parser<Right> const& right)
   {
      return alternative_parser<Left, Right>(left.derived(), right.derived());
   }

}}}

int main()
{
   using namespace boost::spirit::x3;

   {
      auto abc = char_('a') >> char_('b') >> char_('c');
   }

   return 0;
}

