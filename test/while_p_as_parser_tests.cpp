/*=============================================================================
  Spirit V1.6.1
  Copyright (c) 2003 Martin Wille
  http://spirit.sourceforge.net/

  Permission to copy, use, modify, sell and distribute this software
  is granted provided this copyright notice appears in all copies.
  This software is provided "as is" without express or implied
  warranty, and with no claim as to its suitability for any purpose.
 =============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic/while.hpp>

extern bool fun();

struct ftor
{
    bool operator()() const;
};

int
main()
{
    //////////////////////////////////
    // compile time check wether as_parser<> works for while_p

    ::boost::spirit::rule<> r;

    r = ::boost::spirit::while_p('-')['-'];
    r = ::boost::spirit::while_p("-")["-"];

    r = ::boost::spirit::while_p(&fun)["foo"];
    r = ::boost::spirit::while_p(ftor())["foo"];

    r = ::boost::spirit::while_p(r)[r];

    r = ::boost::spirit::do_p['-'].while_p('-');
    r = ::boost::spirit::do_p["-"].while_p("-");

    r = ::boost::spirit::do_p["foo"].while_p(&fun);
    r = ::boost::spirit::do_p["foo"].while_p(ftor());

    r = ::boost::spirit::do_p[r].while_p(r);
}
