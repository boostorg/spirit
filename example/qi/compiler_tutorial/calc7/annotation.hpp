/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_ANNOTATION_HPP)
#define BOOST_SPIRIT_CALC7_ANNOTATION_HPP

#include <map>
#include <boost/variant/apply_visitor.hpp>
#include "ast.hpp"

namespace client
{
    ///////////////////////////////////////////////////////////////////////////////
    //  The annotation handler links the AST to a map of iterator positions
    //  for the purpose of subsequent semantic error handling when the
    //  program is being compiled.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct annotation
    {
        template <typename, typename>
        struct result { typedef void type; };

        std::map<int, Iterator>& iters;
        int& current_id;
        annotation(int& current_id, std::map<int, Iterator>& iters)
          : current_id(current_id), iters(iters) {}

        struct set_id
        {
            typedef void result_type;

            int id;
            set_id(int id) : id(id) {}

            template <typename T>
            void operator()(T& x) const
            {
                x.id = id;
            }
        };

        void operator()(ast::operand& ast, Iterator pos) const
        {
            int id = current_id++;
            iters[id] = pos;
            boost::apply_visitor(set_id(id), ast);
        }
    };
}

#endif

