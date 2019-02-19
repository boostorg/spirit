//Purpose:
//  test that only 1 attribute transformation occurs.
//Method:
//  specialize transform_attribute<int,int> to
//  ++attribute.  If the transform done more than once
//  this should be evident in the resulting value.
//===============
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
namespace x3=boost::spirit::x3;

x3::rule<class trace_xform_id,int> xform_rule="xform_rule";
namespace boost { namespace spirit { namespace x3
{
    template <>
    struct transform_attribute<int,int>
    {
        typedef int type;

        static type pre(int&attr) 
        { 
            return attr;
        }

        static void post(int& val, type&& attr)
        {
            val=attr+1;//non-default transform
        }
    };
}}}
auto xform_rule_def=x3::int_;
BOOST_SPIRIT_DEFINE(xform_rule)

int main()
{
    std::string input_str="1234";
    using iter_t=std::string::iterator;
    iter_t const first=input_str.begin();
    iter_t const last=input_str.end();
    int attr_defn;
    bool result_defn=
      x3::parse
      ( first
      , last
      , xform_rule = xform_rule_def
      , attr_defn
      );
    int attr_rule;
    bool result_rule=
      x3::parse
      ( first
      , last
      , xform_rule
      , attr_rule
      );
    BOOST_TEST(result_defn);
    BOOST_TEST(result_rule);
    BOOST_TEST(attr_defn==attr_rule);
    return boost::report_errors();
}  
