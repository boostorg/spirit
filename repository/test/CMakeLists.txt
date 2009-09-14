#   Copyright (c) 2001-2009 Joel de Guzman
#   Copyright (c) 2001-2009 Hartmut Kaiser
#
#   Use, modification and distribution is subject to the Boost Software
#   License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt)
#==============================================================================

include_directories(.)

boost_additional_test_dependencies(spirit_repository BOOST_DEPENDS spirit)

message(STATUS "Boost/libs/spirit/repository/test needs GCC C++ -ftemplate-depth-300 flag")
message(STATUS "Check libs/spirit/repository/test: need -Wall -Wunused here when under gcc")

set(test_compile_flags "")
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(test_compile_flags "-ftemplate-depth-300")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# run Qi repository tests
boost_test_run(qi_repo_confix qi/confix.cpp COMPILE_FLAGS ${test_compile_flags})
boost_test_run(qi_repo_distinct qi/distinct.cpp COMPILE_FLAGS ${test_compile_flags})
boost_test_run(qi_repo_subrule qi/subrule.cpp COMPILE_FLAGS ${test_compile_flags})

# run Karma repository tests
boost_test_run(karma_repo_confix karma/confix.cpp COMPILE_FLAGS ${test_compile_flags})
boost_test_run(karma_repo_subrule karma/subrule.cpp COMPILE_FLAGS ${test_compile_flags})

