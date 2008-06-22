// state_machine.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_STATE_MACHINE_HPP
#define BOOST_LEXER_STATE_MACHINE_HPP

#include <algorithm>
#include "char_state_machine.hpp"
#include "consts.hpp"
#include <deque>
#include <map>
#include "containers/ptr_vector.hpp"
#include "size_t.hpp"
#include <string>

namespace boost
{
namespace lexer
{
struct state_machine
{
    typedef std::vector<std::size_t> size_t_vector;
    typedef internal::ptr_vector<size_t_vector> size_t_vector_vector;

    size_t_vector_vector _lookup;
    size_t_vector _dfa_alphabet;
    size_t_vector_vector _dfa;
    bool _seen_BOL_assertion;
    bool _seen_EOL_assertion;

    state_machine () :
        _seen_BOL_assertion (false),
        _seen_EOL_assertion (false)
    {
    }

    bool empty () const
    {
        return _lookup->empty () && _dfa_alphabet.empty () &&
            _dfa->empty ();
    }

    void clear ()
    {
        _lookup.clear ();
        _dfa_alphabet.clear ();
        _dfa.clear ();
        _seen_BOL_assertion = false;
        _seen_EOL_assertion = false;
    }

    void swap (state_machine &sm_)
    {
        _lookup->swap (*sm_._lookup);
        _dfa_alphabet.swap (sm_._dfa_alphabet);
        _dfa->swap (*sm_._dfa);
        std::swap (_seen_BOL_assertion, sm_._seen_BOL_assertion);
        std::swap (_seen_EOL_assertion, sm_._seen_EOL_assertion);
    }

    template<typename CharT>
    void human_readable (basic_char_state_machine<CharT> &sm_) const
    {
        const std::size_t max_ = sizeof (CharT) == 1 ?
            num_chars : num_wchar_ts;
        const std::size_t start_states_ = _dfa->size ();

        sm_.clear ();
        sm_._sm_vector.resize (start_states_);

        for (std::size_t start_state_index_ = 0;
            start_state_index_ < start_states_; ++start_state_index_)
        {
            const size_t_vector *lu_ = _lookup[start_state_index_];
            const std::size_t alphabet_ = _dfa_alphabet[start_state_index_] - dfa_offset;
            std::vector<std::basic_string<CharT> > chars_ (alphabet_);
            const std::size_t states_ = _dfa[start_state_index_]->size () /
                (alphabet_ + dfa_offset);
            const std::size_t *read_ptr_ = &_dfa[start_state_index_]->
                front () + alphabet_ + dfa_offset;

            sm_._sm_vector[start_state_index_].resize (states_ - 1);

            for (std::size_t alpha_index_ = 0; alpha_index_ < max_;
                ++alpha_index_)
            {
                const std::size_t col_ = lu_->at (alpha_index_);

                if (col_ != dead_state_index)
                {
                    chars_[col_ - dfa_offset] += static_cast<CharT>
                        (alpha_index_);
                }
            }

            for (std::size_t state_index_ = 1; state_index_ < states_;
                ++state_index_)
            {
                typename basic_char_state_machine<CharT>::state *state_ =
                    &sm_._sm_vector[start_state_index_][state_index_ - 1];

                state_->_end_state = *read_ptr_ != 0;
                state_->_id = *(read_ptr_ + id_index);
                state_->_state = *(read_ptr_ + state_index);
                state_->_bol_index = *(read_ptr_ + bol_index) - 1;
                state_->_eol_index = *(read_ptr_ + eol_index) - 1;
                read_ptr_ += dfa_offset;

                for (std::size_t col_index_ = 0; col_index_ < alphabet_;
                    ++col_index_, ++read_ptr_)
                {
                    const std::size_t transition_ = *read_ptr_;

                    if (transition_ != 0)
                    {
                        const std::size_t i_ = transition_ - 1;
                        typename basic_char_state_machine<CharT>::state::
                            size_t_string_token_map::iterator iter_ =
                            state_->_transitions.find (i_);

                        if (iter_ == state_->_transitions.end ())
                        {
                            basic_string_token<CharT> token_
                                (false, chars_[col_index_]);
                            typename basic_char_state_machine<CharT>::
                                state::size_t_string_token_pair pair_
                                (i_, token_);

                            state_->_transitions.insert (pair_);
                        }
                        else
                        {
                            iter_->second._charset += chars_[col_index_];
                        }
                    }
                }

                for (typename basic_char_state_machine<CharT>::state::
                    size_t_string_token_map::iterator iter_ =
                    state_->_transitions.begin (),
                    end_ = state_->_transitions.end ();
                    iter_ != end_; ++iter_)
                {
                    std::sort (iter_->second._charset.begin (),
                        iter_->second._charset.end ());
                    iter_->second.normalise ();
                }
            }
        }
    }
};
}
}

#endif
