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
#include <boost/spirit/dynamic/for.hpp>

extern bool fun();

struct ftor
{
    bool operator ()() const;
};

extern void init_fun();

struct init_ftor
{
    void operator()() const;
};

extern void step_fun();

struct step_ftor
{
    void operator()() const;
};

extern bool cmp_fun();

struct cmp_ftor
{
    bool operator()() const;
};

int
main()
{
    //////////////////////////////////
    // compile time check wether as_parser<> works for for_p

    ::boost::spirit::rule<> r;

    r = boost::spirit::for_p(&init_fun, &cmp_fun, &step_fun)['-'];
    r = boost::spirit::for_p(init_ftor(), cmp_ftor(), step_ftor())["-"];

    r = boost::spirit::for_p(init_ftor(), r, step_ftor())[r];
}
