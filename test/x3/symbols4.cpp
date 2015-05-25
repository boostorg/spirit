/*=============================================================================
    Copyright (c) 2015 Thomas Bernard

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>

#include "test.hpp"


#include <boost/spirit/home/x3.hpp>

 namespace braille {
      namespace encoding {
          struct dots
          {
              using char_type = char32_t;
          };
      }

      template <typename Encoding>
      struct dots_compare
      {
          int32_t operator()(
                typename Encoding::char_type const lc
              , typename Encoding::char_type const rc) const
          {
              // do magic here
              return lc - rc;
          }

      };

      struct get_dots_compare
      {
          template <typename Context>
           static dots_compare<encoding::dots> get(Context const& context)
           {
               return {};
           }
      };
 }

 namespace braille {
      template<typename T = boost::spirit::x3::unused_type>
      using symbols = boost::spirit::x3::symbols_parser<braille::encoding::dots, T, get_dots_compare>;
 }

 int main()
 {
    using spirit_test::test;
    {
      braille::symbols<> syms;
      syms.add(U"\u2800");
      BOOST_TEST((test(U"\u2800", syms)));
    }
    return boost::report_errors();
 }
