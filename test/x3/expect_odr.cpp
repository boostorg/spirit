/*=============================================================================
    Copyright (c) 2024 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/core/lightweight_test.hpp>

int main_expect_throw();
int main_expect_nothrow();

// Test case for mixing throwing/non-throwing modes in a
// single executable.
int main()
{
    main_expect_throw();

    // If you encounter a GS Security Check failure (on MSVC) or
    // buffer-related assertion failures (on other compilers),
    // that is not a bug in X3's buffer manipulation code.
    // Instead, it's likely due to an ODR violation, which must
    // be fixed in X3's implementation details.
    //
    // In common cases it should be detectable as a link error.
    //
    // The worst case is with Visual Studio 2022 (at least in version 17.10.5),
    // which does not even detect the error at link time. It will
    // compile and link successfully, but when you execute the
    // resulting binary, the assembly mistakenly assumes that
    // `x3::expectation_failure` is an instance of the *throwing*
    // version, causing it to access a nonexistent base class.
    //
    // This situation leads to a genuine stack overrun, which
    // will be caught by the GS Buffer Security Check:
    // https://learn.microsoft.com/en-us/cpp/build/reference/gs-buffer-security-check
    //
    // Surprisingly, in a Release build, the assertion may not
    // be triggered as the vulnerable parameters might be handled
    // differently by the compiler.
    main_expect_nothrow();

    BOOST_TEST(true);
    return boost::report_errors();
}
