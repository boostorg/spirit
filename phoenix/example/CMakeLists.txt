#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

foreach(test 
    all_odds
    arguments
    callback
    factorial
    find_if
    function
    if
    lambda
    values
    references)
  boost_test_run(${test} users_manual/${test}.cpp)
endforeach()



