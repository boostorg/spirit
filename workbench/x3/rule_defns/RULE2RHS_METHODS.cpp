//Print names of RULE2RHS methods indicated by argv[].
#include "RULE2RHS_METHODS.hpp"
#include <iostream>
#include <string>
int main(int argc, char*argv[])
{
  auto print_meth=[](int i)
  {
      std::cout<<"method["<<i<<"]="<<rule2rhs_name[i]<<"\n";
  };
  if(argc>1)
  { 
    for(unsigned i=1; i<argc; ++i)
    {
      int rule2rhs_which=std::stoi(argv[i]);
      print_meth(rule2rhs_which);
    }
  }
  else
  {
    int const n=RULE2RHS_SIZE;
    for(int i=0; i<n; ++i)
    {
      print_meth(i);
    }
  }
  return 0;
}  
