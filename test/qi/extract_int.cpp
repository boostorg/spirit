/*=============================================================================
  Copyright (c) 2018 Nikita Kniazev

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/qi/numeric/numeric_utils.hpp>
#include <boost/static_assert.hpp>
#include <iosfwd>
#include <limits>
#include <sstream>

struct custom_int
{
    BOOST_DEFAULTED_FUNCTION(custom_int(), {})
    BOOST_CONSTEXPR custom_int(int value) : value_(value) {}

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

    BOOST_STATIC_CONSTEXPR int max = 10;
    BOOST_STATIC_CONSTEXPR int min = -11;

private:
    int value_;
};

namespace std {

template <>
class numeric_limits<custom_int> : public numeric_limits<int>
{
public:
    static BOOST_CONSTEXPR custom_int max() BOOST_NOEXCEPT_OR_NOTHROW { return custom_int::max; }
    static BOOST_CONSTEXPR custom_int min() BOOST_NOEXCEPT_OR_NOTHROW { return custom_int::min; }
    static BOOST_CONSTEXPR custom_int lowest() BOOST_NOEXCEPT_OR_NOTHROW { return min(); }
    BOOST_STATIC_CONSTEXPR int digits = 3;
    BOOST_STATIC_CONSTEXPR int digits10 = 1;
};

}

namespace qi = boost::spirit::qi;

template <typename T, int Base, int MaxDigits>
void test_overflow_handling(char const* begin, char const* end, int i)
{
    // Check that parser fails on overflow
    BOOST_STATIC_ASSERT_MSG(std::numeric_limits<T>::is_bounded, "tests prerequest");
    BOOST_ASSERT_MSG(MaxDigits == -1 || std::pow(Base, MaxDigits) >= T::max,
                     "test prerequest");
    int initial = Base - i % Base; // just a 'random' non-equal to i number
    T x(initial);
    char const* it = begin;
    bool r = qi::extract_int<T, Base, 1, MaxDigits>::call(it, end, x);
    if (T::min <= i && i <= T::max) {
        BOOST_TEST(r);
        BOOST_TEST(it == end);
        BOOST_TEST_EQ(x, i);
    }
    else {
        BOOST_TEST(!r);
        BOOST_TEST(it == begin);
    }
}

template <typename T, int Base>
void test_unparsed_digits_are_not_consumed(char const* it, char const* end, int i)
{
    // Check that unparsed digits are not consumed
    BOOST_STATIC_ASSERT_MSG(T::min <= -Base, "test prerequest");
    BOOST_STATIC_ASSERT_MSG(T::max >=  Base, "test prerequest");
    bool has_sign = *it == '+' || *it == '-';
    char const* begin = it;
    int initial = Base - i % Base; // just a 'random' non-equal to i number
    T x(initial);
    bool r = qi::extract_int<T, Base, 1, 1>::call(it, end, x);
    BOOST_TEST(r);
    if (-Base < i && i < Base) {
        BOOST_TEST(it == end);
        BOOST_TEST_EQ(x, i);
    }
    else {
        BOOST_TEST_EQ(end - it, (end - begin) - 1 - has_sign);
        BOOST_TEST_EQ(x, i / Base);
    }
}

template <typename T, int Base>
void run_tests(char const* begin, char const* end, int i)
{
    // Check that parser fails on overflow
    test_overflow_handling<T, Base, -1>(begin, end, i);
    // Check that MaxDigits > digits10 behave like MaxDigits=-1
    test_overflow_handling<T, Base, 2>(begin, end, i);
    // Check that unparsed digits are not consumed
    test_unparsed_digits_are_not_consumed<T, Base>(begin, end, i);
}

int main()
{
    for (int i = -20; i <= 20; ++i) {
        std::ostringstream oss;
        oss << i;
        std::string s = oss.str();
        char const* begin = s.data(), *const end = begin + s.size();

        run_tests<custom_int, 10>(begin, end, i);
    }

    return boost::report_errors();
}
