/*=============================================================================
    Copyright (c) 2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_POSITION_TAGGED_MAY_01_2014_0321PM)
#define BOOST_SPIRIT_X3_POSITION_TAGGED_MAY_01_2014_0321PM

#include <boost/range.hpp>

namespace boost { namespace spirit { namespace x3
{
    struct position_tagged
    {
        // Use this to annotate an AST with the iterator position.
        // These ids are used as a key to the position_cache (below)
        // and marks the start and end of an AST node.
        int id_first;
        int id_last;
    };

    // this is used to find the position_cache form the context
    struct position_cache_tag;

    template <typename Container>
    class position_cache
    {
    public:

        position_cache(
          , pos_iterator_type first
          , pos_iterator_type last)
          : first_(first), last_(last) {}

        // This will catch all nodes inheriting from position_tagged
        boost::iterator_range<pos_iterator_type>
        position_of(position_tagged const& ast) const
        {
            return
                boost::iterator_range<pos_iterator_type>(
                    positions.at(ast.id_first) // throws if out of range
                  , positions.at(ast.id_last)  // throws if out of range
                );
        }

        // This will catch all nodes except those inheriting from position_tagged
        template <typename AST>
        boost::iterator_range<pos_iterator_type>
        position_of(AST const& ast) const
        {
            // returns an empty position
            return boost::iterator_range<pos_iterator_type>();
        }

        // This will catch all nodes inheriting from position_tagged
        void annotate(position_tagged& ast, Iterator first, Iterator last) const
        {
            x.id_first = positions.size();
            positions.push_back(first);
            x.id_last =  = positions.size();
            positions.push_back(last);
        }

        // This will catch all nodes except those inheriting from position_tagged
        template <typename AST>
        void annotate(AST& ast, Iterator first, Iterator last) const
        {
            // (no-op) no need for tags
        }

        Container const&
        get_positions() const
        {
            return positions;
        }

        pos_iterator_type first() const { return first_; }
        pos_iterator_type last() const { return last_; }

    private

        Container positions;
        pos_iterator_type first_;
        pos_iterator_type last_;
    };

}}}

#endif
