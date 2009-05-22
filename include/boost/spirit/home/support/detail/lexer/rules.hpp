// rules.hpp
// Copyright (c) 2007-2009 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_RULES_HPP
#define BOOST_LEXER_RULES_HPP

#include "consts.hpp"
#include <deque>
#include <locale>
#include <map>
#include "runtime_error.hpp"
#include <set>
#include "size_t.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace boost
{
namespace lexer
{
namespace detail
{
    // return name of initial state
    template <typename CharT>
    struct initial;

    template <>
    struct initial<char>
    {
        static const char *str ()
        {
            return "INITIAL";
        }
    };

    template <>
    struct initial<wchar_t>
    {
        static const wchar_t *str ()
        {
            return L"INITIAL";
        }
    };

    template <typename CharT>
    struct dot;

    template<>
    struct dot<char>
    {
        static const char *str ()
        {
            return ".";
        }
    };

    template<>
    struct dot<wchar_t>
    {
        static const wchar_t *str()
        {
            return L".";
        }
    };
}

template<typename CharT>
class basic_rules
{
public:
    typedef std::vector<std::size_t> id_vector;
    typedef std::deque<id_vector> id_vector_deque;
    typedef std::basic_string<CharT> string;
    typedef std::deque<string> string_deque;
    typedef std::deque<string_deque> string_deque_deque;
    typedef std::set<string> string_set;
    typedef std::pair<string, string> string_pair;
    typedef std::deque<string_pair> string_pair_deque;
    typedef std::map<string, std::size_t> string_size_t_map;
    typedef std::pair<string, std::size_t> string_size_t_pair;
    typedef std::pair<std::size_t, std::size_t> unique_id_key;
    typedef std::map<unique_id_key, std::size_t> unique_id_map;

    basic_rules (const regex_flags flags_ = dot_not_newline) :
        _flags (flags_)
    {
        add_state (initial ());
    }

    void clear ()
    {
        _statemap.clear ();
        _macrodeque.clear ();
        _macroset.clear ();
        _regexes.clear ();
        _ids.clear ();
        _states.clear ();
        _flags = dot_not_newline;
        _locale = std::locale ();
        add_state (initial ());
    }

    void clear (const CharT *state_name_)
    {
        std::size_t state_ = state (state_name_);

        if (state_ != npos)
        {
            _regexes[state_].clear ();
            _ids[state_].clear ();
            _states[state_].clear ();
        }
    }

    void flags (const regex_flags flags_)
    {
        _flags = flags_;
    }

    regex_flags flags () const
    {
        return _flags;
    }

    std::locale imbue (std::locale &locale_)
    {
        std::locale loc_ = _locale;

        _locale = locale_;
        return loc_;
    }

    const std::locale &locale () const
    {
        return _locale;
    }

    std::size_t state (const CharT *name_) const
    {
        std::size_t state_ = npos;
        typename string_size_t_map::const_iterator iter_ =
            _statemap.find (name_);

        if (iter_ != _statemap.end ())
        {
            state_ = iter_->second;
        }

        return state_;
    }

    const CharT *state (const std::size_t index_) const
    {
        if (index_ == 0)
        {
            return initial ();
        }
        else
        {
            return _lexer_state_names[index_ - 1].c_str ();
        }
    }

    std::size_t add_state (const CharT *name_)
    {
        validate (name_);

        if (_statemap.insert (string_size_t_pair (name_,
            _statemap.size ())).second)
        {
            _regexes.push_back (string_deque ());
            _ids.push_back (id_vector ());
            _states.push_back (id_vector ());

            if (string (name_) != initial ())
            {
                _lexer_state_names.push_back (name_);
            }
        }

        // Initial is not stored, so no need to - 1.
        return _lexer_state_names.size();
    }

    void add_macro (const CharT *name_, const CharT *regex_)
    {
        add_macro (name_, string (regex_));
    }

    void add_macro (const CharT *name_, const CharT *regex_start_,
        const CharT *regex_end_)
    {
        add_macro (name_, string (regex_start_, regex_end_));
    }

    void add_macro (const CharT *name_, const string &regex_)
    {
        validate (name_);

        typename string_set::const_iterator iter_ = _macroset.find (name_);

        if (iter_ == _macroset.end ())
        {
            _macrodeque.push_back (string_pair (name_, regex_));
            _macroset.insert (name_);
        }
        else
        {
            std::basic_stringstream<CharT> ss_;
            std::ostringstream os_;

            os_ << "Attempt to redefine MACRO '";

            while (*name_)
            {
                os_ << ss_.narrow (*name_++, static_cast<CharT> (' '));
            }

            os_ << "'.";
            throw runtime_error (os_.str ());
        }
    }

    std::size_t add (const CharT *regex_, const std::size_t id_)
    {
        return add (string (regex_), id_);
    }

    std::size_t add (const CharT *regex_start_, const CharT *regex_end_,
        const std::size_t id_)
    {
        return add (string (regex_start_, regex_end_), id_);
    }

    std::size_t add (const string &regex_, const std::size_t id_)
    {
        check_for_invalid_id (id_);
        _regexes[0].push_back (regex_);
        _ids[0].push_back (id_);
        _states[0].push_back (0);
        return _regexes[0].size () - 1;
    }

    void add (const CharT *curr_state_, const CharT *regex_,
        const CharT *new_state_, id_vector *id_vec_ = 0)
    {
        add (curr_state_, string (regex_), new_state_, id_vec_);
    }

    void add (const CharT *curr_state_, const CharT *regex_start_,
        const CharT *regex_end_, const CharT *new_state_,
        id_vector *id_vec_ = 0)
    {
        add (curr_state_, string (regex_start_, regex_end_),
            new_state_, id_vec_);
    }

    void add (const CharT *curr_state_, const string &regex_,
        const CharT *new_state_, id_vector *id_vec_ = 0)
    {
        add (curr_state_, regex_, 0, new_state_, false, id_vec_);
    }

    void add (const CharT *curr_state_, const CharT *regex_,
        const std::size_t id_, const CharT *new_state_, id_vector *id_vec_ = 0)
    {
        add (curr_state_, string (regex_), id_, new_state_, id_vec_);
    }

    void add (const CharT *curr_state_, const CharT *regex_start_,
        const CharT *regex_end_, const std::size_t id_,
        const CharT *new_state_, id_vector *id_vec_ = 0)
    {
        add (curr_state_, string (regex_start_, regex_end_), id_,
            new_state_, id_vec_);
    }

    void add (const CharT *curr_state_, const string &regex_,
        const std::size_t id_, const CharT *new_state_, id_vector *id_vec_ = 0)
    {
        add (curr_state_, regex_, id_, new_state_, true, id_vec_);
    }

    void add (const CharT *curr_state_, const basic_rules &rules_,
        id_vector *id_vec_ = 0)
    {
        const string_deque_deque &regexes_ = rules_.regexes ();
        const id_vector_deque &ids_ = rules_.ids ();
        typename string_deque_deque::const_iterator state_regex_iter_ =
            regexes_.begin ();
        typename string_deque_deque::const_iterator state_regex_end_ =
            regexes_.end ();
        typename id_vector_deque::const_iterator state_id_iter_ =
            ids_.begin ();
        typename string_deque::const_iterator regex_iter_;
        typename string_deque::const_iterator regex_end_;
        typename id_vector::const_iterator id_iter_;
        id_vector *temp_id_vec_ = id_vec_;

        for (; state_regex_iter_ != state_regex_end_; ++state_regex_iter_)
        {
            regex_iter_ = state_regex_iter_->begin ();
            regex_end_ = state_regex_iter_->end ();
            id_iter_ = state_id_iter_->begin ();

            for (; regex_iter_ != regex_end_; ++regex_iter_, ++id_iter_)
            {
                add (curr_state_, *regex_iter_, *id_iter_, detail::dot<CharT>::str(),
                    temp_id_vec_);

                if (temp_id_vec_)
                {
                    // As suggested by Hartmut, only fill the id_vec_ once.
                    // The dfa sizes can be examined at the end to get a range
                    // of ids.
                    temp_id_vec_ = 0;
                }
            }
        }
    }

    const string_size_t_map &statemap () const
    {
        return _statemap;
    }

    const string_pair_deque &macrodeque () const
    {
        return _macrodeque;
    }

    const string_deque_deque &regexes () const
    {
        return _regexes;
    }

    const id_vector_deque &ids () const
    {
        return _ids;
    }

    const id_vector_deque &states () const
    {
        return _states;
    }

    bool empty () const
    {
        typename string_deque_deque::const_iterator iter_ = _regexes.begin ();
        typename string_deque_deque::const_iterator end_ = _regexes.end ();
        bool empty_ = true;

        for (; iter_ != end_; ++iter_)
        {
            if (!iter_->empty ())
            {
                empty_ = false;
                break;
            }
        }

        return empty_;
    }

    static const CharT *initial ()
    {
        return detail::initial<CharT>::str ();
    }

    std::size_t retrieve_id (std::size_t state, std::size_t id) const
    {
        unique_id_key key (state, id);
        typename unique_id_map::const_iterator it = _unique_ids.find (key);

        if (it == _unique_ids.end ())
        {
            return npos;
        }

        return (*it).second;
    }

private:
    string_size_t_map _statemap;
    string_pair_deque _macrodeque;
    string_set _macroset;
    string_deque_deque _regexes;
    id_vector_deque _ids;
    id_vector_deque _states;
    regex_flags _flags;
    std::locale _locale;
    string_deque _lexer_state_names;
    unique_id_map _unique_ids;

    void add (const CharT *curr_state_, const string &regex_,
        const std::size_t id_, const CharT *new_state_, const bool check_,
        id_vector *id_vec_ = 0)
    {
        const bool star_ = *curr_state_ == '*' && *(curr_state_ + 1) == 0;
        const bool dot_ = *new_state_ == '.' && *(new_state_ + 1) == 0;

        if (id_vec_)
        {
            id_vec_->clear();
        }

        if (check_)
        {
            check_for_invalid_id (id_);
        }

        if (!dot_)
        {
            validate (new_state_);
        }

        std::size_t new_ = string::npos;
        typename string_size_t_map::const_iterator iter_;
        typename string_size_t_map::const_iterator end_ = _statemap.end ();
        id_vector states_;

        if (!dot_)
        {
            iter_ = _statemap.find (new_state_);

            if (iter_ == end_)
            {
                std::basic_stringstream<CharT> ss_;
                std::ostringstream os_;

                os_ << "Unknown state name '";

                while (*new_state_)
                {
                    os_ << ss_.narrow (*new_state_++, ' ');
                }

                os_ << "'.";
                throw runtime_error (os_.str ());
            }

            new_ = iter_->second;
        }

        if (star_)
        {
            const std::size_t size_ = _statemap.size ();

            for (std::size_t i_ = 0; i_ < size_; ++i_)
            {
                states_.push_back (i_);
            }
        }
        else
        {
            const CharT *start_ = curr_state_;
            string state_;

            while (*curr_state_)
            {
                while (*curr_state_ && *curr_state_ != ',')
                {
                    ++curr_state_;
                }

                state_.assign (start_, curr_state_);

                if (*curr_state_)
                {
                    ++curr_state_;
                    start_ = curr_state_;
                }

                validate (state_.c_str ());
                iter_ = _statemap.find (state_.c_str ());

                if (iter_ == end_)
                {
                    std::basic_stringstream<CharT> ss_;
                    std::ostringstream os_;

                    os_ << "Unknown state name '";
                    curr_state_ = state_.c_str ();

                    while (*curr_state_)
                    {
                        os_ << ss_.narrow (*curr_state_++, ' ');
                    }

                    os_ << "'.";
                    throw runtime_error (os_.str ());
                }

                states_.push_back (iter_->second);
            }
        }

        for (std::size_t i_ = 0, size_ = states_.size (); i_ < size_; ++i_)
        {
            const std::size_t curr_ = states_[i_];

            _regexes[curr_].push_back (regex_);
            _ids[curr_].push_back (id_);
            _states[curr_].push_back (dot_ ? curr_ : new_);

            if (id_vec_)
            {
                id_vec_->push_back (_regexes[curr_].size () - 1);
            }

            map_id (dot_ ? curr_ : new_, id_, _regexes[curr_].size () - 1);
        }
    }

    void validate (const CharT *name_) const
    {
        const CharT *start_ = name_;

        if (*name_ != '_' && !(*name_ >= 'A' && *name_ <= 'Z') &&
            !(*name_ >= 'a' && *name_ <= 'z'))
        {
            std::basic_stringstream<CharT> ss_;
            std::ostringstream os_;

            os_ << "Invalid name '";

            while (*name_)
            {
                os_ << ss_.narrow (*name_++, ' ');
            }

            os_ << "'.";
            throw runtime_error (os_.str ());
        }
        else if (*name_)
        {
            ++name_;
        }

        while (*name_)
        {
            if (*name_ != '_' && *name_ != '-' &&
                !(*name_ >= 'A' && *name_ <= 'Z') &&
                !(*name_ >= 'a' && *name_ <= 'z') &&
                !(*name_ >= '0' && *name_ <= '9'))
            {
                std::basic_stringstream<CharT> ss_;
                std::ostringstream os_;

                os_ << "Invalid name '";
                name_ = start_;

                while (*name_)
                {
                    os_ << ss_.narrow (*name_++, ' ');
                }

                os_ << "'.";
                throw runtime_error (os_.str ());
            }

            ++name_;
        }

        if (name_ - start_ > static_cast<std::ptrdiff_t>(max_macro_len))
        {
            std::basic_stringstream<CharT> ss_;
            std::ostringstream os_;

            os_ << "Name '";
            name_ = start_;

            while (*name_)
            {
                os_ << ss_.narrow (*name_++, ' ');
            }

            os_ << "' too long.";
            throw runtime_error (os_.str ());
        }
    }

    void check_for_invalid_id (const std::size_t id_) const
    {
        switch (id_)
        {
        case 0:
            throw runtime_error ("id 0 is reserved for EOF.");
        case npos:
            throw runtime_error ("id npos is reserved for the "
                "UNKNOWN token.");
        default:
            // OK
            break;
        }
    }

    bool map_id (std::size_t state, std::size_t id, std::size_t unique_id)
    {
        typedef typename unique_id_map::iterator iterator_type;

        unique_id_key key (state, id);
        iterator_type it = _unique_ids.find (key);
        if (it != _unique_ids.end ())
        {
            (*it).second = unique_id;
            return false;
        }

        typedef typename unique_id_map::value_type value_type;
        return _unique_ids.insert (value_type (key, unique_id)).second;
    }
};

typedef basic_rules<char> rules;
typedef basic_rules<wchar_t> wrules;
}
}

#endif
