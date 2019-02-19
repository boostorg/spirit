/*=============================================================================
  Copyright (c) 2018 Nikita Kniazev

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3/support/numeric_utils/extract_int.hpp>
#include <cstdio>
#include <iosfwd>
#include <limits>

struct custom_int
{
    custom_int() = default;
    constexpr custom_int(int value) : value_{value} {}

    custom_int operator+(custom_int x) const { return value_ + x.value_; }
    custom_int operator-(custom_int x) const { return value_ - x.value_; }
    custom_int operator*(custom_int x) const { return value_ * x.value_; }
    custom_int operator/(custom_int x) const { return value_ / x.value_; }

    custom_int& operator+=(custom_int x) { value_ += x.value_; return *this; }
    custom_int& operator-=(custom_int x) { value_ -= x.value_; return *this; }
    custom_int& operator*=(custom_int x) { value_ *= x.value_; return *this; }
    custom_int& operator/=(custom_int x) { value_ /= x.value_; return *this; }
    custom_int& operator++() { ++value_; return *this; }
    custom_int& operator--() { --value_; return *this; }
    custom_int operator++(int) { return value_++; }
    custom_int operator--(int) { return value_--; }

    custom_int operator+() { return +value_; }
    custom_int operator-() { return -value_; }

    bool operator< (custom_int x) const { return value_ <  x.value_; }
    bool operator> (custom_int x) const { return value_ >  x.value_; }
    bool operator<=(custom_int x) const { return value_ <= x.value_; }
    bool operator>=(custom_int x) const { return value_ >= x.value_; }
    bool operator==(custom_int x) const { return value_ == x.value_; }
    bool operator!=(custom_int x) const { return value_ != x.value_; }

    template <typename Char, typename Traits>
    friend std::basic_ostream<Char, Traits>&
    operator<<(std::basic_ostream<Char, Traits>& os, custom_int x) {
        return os << x.value_;
    }

    static constexpr int max = 10;
    static constexpr int min = -11;

private:
    int value_;
};

namespace std {

template <>
class numeric_limits<custom_int> : public numeric_limits<int>
{
public:
    static constexpr custom_int max() noexcept { return custom_int::max; }
    static constexpr custom_int min() noexcept { return custom_int::min; }
    static constexpr custom_int lowest() noexcept { return min(); }
    static constexpr int digits = 4;
    static constexpr int digits10 = 1;
};

}

int main()
{
    namespace x3 = boost::spirit::x3;

    for (int i = -20; i <= 20; ++i) {
        int initial = i > 0 ? -9 : 9;
        custom_int x = initial;
        char s[4];
        std::snprintf(s, 4, "%d", i);
        auto begin = &s[0], it = begin, end = it + std::strlen(it);

        // Test prerequest: Limits::is_bounded == true
        // Check that parser fails on overflow
        bool r = x3::extract_int<custom_int, 10, 1, -1>::call(it, end, x);
        if (custom_int::min <= i && i <= custom_int::max) {
            BOOST_TEST(r);
            BOOST_TEST(it == end);
            BOOST_TEST_EQ(x, i);
        }
        else {
            BOOST_TEST(!r);
            BOOST_TEST(it == begin);
        }

        // Test prerequest: MaxDigits == log10(Limits::digits)
        //                  MaxDigits > Limits::digits10
        // Check that MaxDigits > digits10 behave like MaxDigits=-1
        it = begin;
        x = initial;
        r = x3::extract_int<custom_int, 10, 1, 2>::call(it, end, x);
        if (custom_int::min <= i && i <= custom_int::max) {
            BOOST_TEST(r);
            BOOST_TEST(it == end);
            BOOST_TEST_EQ(x, i);
        }
        else {
            /* TODO: Looks like a regression. See #430
            BOOST_TEST(!r);
            BOOST_TEST(it == begin);
            */
        }

        // Test prerequest: MaxDigits < Limits::digits10
        // Check that unparsed digits are not consumed
        it = begin;
        x = initial;
        r = x3::extract_int<custom_int, 10, 1, 1>::call(it, end, x);
        static_assert(custom_int::min < -9 && custom_int::max > 9, "");
        BOOST_TEST(r);
        if (-9 <= i && i <= 9) {
            BOOST_TEST(it == end);
            BOOST_TEST_EQ(x, i);
        }
        else {
            BOOST_ASSERT(-100 < i && i < 100);
            BOOST_TEST_EQ(end - it, 1);
            BOOST_TEST_EQ(x, i / 10);
        }
    }

    return boost::report_errors();
}
