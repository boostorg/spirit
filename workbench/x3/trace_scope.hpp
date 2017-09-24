#ifndef TRACE_SCOPE_HPP_INCLUDED
#define TRACE_SCOPE_HPP_INCLUDED
#include "iostreams_utility.hpp"
//#define USE_TRACING  //if defined(USE_TRACING), trace output is produced.
#include <string>
namespace boost
{
  template
  < typename Enter=std::string
  , typename Exit=Enter
  >
  class
indent_scope
  //Purpose:
  //  Indicate on a std::ostream, 
  //  when a scope entered and exited.
  {
  public:
    indent_scope
      ( Enter a_enter="" //entry "tag"
      , Exit a_exit=""   //exit "tag"
      , std::ostream& a_strm=std::cout
      )
      : my_exit(a_exit)//save for DTOR.
      , my_strm(a_strm)//save for DTOR.
      {
      ; my_strm<<a_enter
      ; my_strm<<indent_buf_in
      ; my_strm.flush()
      ;}
    ~indent_scope(void)
      {
      ; my_strm<<indent_buf_out
      ; my_strm<<my_exit
      ; my_strm.flush()
      ;}
  private:
      Exit
    my_exit
      ;
      std::ostream&
    my_strm
      ;
  };//end indent_scope class
  class
trace_scope
  //Purpose:
  //  Indicate on a std::ostream, 
  //  when a scope entered and exited.
  //  Supply non-default values for
  //  entry and exit tags
  //  as function of a_scopeName.
  : indent_scope<>
  {
  public:
      template
      < typename ScopeName
      >
    trace_scope
      ( ScopeName a_scopeName
      , std::ostream& a_strm=std::cout
      )
      : indent_scope<>
        ( std::string("===>Enter:{{")+a_scopeName+"\n"
        , std::string("===>Exit_:}}")+a_scopeName+"\n"
        , a_strm
        )
      {
      }
  };//end trace_scope class
}//exit boost namespace
#endif//TRACE_SCOPE_HPP_INCLUDED
