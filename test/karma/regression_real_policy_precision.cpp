//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/spirit/home/karma/numeric/real_policies.hpp>
#include <limits>

#include "real.hpp"

template <class T>
struct max_precision_policy : boost::spirit::karma::real_policies<T> {
    static BOOST_CONSTEXPR unsigned precision(T /*unused*/) BOOST_NOEXCEPT {
        return std::numeric_limits<T>::max_digits10;
    }
};

int main() {
    using namespace boost::spirit;

    boost::spirit::karma::real_generator<long double,
                                         max_precision_policy<long double>>
        real;

    BOOST_TEST(test("1.004999999999999999996", real, 1.005l));
    BOOST_TEST(test("1.049999999999999999956", real, 1.05l));
    BOOST_TEST(test("1.549999999999999999968", real, 1.55l));

    // Construct the fractional part as the limit of uint64_t plus one as
    // 1844674407370955265 = (2^64-1)/10)+1
    // to ensure the formatting does not break above this internal limit.
    BOOST_TEST(test("1.001844674407370955251", real, 1.001844674407370955251l));
    BOOST_TEST(test("1.01844674407370955262", real, 1.01844674407370955262l));
    BOOST_TEST(test("1.184467440737095526528", real, 1.184467440737095526528l));

    return boost::report_errors();
}
