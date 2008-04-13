/*=============================================================================
  Copyright (c) 2001-2008 Joel de Guzman
  Copyright (c) 2001-2008 Hartmut Kaiser
  http://spirit.sourceforge.net/

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_CLASSIC_NAMESPACE_HPP)
#define SPIRIT_CLASSIC_NAMESPACE_HPP

#if defined(BOOST_SPIRIT_USE_OLD_NAMESPACE)

#define BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN /*namespace classic {*/
#define BOOST_SPIRIT_CLASSIC_NS              boost::spirit::/*classic*/
#define BOOST_SPIRIT_CLASSIC_NAMESPACE_END   /*}*/
        
#else

#define BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN namespace classic {
#define BOOST_SPIRIT_CLASSIC_NS              boost::spirit::classic
#define BOOST_SPIRIT_CLASSIC_NAMESPACE_END   }

#endif

#endif 
