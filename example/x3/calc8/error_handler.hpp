/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_ERROR_HANDLER_HPP)
#define BOOST_SPIRIT_X3_CALC8_ERROR_HANDLER_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include "expression.hpp"
#include "statement.hpp"

namespace client { namespace parser
{
    namespace x3 = boost::spirit::x3;
    
    ////////////////////////////////////////////////////////////////////////////
    //  Our error handler
    ////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    class error_handler : public x3::error_handler<Iterator>
    {
    public:
    
        typedef x3::error_handler<Iterator> base_type;
    
        error_handler(Iterator first, Iterator last, std::ostream& err_out, std::string file = "", int tabs = 4)
          : base_type(err_out, file, tabs)
          , pos_cache(first, last) {}
        
        template <typename AST>
        void annotate(AST& ast, Iterator first, Iterator last)
        {
            return pos_cache.annotate(ast, first, last);
        }
        
    private:
    
        x3::position_cache<std::vector<Iterator>> pos_cache;
    };
    
    // tag used to get our error handling from the context
    struct error_handler_tag;

    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result on_error(
        x3::identity<expression_id>, Iterator& first
      , Exception const& x, Context const& context)
    {
        auto& error_handler = x3::get<error_handler_tag>(context).get();
        error_handler(first, x.last, x.first, "Error! Expecting: " + x.what_);
        return x3::error_handler_result::fail;
    }
    
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result on_error(
        x3::identity<statement_id>, Iterator& first
      , Exception const& x, Context const& context)
    {
        auto& error_handler = x3::get<error_handler_tag>(context).get();
        error_handler(first, x.last, x.first, "Error! Expecting: " + x.what_);
        return x3::error_handler_result::fail;
    }
}}

#endif
