//  Copyright (c) 2001-2009 Hartmut Kaiser
//  http://spirit.sourceforge.net/
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SPIRIT_SCOPED_ENUM_EMULATION_HPP
#define BOOST_SPIRIT_SCOPED_ENUM_EMULATION_HPP

#include <boost/version.hpp>

#if BOOST_VERSION >= 104000
#include <boost/detail/scoped_enum_emulation.hpp>
#else
# define BOOST_SCOPED_ENUM_START(name) struct name { enum enum_t
# define BOOST_SCOPED_ENUM_END };
# define BOOST_SCOPED_ENUM(name) name::enum_t
#endif

#endif
