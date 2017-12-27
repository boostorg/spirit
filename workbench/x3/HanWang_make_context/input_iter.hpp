#ifndef INPUT_ITER_HPP_INCLUDED_20170917
#define INPUT_ITER_HPP_INCLUDED_20170917
#include <iostream>
#include <string>
  using 
input_stream_t
  =std::string const
  //default type of input stream.
  ;
  
#include <boost/range/iterator_range.hpp>
using input_rng=decltype(boost::make_iterator_range(std::declval<input_stream_t>()));
struct input_iter
: public input_rng
{
    using input_rng::
  input_rng
    ;
    using
  value_t=
    typename input_rng::value_type
    ;
    value_t
  operator*()
    {  return this->input_rng::front();
    }
    input_iter&
  operator++()
    {  this->input_rng::advance_begin(1);
       //std::cout<<"input_iter::operator++:this="<<*this<<"\n";
       return *this;
    }
    bool 
  at_end()const
    {  return this->input_rng::empty();
    }
      friend
    std::ostream&
  operator<<
    ( std::ostream& sout
    , input_iter const& inp
    )
    { 
      auto const&rng=static_cast<input_rng const&>(inp);
      for(char c:rng) 
      { 
        sout<<c;
      }
      return sout;
    }
};
  
#endif//INPUT_ITER_HPP_INCLUDED_20170917
