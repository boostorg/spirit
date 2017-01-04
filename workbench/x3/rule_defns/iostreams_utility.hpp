#ifndef IOSTREAMS_UTILITH_HPP_INCLUDED
#define IOSTREAMS_UTILITH_HPP_INCLUDED
#include <iostream>
//#define USE_IOSTREAMS_UTILITY
#ifdef USE_IOSTREAMS_UTILITY
  #include <boost/iostreams/utility/indent_scoped_ostreambuf.hpp>
  //The code for above #include *should* be available here:
  /*
  https://github.com/cppljevans/variadic_templates/tree/master/boost/iostreams/utility
   */
#else
  //Otherwise, the following do-nothing code can be used instead; however,
  //no indentation of output will happen ;(
  namespace boost{namespace iostreams{
  template<typename Ch=char, typename Tr=std::char_traits<Ch> >
  class indent_scoped_ostreambuf 
  {
  public:
      typedef std::basic_ostream<Ch,Tr> std_ostrm_type;
      indent_scoped_ostreambuf
      ( std_ostrm_type& a_ostrm
      , int width=2
      )
      {}
  };
  }}//exit boost::iostreams namespace
  template<class Ch, class Tr>
  inline std::basic_ostream<Ch, Tr>&
  indent_buf_in(std::basic_ostream<Ch, Tr>& os)
  {
      return os;
  }
  template<class Ch, class Tr>
  inline std::basic_ostream<Ch, Tr>&
  indent_buf_out(std::basic_ostream<Ch, Tr>& os)
  {
      return os;
  }
#endif //USE_IOSTREAMS_UTILITY
#endif// IOSTREAMS_UTILITH_HPP_INCLUDED
