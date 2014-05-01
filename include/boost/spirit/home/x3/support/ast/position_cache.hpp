/*=============================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_POSITION_CACHE_MAY_01_2014_0321PM)
#define BOOST_SPIRIT_X3_POSITION_CACHE_MAY_01_2014_0321PM

#include <boost/range.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Container>
    class position_cache
    {
    public:

        position_cache(
          , pos_iterator_type first
          , pos_iterator_type last)
          : first_(first), last_(last) {}

        template <typename AST>
        boost::iterator_range<pos_iterator_type>
        position_of(AST const& ast) const
        {
            return
                boost::iterator_range<pos_iterator_type>(
                    positions.at(ast.id_first) // throws if out of range
                  , positions.at(ast.id_last)  // throws if out of range
                );
        }

        template <typename AST>
        void annotate(AST& ast, Iterator first, Iterator last) const
        {
            int id = positions.size();
            positions.push_back(first);
            ast.id_first = id;
            id = positions.size();
            positions.push_back(last);
            ast.id_last = id;
        }

        Container const&
        get_positions() const
        {
            return positions;
        }

        pos_iterator_type first() const { return first_; }
        pos_iterator_type last() const { return last_; }

    private:

        Container positions;
        pos_iterator_type first_;
        pos_iterator_type last_;
    };

}}}

#endif
