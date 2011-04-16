//  Copyright (c) 2001-2011 Hartmut Kaiser
//  Copyright (c) 2011 Colin Rundel
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/fusion/include/adapt_adt.hpp>

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/support_adapt_adt_attributes.hpp>

#include "test.hpp"

///////////////////////////////////////////////////////////////////////////////
class data1
{
private:
    int width_;
    int height_;

public:
    data1()
      : width_(400), height_(400) 
    {}

    data1(int width, int height)
      : width_(width), height_(height) 
    {}

    int width() const { return width_;}
    int height() const { return height_;}

    void set_width(int width) { width_=width;}
    void set_height(int height) { height_=height;}
};

BOOST_FUSION_ADAPT_ADT(
    data1,
    (int, int, obj.width(),  obj.set_width(val))
    (int, int, obj.height(), obj.set_height(val))
);

///////////////////////////////////////////////////////////////////////////////
class data2
{
private:
    std::string data_;

public:
    data2()
      : data_("test")
    {}

    data2(std::string const& data)
      : data_(data)
    {}

    std::string const& data() const { return data_;}
    void set_data(std::string const& data) { data_ = data;}
};

BOOST_FUSION_ADAPT_ADT(
    data2, 
    (std::string, std::string const&, obj.data(), obj.set_data(val))
);

///////////////////////////////////////////////////////////////////////////////
int main () 
{
    using spirit_test::test;

    {
        using boost::spirit::karma::int_;

        data1 b(800, 600);
        BOOST_TEST(test("width: 800\nheight: 600\n", 
            "width: " << int_ << "\n" << "height: " << int_ << "\n", b));
    }

    {
        using boost::spirit::karma::char_;
        using boost::spirit::karma::string;

        data2 d("test");
        BOOST_TEST(test("data: test\n", "data: " << +char_ << "\n", d));
        BOOST_TEST(test("data: test\n", "data: " << string << "\n", d));
    }

    return boost::report_errors();
}
