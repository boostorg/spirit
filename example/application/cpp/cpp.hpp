/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_HPP__920D0370_741F_44AF_BF86_F6104BDACF75__INCLUDED_)
#define _CPP_HPP__920D0370_741F_44AF_BF86_F6104BDACF75__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
//  This file may be used as a precompiled header (if applicable)

///////////////////////////////////////////////////////////////////////////////
//  build version
#include "cpp_version.hpp"

///////////////////////////////////////////////////////////////////////////////
//  configure this app here (global configiguration constants)
#include <boost/config.hpp>     //  global configuration information
#include "cpp_config.hpp"

///////////////////////////////////////////////////////////////////////////////
//  specific exceptions, thrown by the cpp::pp_iterator functions
#include "cpp/cpp_exceptions.hpp"

///////////////////////////////////////////////////////////////////////////////
//  include often used files from the stdlib
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//  include required boost libraries
#include <boost/assert.hpp>

#endif // !defined(_CPP_HPP__920D0370_741F_44AF_BF86_F6104BDACF75__INCLUDED_)
