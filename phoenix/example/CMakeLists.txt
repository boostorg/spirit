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



