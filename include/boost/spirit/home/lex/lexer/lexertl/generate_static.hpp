//  Copyright (c) 2008 Ben Hanson
//  Copyright (c) 2008-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_LEX_LEXERTL_GENERATE_CPP_FEB_10_2008_0855PM)
#define BOOST_SPIRIT_LEX_LEXERTL_GENERATE_CPP_FEB_10_2008_0855PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/detail/lexer/char_traits.hpp>
#include <boost/spirit/home/support/detail/lexer/consts.hpp>
#include <boost/spirit/home/support/detail/lexer/rules.hpp>
#include <boost/spirit/home/support/detail/lexer/size_t.hpp>
#include <boost/spirit/home/support/detail/lexer/state_machine.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/static_version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace lex { namespace lexertl 
{
    namespace detail
    {

    inline bool
    generate_delimiter(std::ostream &os_)
    {
        os_ << std::string(80, '/') << "\n";
        return os_.good();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Generate a table of the names of the used lexer states, which is a bit 
    // tricky, because the table stored with the rules is sorted based on the 
    // names, but we need it sorted using the state ids.
    template <typename Char>
    inline bool 
    generate_cpp_state_info (boost::lexer::basic_rules<Char> const& rules_
      , std::ostream &os_, char const* name_suffix = "")
    {
        // we need to re-sort the state names in ascending order of the state 
        // ids, filling possible gaps in between later
        typedef typename 
            boost::lexer::basic_rules<Char>::string_size_t_map::const_iterator
        state_iterator;
        typedef std::map<std::size_t, char const*> reverse_state_map_type;

        reverse_state_map_type reverse_state_map;
        state_iterator send = rules_.statemap().end();
        for (state_iterator sit = rules_.statemap().begin(); sit != send; ++sit)
        {
            typedef typename reverse_state_map_type::value_type value_type;
            reverse_state_map.insert(value_type((*sit).second, (*sit).first.c_str()));
        }

        generate_delimiter(os_);
        os_ << "// this table defines the names of the lexer states\n";
        os_ << "char const* const lexer_state_names" 
            << (name_suffix[0] ? "_" : "") << name_suffix
            << "[" << rules_.statemap().size() << "] = \n{\n";

        typedef typename reverse_state_map_type::iterator iterator;
        iterator rend = reverse_state_map.end();
        std::size_t last_id = 0;
        for (iterator rit = reverse_state_map.begin(); rit != rend; ++last_id)
        {
            for (/**/; last_id < (*rit).first; ++last_id)
            {
                os_ << "    0,  // \"<undefined state>\"\n";
            }
            os_ << "    \"" << (*rit).second << "\"";
            if (++rit != rend)
                os_ << ",\n";
            else
                os_ << "\n";        // don't generate the final comma
        }
        os_ << "};\n\n";

        generate_delimiter(os_);
        os_ << "// this variable defines the number of lexer states\n";
        os_ << "std::size_t const lexer_state_count" 
            << (name_suffix[0] ? "_" : "") << name_suffix
            << " = " << rules_.statemap().size() << ";\n\n";
        return os_.good();
    }

    inline bool 
    generate_cpp_state_table (std::ostream &os_, char const* name_suffix = "")
    {
        std::string suffix(name_suffix[0] ? "_" : "");
        suffix += name_suffix;

        generate_delimiter(os_);
        os_ << "// this defines a generic accessors for the information above\n";
        os_ << "struct lexer" << suffix << "\n{\n";
        os_ << "    // version number of compatible static lexer engine\n";
        os_ << "    enum { static_version = " 
            << boost::lexical_cast<std::string>(SPIRIT_STATIC_LEXER_VERSION) << " };\n\n";
        os_ << "    // return the number of lexer states\n";
        os_ << "    static std::size_t state_count()\n";
        os_ << "    {\n        return lexer_state_count" << suffix << "; \n    }\n\n";
        os_ << "    // return the name of the lexer state as given by 'idx'\n";
        os_ << "    static char const* state_name(std::size_t idx)\n";
        os_ << "    {\n        return lexer_state_names" << suffix << "[idx]; \n    }\n\n";
        os_ << "    // return the next matched token\n";
        os_ << "    template<typename Iterator>\n";
        os_ << "    static std::size_t next(std::size_t &start_state_, Iterator const& start_\n";
        os_ << "      , Iterator &start_token_, Iterator const& end_, std::size_t& unique_id_)\n";
        os_ << "    {\n        return next_token" << suffix 
            << "(start_state_, start_, start_token_, end_, unique_id_);\n    }\n";
        os_ << "};\n\n";
        return os_.good();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Generate a tokenizer for the given state machine.
    template <typename Char>
    inline bool 
    generate_cpp (boost::lexer::basic_state_machine<Char> const& sm_
      , boost::lexer::basic_rules<Char> const& rules_
      , std::ostream &os_, char const* name_suffix = ""
      , bool skip_on_nomatch = true, bool optimize_parameters = true)
    {
        if (sm_.data()._lookup->empty())
            return false;

        std::size_t const dfas_ = sm_.data()._dfa->size();
        std::size_t const lookups_ = sm_.data()._lookup->front()->size();

        os_ << "// Copyright (c) 2008-2009 Ben Hanson\n";
        os_ << "// Copyright (c) 2008-2009 Hartmut Kaiser\n";
        os_ << "//\n";
        os_ << "// Distributed under the Boost Software License, "
            "Version 1.0. (See accompanying\n";
        os_ << "// file licence_1_0.txt or copy at "
            "http://www.boost.org/LICENSE_1_0.txt)\n\n";
        os_ << "// Auto-generated by boost::lexer, do not edit\n\n";

        std::string guard(name_suffix);
        guard += name_suffix[0] ? "_" : "";
        guard += __DATE__ "_" __TIME__;
        std::string::size_type p = guard.find_first_of(": ");
        while (std::string::npos != p) 
        {
            guard.replace(p, 1, "_");
            p = guard.find_first_of(": ", p);
        }
        boost::to_upper(guard);

        os_ << "#if !defined(BOOST_SPIRIT_LEXER_NEXT_TOKEN_" << guard << ")\n";
        os_ << "#define BOOST_SPIRIT_LEXER_NEXT_TOKEN_" << guard << "\n\n";

        os_ << "#include <boost/detail/iterator.hpp>\n";
        os_ << "#include <boost/spirit/home/support/detail/lexer/char_traits.hpp>\n\n";

        generate_delimiter(os_);
        os_ << "// the generated table of state names and the tokenizer have to be\n"
               "// defined in the boost::spirit::lex::lexertl::static_ namespace\n";
        os_ << "namespace boost { namespace spirit { namespace lex { "
            "namespace lexertl { namespace static_ {\n\n";

        // generate the lexer state information variables
        if (!generate_cpp_state_info(rules_, os_, name_suffix))
            return false;

        generate_delimiter(os_);
        os_ << "// this function returns the next matched token\n";
        os_ << "template<typename Iterator>\n";
        os_ << "std::size_t next_token" << (name_suffix[0] ? "_" : "") 
            << name_suffix  << " (";

        if (dfas_ > 1)
        {
            os_ << "std::size_t& start_state_, ";
        }
        else if (!optimize_parameters)
        {
            os_ << "std::size_t& /*start_state_*/, ";
        }

        if (sm_.data()._seen_BOL_assertion)
        {
            os_ << "Iterator const& start_, ";
        }
        else if (!optimize_parameters)
        {
            os_ << "Iterator const& /*start_*/, ";
        }

        if (dfas_ > 1 || sm_.data()._seen_BOL_assertion || !optimize_parameters)
        {
            os_ << "\n    ";
        }

        os_ << "Iterator &start_token_, Iterator const& end_, ";
        os_ << "std::size_t& unique_id_)\n";
        os_ << "{\n";
        os_ << "    enum {end_state_index, id_index, unique_id_index, "
          "state_index, bol_index,\n";
        os_ << "        eol_index, dead_state_index, dfa_offset};\n\n";
        os_ << "    static const std::size_t npos = static_cast"
          "<std::size_t>(~0);\n";

        if (dfas_ > 1)
        {
            for (std::size_t state_ = 0; state_ < dfas_; ++state_)
            {
                std::size_t i_ = 0;
                std::size_t j_ = 1;
                std::size_t count_ = lookups_ / 8;
                std::size_t const* lookup_ = &sm_.data()._lookup[state_]->front();
                std::size_t const* dfa_ = &sm_.data()._dfa[state_]->front();

                os_ << "    static const std::size_t lookup" << state_ 
                    << "_[" << lookups_ << "] = {\n        ";

                for (/**/; i_ < count_; ++i_)
                {
                    const std::size_t index_ = i_ * 8;

                    os_ << lookup_[index_];

                    for (; j_ < 8; ++j_)
                    {
                        os_ << ", " << lookup_[index_ + j_];
                    }

                    if (i_ < count_ - 1)
                    {
                        os_ << ",\n        ";
                    }

                    j_ = 1;
                }

                os_ << " };\n";
                count_ = sm_.data()._dfa[state_]->size ();
                os_ << "    static const std::size_t dfa" << state_ << "_[" <<
                    count_ << "] = {\n        ";
                count_ /= 8;

                for (i_ = 0; i_ < count_; ++i_)
                {
                    const std::size_t index_ = i_ * 8;

                    os_ << dfa_[index_];

                    for (j_ = 1; j_ < 8; ++j_)
                    {
                        os_ << ", " << dfa_[index_ + j_];
                    }

                    if (i_ < count_ - 1)
                    {
                        os_ << ",\n        ";
                    }
                }

                std::size_t const mod_ = sm_.data()._dfa[state_]->size () % 8;
                if (mod_)
                {
                    const std::size_t index_ = count_ * 8;

                    if (count_)
                    {
                        os_ << ",\n        ";
                    }

                    os_ << dfa_[index_];

                    for (j_ = 1; j_ < mod_; ++j_)
                    {
                        os_ << ", " << dfa_[index_ + j_];
                    }
                }

                os_ << " };\n";
            }

            std::size_t count_ = sm_.data()._dfa_alphabet.size();
            std::size_t i_ = 1;

            os_ << "    static const std::size_t *lookup_arr_[" << count_ <<
                "] = { lookup0_";

            for (i_ = 1; i_ < count_; ++i_)
            {
                os_ << ", " << "lookup" << i_ << "_";
            }

            os_ << " };\n";
            os_ << "    static const std::size_t dfa_alphabet_arr_[" << 
                count_ << "] = { ";

            os_ << sm_.data()._dfa_alphabet.front ();

            for (i_ = 1; i_ < count_; ++i_)
            {
                os_ << ", " << sm_.data()._dfa_alphabet[i_];
            }

            os_ << " };\n";
            os_ << "    static const std::size_t *dfa_arr_[" << count_ <<
                "] = { ";
            os_ << "dfa0_";

            for (i_ = 1; i_ < count_; ++i_)
            {
                os_ << ", " << "dfa" << i_ << "_";
            }

            os_ << " };\n";
        }
        else
        {
            std::size_t const* lookup_ = &sm_.data()._lookup[0]->front();
            std::size_t const* dfa_ = &sm_.data()._dfa[0]->front();
            std::size_t i_ = 0;
            std::size_t j_ = 1;
            std::size_t count_ = lookups_ / 8;

            os_ << "    static const std::size_t lookup_[";
            os_ << sm_.data()._lookup[0]->size() << "] = {\n        ";

            for (; i_ < count_; ++i_)
            {
                const std::size_t index_ = i_ * 8;

                os_ << lookup_[index_];

                for (; j_ < 8; ++j_)
                {
                    os_ << ", " << lookup_[index_ + j_];
                }

                if (i_ < count_ - 1)
                {
                    os_ << ",\n        ";
                }

                j_ = 1;
            }

            os_ << " };\n";
            os_ << "    static const std::size_t dfa_alphabet_ = " <<
              sm_.data()._dfa_alphabet.front () << ";\n";
            os_ << "    static const std::size_t dfa_[" <<
              sm_.data()._dfa[0]->size () << "] = {\n        ";
                count_ = sm_.data()._dfa[0]->size () / 8;

            for (i_ = 0; i_ < count_; ++i_)
            {
                const std::size_t index_ = i_ * 8;

                os_ << dfa_[index_];

                for (j_ = 1; j_ < 8; ++j_)
                {
                    os_ << ", " << dfa_[index_ + j_];
                }

                if (i_ < count_ - 1)
                {
                    os_ << ",\n        ";
                }
            }

            const std::size_t mod_ = sm_.data()._dfa[0]->size () % 8;

            if (mod_)
            {
                const std::size_t index_ = count_ * 8;

                if (count_)
                {
                    os_ << ",\n        ";
                }

                os_ << dfa_[index_];

                for (j_ = 1; j_ < mod_; ++j_)
                {
                    os_ << ", " << dfa_[index_ + j_];
                }
            }

            os_ << " };\n";
        }

        os_ << "\n    if (start_token_ == end_) return 0;\n\n";

        if (dfas_ > 1)
        {
            os_ << "again:\n";
            os_ << "    const std::size_t * lookup_ = lookup_arr_[start_state_];\n";
            os_ << "    std::size_t dfa_alphabet_ = dfa_alphabet_arr_[start_state_];\n";
            os_ << "    const std::size_t *dfa_ = dfa_arr_[start_state_];\n";
        }

        os_ << "    const std::size_t *ptr_ = dfa_ + dfa_alphabet_;\n";
        os_ << "    Iterator curr_ = start_token_;\n";
        os_ << "    bool end_state_ = *ptr_ != 0;\n";
        os_ << "    std::size_t id_ = *(ptr_ + id_index);\n";
        os_ << "    std::size_t uid_ = *(ptr_ + unique_id_index);\n";
        os_ << "    Iterator end_token_ = start_token_;\n\n";

        os_ << "    while (curr_ != end_)\n";
        os_ << "    {\n";

        if (sm_.data()._seen_BOL_assertion)
        {
            os_ << "        std::size_t const BOL_state_ = ptr_[bol_index];\n\n";
        }

        if (sm_.data()._seen_EOL_assertion)
        {
            os_ << "        std::size_t const EOL_state_ = ptr_[eol_index];\n\n";
        }

        if (sm_.data()._seen_BOL_assertion && sm_.data()._seen_EOL_assertion)
        {
            os_ << "        if (BOL_state_ && (start_token_ == start_ ||\n";
            os_ << "            *(start_token_ - 1) == '\\n'))\n";
            os_ << "        {\n";
            os_ << "            ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];\n";
            os_ << "        }\n";
            os_ << "        else if (EOL_state_ && *curr_ == '\\n')\n";
            os_ << "        {\n";
            os_ << "            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n";
            os_ << "        }\n";
            os_ << "        else\n";
            os_ << "        {\n";
            os_ << "            std::size_t const state_ =\n";

            if (lookups_ == 256)
            {
                os_ << "                ptr_[lookup_[<typename Traits::index_type>"
                    "(*curr_++)]];\n";
            }
            else
            {
                os_ << "                ptr_[lookup_[*curr_++]];\n";
            }

            os_ << '\n';
            os_ << "            if (state_ == 0) break;\n";
            os_ << '\n';
            os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
            os_ << "        }\n\n";
        }
        else if (sm_.data()._seen_BOL_assertion)
        {
            os_ << "        if (BOL_state_ && (start_token_ == start_ ||\n";
            os_ << "            *(start_token_ - 1) == '\\n'))\n";
            os_ << "        {\n";
            os_ << "            ptr_ = &dfa_[BOL_state_ * dfa_alphabet_];\n";
            os_ << "        }\n";
            os_ << "        else\n";
            os_ << "        {\n";
            os_ << "            std::size_t const state_ =\n";

            if (lookups_ == 256)
            {
                os_ << "                ptr_[lookup_[static_cast<unsigned char>"
                       "(*curr_++)]];\n";
            }
            else
            {
                os_ << "                ptr_[lookup_[*curr_++]];\n";
            }

            os_ << '\n';
            os_ << "            if (state_ == 0) break;\n";
            os_ << '\n';
            os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
            os_ << "        }\n\n";
        }
        else if (sm_.data()._seen_EOL_assertion)
        {
            os_ << "        if (EOL_state_ && *curr_ == '\\n')\n";
            os_ << "        {\n";
            os_ << "            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n";
            os_ << "        }\n";
            os_ << "        else\n";
            os_ << "        {\n";
            os_ << "            std::size_t const state_ =\n";

            if (lookups_ == 256)
            {
                os_ << "                ptr_[lookup_[static_cast<unsigned char>"
                       "(*curr_++)]];\n";
            }
            else
            {
                os_ << "                ptr_[lookup_[*curr_++]];\n";
            }

            os_ << '\n';
            os_ << "            if (state_ == 0) break;\n";
            os_ << '\n';
            os_ << "            ptr_ = &dfa_[state_ * dfa_alphabet_];\n";
            os_ << "        }\n\n";
        }
        else
        {
            os_ << "        std::size_t const state_ =\n";

            if (lookups_ == 256)
            {
                os_ << "            ptr_[lookup_[static_cast<unsigned char>"
                       "(*curr_++)]];\n";
            }
            else
            {
                os_ << "            ptr_[lookup_[*curr_++]];\n";
            }

            os_ << '\n';
            os_ << "        if (state_ == 0) break;\n";
            os_ << '\n';
            os_ << "        ptr_ = &dfa_[state_ * dfa_alphabet_];\n\n";
        }

        os_ << "        if (*ptr_)\n";
        os_ << "        {\n";
        os_ << "            end_state_ = true;\n";
        os_ << "            id_ = *(ptr_ + id_index);\n";
        os_ << "            uid_ = *(ptr_ + unique_id_index);\n";

        if (dfas_ > 1)
        {
            os_ << "            start_state_ = *(ptr_ + state_index);\n";
        }

        os_ << "            end_token_ = curr_;\n";
        os_ << "        }\n";
        os_ << "    }\n\n";

        if (sm_.data()._seen_EOL_assertion)
        {
            os_ << "    const std::size_t EOL_state_ = ptr_[eol_index];\n\n";

            os_ << "    if (EOL_state_ && curr_ == end_)\n";
            os_ << "    {\n";
            os_ << "        ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];\n\n";

            os_ << "        if (*ptr_)\n";
            os_ << "        {\n";
            os_ << "            end_state_ = true;\n";
            os_ << "            id_ = *(ptr_ + id_index);\n";
            os_ << "            uid_ = *(ptr_ + unique_id_index);\n";

            if (dfas_ > 1)
            {
                os_ << "            start_state_ = *(ptr_ + state_index);\n";
            }

            os_ << "            end_token_ = curr_;\n";
            os_ << "        }\n";
            os_ << "    }\n\n";
        }

        os_ << "    if (end_state_)\n";
        os_ << "    {\n";
        os_ << "        // return longest match\n";
        os_ << "        start_token_ = end_token_;\n";

        if (dfas_ > 1)
        {
            os_ << "        if (id_ == 0) goto again;\n";
        }

        os_ << "    }\n";
        os_ << "    else\n";
        os_ << "    {\n";

        if (skip_on_nomatch)
        {
            os_ << "        // No match causes char to be skipped\n";
            os_ << "        ++start_token_;\n";
        }

        os_ << "        id_ = npos;\n";
        os_ << "        uid_ = npos;\n";
        os_ << "    }\n\n";

        os_ << "    unique_id_ = uid_;\n";
        os_ << "    return id_;\n";
        os_ << "}\n\n";

        if (!generate_cpp_state_table(os_, name_suffix))
            return false;

        os_ << "}}}}}  // namespace boost::spirit::lex::lexertl::static_\n\n";

        os_ << "#endif\n";

        return os_.good();
    }

    }   // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    template <typename Lexer>
    inline bool 
    generate_static(Lexer const& lexer, std::ostream& os
      , char const* name_suffix = "")
    {
        if (!lexer.init_dfa())
            return false;
        return detail::generate_cpp(lexer.state_machine_, lexer.rules_, os
          , name_suffix, false, false);
    }

///////////////////////////////////////////////////////////////////////////////
}}}}

#endif
