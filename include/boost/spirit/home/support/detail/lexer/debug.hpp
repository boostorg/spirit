// debug.hpp
// Copyright (c) 2007 Ben Hanson
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_LEXER_DEBUG_HPP
#define BOOST_LEXER_DEBUG_HPP

#include <map>
#include <ostream>
#include "size_t.hpp"
#include "state_machine.hpp"
#include "string_token.hpp"
#include <vector>

namespace boost
{
namespace lexer
{
template<typename CharT>
class basic_debug
{
public:
    typedef std::basic_ostream<CharT> ostream;
    typedef std::basic_string<CharT> string;
    typedef std::vector<std::size_t> size_t_vector;

    static void escape_control_chars (const string &in_, string &out_)
    {
        const CharT *ptr_ = in_.c_str ();
        std::size_t size_ = in_.size ();

#if defined _MSC_VER && _MSC_VER <= 1200
        out_.erase ();
#else
        out_.clear ();
#endif

        while (size_)
        {
            switch (*ptr_)
            {
                case '\0':
                    out_ += '\\';
                    out_ += '0';
                    break;
                case '\a':
                    out_ += '\\';
                    out_ += 'a';
                    break;
                case '\b':
                    out_ += '\\';
                    out_ += 'b';
                    break;
                case 27:
                    out_ += '\\';
                    out_ += 'x';
                    out_ += '1';
                    out_ += 'b';
                    break;
                case '\f':
                    out_ += '\\';
                    out_ += 'f';
                    break;
                case '\n':
                    out_ += '\\';
                    out_ += 'n';
                    break;
                case '\r':
                    out_ += '\\';
                    out_ += 'r';
                    break;
                case '\t':
                    out_ += '\\';
                    out_ += 't';
                    break;
                case '\v':
                    out_ += '\\';
                    out_ += 'v';
                    break;
                case '\\':
                    out_ += '\\';
                    out_ += '\\';
                    break;
                case '"':
                    out_ += '\\';
                    out_ += '"';
                    break;
                default:
                {
                    if (*ptr_ < 32 && *ptr_ >= 0)
                    {
                        stringstream ss_;

                        out_ += '\\';
                        out_ += 'x';
                        ss_ << std::hex <<
                            static_cast<std::size_t> (*ptr_);
                        out_ += ss_.str ();
                    }
                    else
                    {
                        out_ += *ptr_;
                    }

                    break;
                }
            }

            ++ptr_;
            --size_;
        }
    }

    static void dump (const state_machine &state_machine_, ostream &stream_)
    {
        basic_char_state_machine<CharT> char_state_machine_;

        state_machine_.human_readable (char_state_machine_);

        for (std::size_t state_ = 0,
            states_ = char_state_machine_._sm_vector.size ();
            state_ < states_; ++state_)
        {
            const typename basic_char_state_machine<CharT>::state *ptr_ =
                &char_state_machine_._sm_vector[state_].front ();
            const std::size_t size_ = char_state_machine_.
                _sm_vector[state_].size ();

            for (std::size_t i_ = 0; i_ < size_; ++i_, ++ptr_)
            {
                state (stream_);
                stream_ << i_ << std::endl;

                if (ptr_->_end_state)
                {
                    end_state (stream_);
                    stream_ << ptr_->_id;
                    dfa (stream_);
                    stream_ << ptr_->_state;
                    stream_ << std::endl;
                }

                if (ptr_->_bol_index != npos)
                {
                    bol (stream_);
                    stream_ << ptr_->_bol_index << std::endl;
                }

                if (ptr_->_eol_index != npos)
                {
                    eol (stream_);
                    stream_ << ptr_->_eol_index << std::endl;
                }

                for (typename basic_char_state_machine<CharT>::state::
                    size_t_string_token_map::const_iterator iter_ = ptr_->
                    _transitions.begin (), end_ = ptr_->_transitions.end ();
                    iter_ != end_; ++iter_)
                {
                    std::size_t transition_ = iter_->first;

                    if (iter_->second.any ())
                    {
                        any (stream_);
                    }
                    else
                    {
                        open_bracket (stream_);

                        if (iter_->second._negated)
                        {
                            negated (stream_);
                        }

                        string charset_;

                        escape_control_chars (iter_->second._charset,
                            charset_);
                        stream_ << charset_;
                        close_bracket (stream_);
                    }

                    stream_ << transition_ << std::endl;
                }

                stream_ << std::endl;
            }
        }
    }

protected:
    typedef std::basic_stringstream<CharT> stringstream;

    static void state (std::basic_ostream<char> &stream_)
    {
        stream_ << "State: ";
    }

    static void state (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"State: ";
    }

    static void bol (std::basic_ostream<char> &stream_)
    {
        stream_ << "  BOL -> ";
    }

    static void bol (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"  BOL -> ";
    }

    static void eol (std::basic_ostream<char> &stream_)
    {
        stream_ << "  EOL -> ";
    }

    static void eol (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"  EOL -> ";
    }

    static void end_state (std::basic_ostream<char> &stream_)
    {
        stream_ << "  END STATE, Id = ";
    }

    static void end_state (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"  END STATE, Id = ";
    }

    static void any (std::basic_ostream<char> &stream_)
    {
        stream_ << "  . -> ";
    }

    static void any (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"  . -> ";
    }

    static void open_bracket (std::basic_ostream<char> &stream_)
    {
        stream_ << "  [";
    }

    static void open_bracket (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"  [";
    }

    static void negated (std::basic_ostream<char> &stream_)
    {
        stream_ << "^";
    }

    static void negated (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"^";
    }

    static void close_bracket (std::basic_ostream<char> &stream_)
    {
        stream_ << "] -> ";
    }

    static void close_bracket (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L"] -> ";
    }

    static void dfa (std::basic_ostream<char> &stream_)
    {
        stream_ << ", dfa = ";
    }

    static void dfa (std::basic_ostream<wchar_t> &stream_)
    {
        stream_ << L", dfa = ";
    }
};

typedef basic_debug<char> debug;
typedef basic_debug<wchar_t> wdebug;
}
}

#endif
