//  Copyright (c) 2011 Roji Philip
//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/optional.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_adapt_adt_attributes.hpp>

#include "test.hpp"

///////////////////////////////////////////////////////////////////////////////
struct test 
{
    unsigned var;
    boost::optional<unsigned> opt;

    unsigned const& getvar() const { return var; }
    void setvar(unsigned val) { var = val; }

    boost::optional<unsigned> const& getopt() const { return opt; }
    void setopt(boost::optional<unsigned> const& val) { opt = val; }
};

BOOST_FUSION_ADAPT_ADT(
    test,
    (unsigned const&, unsigned const&, obj.getvar(), obj.setvar(val) )
    (boost::optional<unsigned> const&, boost::optional<unsigned> const&, 
        obj.getopt(), obj.setopt(val) )
)

///////////////////////////////////////////////////////////////////////////////
int main() 
{
    using spirit_test::test_attr;
    namespace qi = boost::spirit::qi;

    {
        test data;
        BOOST_TEST(test_attr("123@999", qi::uint_ >> -('@' >> qi::uint_), data) &&
            data.var == 123 && data.opt && data.opt.get() == 999);
    }

    {
        test data;
        BOOST_TEST(test_attr("123", qi::uint_ >> -('@' >> qi::uint_), data) &&
            data.var == 123 && !data.opt);
    }

    return boost::report_errors();
}
