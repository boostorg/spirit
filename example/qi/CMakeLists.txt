#==============================================================================
#   Copyright (c) 2001-2007 Joel de Guzman
#
#   Distributed under the Boost Software License, Version 1.0. (See accompanying
#   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#==============================================================================
# project spirit-qi-example
#     : requirements <toolset>gcc:<c++-template-depth>300
#     :
#     :
#     ;

boost_add_executable(actions_ actions.cpp)
boost_add_executable( sum  sum.cpp )
boost_add_executable( complex_number  complex_number.cpp )
boost_add_executable( employee  employee.cpp  )
boost_add_executable( roman  roman.cpp )
boost_add_executable( mini_xml1  mini_xml1.cpp )
boost_add_executable( mini_xml2  mini_xml2.cpp )
boost_add_executable( mini_xml3  mini_xml3.cpp )
boost_add_executable( qi_num_list1  num_list1.cpp )
boost_add_executable( num_list2  num_list2.cpp )
boost_add_executable( num_list3  num_list3.cpp )
boost_add_executable( num_list4  num_list4.cpp )
boost_add_executable( reorder_struct  reorder_struct.cpp )

boost_add_executable( calc1  calc1.cpp )
boost_add_executable( calc2  calc2.cpp )
boost_add_executable( calc2_ast  calc2_ast.cpp )
boost_add_executable( calc3  calc3.cpp )
# boost_add_executable( calc3_lexer  calc3_lexer.cpp )
boost_add_executable( calc4  calc4.cpp )
boost_add_executable( calc4_debug  calc4_debug.cpp )
boost_add_executable( calc5  calc5.cpp )

boost_add_executable( calc6 
     calc6/calc6.cpp
     calc6/calc6a.cpp
     calc6/calc6b.cpp
     calc6/calc6c.cpp
     )

boost_add_executable( calc7 
     calc7/calc7.cpp
     calc7/calc7a.cpp
     calc7/calc7b.cpp
     calc7/calc7c.cpp
     )

boost_add_executable( mini_c 
     mini_c/mini_c.cpp
     mini_c/mini_ca.cpp
     mini_c/mini_cb.cpp
     mini_c/mini_cc.cpp
     mini_c/mini_cd.cpp
     )

