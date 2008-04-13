// examples/tokeniser.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_EXAMPLES_TOKENISER_HPP
#define BOOST_LEXER_EXAMPLES_TOKENISER_HPP

#include "char_traits.hpp"
#include "consts.hpp"
#include <boost/detail/iterator.hpp>
#include "size_t.hpp"
#include "state_machine.hpp"

namespace boost
{
namespace lexer
{
template<typename Iterator, typename Traits =
    char_traits<typename detail::iterator_traits<Iterator>::value_type> >
struct basic_tokeniser
{
    static std::size_t next (const state_machine &state_machine_,
        std::size_t &start_state_, const Iterator &start_,
        Iterator &start_token_, const Iterator &end_)
    {
        if (start_token_ == end_) return 0;

    again:
        const std::size_t * lookup_ = &state_machine_._lookup[start_state_]->
            front ();
        std::size_t dfa_alphabet_ = state_machine_._dfa_alphabet[start_state_];
        const std::size_t *dfa_ = &state_machine_._dfa[start_state_]->front ();
        const std::size_t *ptr_ = dfa_ + dfa_alphabet_;
        Iterator curr_ = start_token_;
        bool end_state_ = *ptr_ != 0;
        std::size_t id_ = *(ptr_ + id_index);
        Iterator end_token_ = start_token_;

        while (curr_ < end_)
        {
            const std::size_t BOL_state_ = ptr_[bol_index];
            const std::size_t EOL_state_ = ptr_[eol_index];

            if (BOL_state_ && (start_token_ == start_ ||
                *(start_token_ - 1) == '\n'))
            {
                ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];
            }
            else if (EOL_state_ && *curr_ == '\n')
            {
                ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
            }
            else
            {
                const std::size_t state_ =
                    ptr_[lookup_[static_cast<typename Traits::index_type>
                    (*curr_++)]];

                if (state_ == 0)
                {
                    break;
                }

                ptr_ = &dfa_[state_ * dfa_alphabet_];
            }

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                start_state_ = *(ptr_ + state_index);
                end_token_ = curr_;
            }
        }

        const std::size_t EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && curr_ == end_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                start_state_ = *(ptr_ + state_index);
                end_token_ = curr_;
            }
        }

        if (end_state_)
        {
            // return longest match
            start_token_ = end_token_;

            if (id_ == 0) goto again;
        }
        else
        {
            // No match causes char to be skipped
            ++start_token_;
            id_ = npos;
        }

        return id_;
    }

    static std::size_t next (const state_machine &state_machine_,
        std::size_t &start_state_, Iterator &start_token_,
        Iterator const &end_)
    {
        if (start_token_ == end_) return 0;

    again:
        const std::size_t * lookup_ = &state_machine_._lookup[start_state_]->
            front ();
        std::size_t dfa_alphabet_ = state_machine_._dfa_alphabet[start_state_];
        const std::size_t *dfa_ = &state_machine_._dfa[start_state_]->front ();
        const std::size_t *ptr_ = dfa_ + dfa_alphabet_;
        Iterator curr_ = start_token_;
        bool end_state_ = *ptr_ != 0;
        std::size_t id_ = *(ptr_ + id_index);
        Iterator end_token_ = start_token_;

        while (curr_ < end_)
        {
            const std::size_t state_ = ptr_[lookup_[static_cast
                <typename Traits::index_type>(*curr_++)]];

            if (state_ == 0)
            {
                break;
            }

            ptr_ = &dfa_[state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                start_state_ = *(ptr_ + state_index);
                end_token_ = curr_;
            }
        }

        if (end_state_)
        {
            // return longest match
            start_token_ = end_token_;

            if (id_ == 0) goto again;
        }
        else
        {
            // No match causes char to be skipped
            ++start_token_;
            id_ = npos;
        }

        return id_;
    }

    static std::size_t next (const std::size_t * const lookup_,
        const std::size_t dfa_alphabet_, const std::size_t * const dfa_,
        Iterator const &start_, Iterator &start_token_,
        Iterator const &end_)
    {
        if (start_token_ == end_) return 0;

        const std::size_t *ptr_ = dfa_ + dfa_alphabet_;
        Iterator curr_ = start_token_;
        bool end_state_ = *ptr_ != 0;
        std::size_t id_ = *(ptr_ + id_index);
        Iterator end_token_ = start_token_;

        while (curr_ < end_)
        {
            const std::size_t BOL_state_ = ptr_[bol_index];
            const std::size_t EOL_state_ = ptr_[eol_index];

            if (BOL_state_ && (start_token_ == start_ ||
                *(start_token_ - 1) == '\n'))
            {
                ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];
            }
            else if (EOL_state_ && *curr_ == '\n')
            {
                ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
            }
            else
            {
                const std::size_t state_ =
                    ptr_[lookup_[static_cast<typename Traits::index_type>
                    (*curr_++)]];

                if (state_ == 0)
                {
                    break;
                }

                ptr_ = &dfa_[state_ * dfa_alphabet_];
            }

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                end_token_ = curr_;
            }
        }

        const std::size_t EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && curr_ == end_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                end_token_ = curr_;
            }
        }

        if (end_state_)
        {
            // return longest match
            start_token_ = end_token_;
        }
        else
        {
            // No match causes char to be skipped
            ++start_token_;
            id_ = npos;
        }

        return id_;
    }

    static std::size_t next (const std::size_t * const lookup_,
        const std::size_t dfa_alphabet_, const std::size_t * const dfa_,
        Iterator &start_token_, Iterator const &end_)
    {
        if (start_token_ == end_) return 0;

        const std::size_t *ptr_ = dfa_ + dfa_alphabet_;
        Iterator curr_ = start_token_;
        bool end_state_ = *ptr_ != 0;
        std::size_t id_ = *(ptr_ + id_index);
        Iterator end_token_ = start_token_;

        while (curr_ < end_)
        {
            const std::size_t state_ = ptr_[lookup_[static_cast
                <typename Traits::index_type>(*curr_++)]];

            if (state_ == 0)
            {
                break;
            }

            ptr_ = &dfa_[state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                end_token_ = curr_;
            }
        }

        if (end_state_)
        {
            // return longest match
            start_token_ = end_token_;
        }
        else
        {
            // No match causes char to be skipped
            ++start_token_;
            id_ = npos;
        }

        return id_;
    }
};

typedef basic_tokeniser<std::string::iterator> iter_tokeniser;
typedef basic_tokeniser<std::wstring::iterator> iter_wtokeniser;
typedef basic_tokeniser<const char *> ptr_tokeniser;
typedef basic_tokeniser<const wchar_t *> ptr_wtokeniser;

}
}

#endif
