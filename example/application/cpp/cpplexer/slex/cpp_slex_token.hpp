/*=============================================================================
    A Standard compliant C++ preprocessor

    SLex (Spirit Lex) based C++ lexer token definition
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_SLEX_TOKEN_HPP__53A13BD2_FBAA_444B_9B8B_FCB225C2BBA8__INCLUDED_)
#define _CPP_SLEX_TOKEN_HPP__53A13BD2_FBAA_444B_9B8B_FCB225C2BBA8__INCLUDED_

#include <boost/spirit/iterator/position_iterator.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace slex {

///////////////////////////////////////////////////////////////////////////////
//  
//  new_lexer_gen
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT, typename PositionT = boost::spirit::file_position>
class lex_token;

template <typename IteratorT, typename PositionT = boost::spirit::file_position>
struct new_lexer_gen
{
//  The NewLexer function allows the opaque generation of a new lexer object.
//  It is coupled to the token type to allow to decouple the lexer/token 
//  configurations at compile time.
    static lex_input_interface<lex_token<IteratorT, PositionT> > *
    new_lexer(IteratorT const &first, IteratorT const &last, 
        PositionT const &pos);
};

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_token
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT, typename PositionT>
class lex_token 
{
public:
    typedef typename std::iterator_traits<IteratorT>::value_type    char_t;
    typedef IteratorT                           iterator_t;
    typedef std::basic_string<char_t>           string_t;
    typedef PositionT                           position_t;
    typedef new_lexer_gen<IteratorT, PositionT> lexer_gen_t;
    
    lex_token()
    :   id(T_EOF)
    {}
    
    lex_token(token_id id_, string_t const &value_, PositionT const &pos_)
    :   id(id_), value(value_), pos(pos_)
    {}

// accessors
    operator token_id() const { return id; }
    string_t const &get_value() const { return value; }
    position_t const &get_position() const { return pos; }
    void set_token_id (token_id id_) { id = id_; }
    void set_value (string_t const &newval) { value = newval; }
    void set_position (position_t const &pos_) { pos = pos_; }

// debug support    
#if defined(CPP_DUMP_PARSE_TREE)
// access functions for the tree_to_xml functionality
    static int get_token_id(lex_token const &t) 
        { return ID_FROM_TOKEN(token_id(t)); }
    static string_t get_token_value(lex_token const &t) 
        { return t.get_value(); }
#endif // defined(CPP_DUMP_PARSE_TREE)
    
#if defined(BOOST_SPIRIT_DEBUG)
// debug support
    void print (std::ostream &stream) const
    {
        stream << cpplexer::get_token_name(id) << "(";
        for (size_t i = 0; i < value.size(); ++i) {
            switch (value[i]) {
            case '\r':  stream << "\\r"; break;
            case '\n':  stream << "\\n"; break;
            default:
                stream << value[i]; 
                break;
            }
        }
        stream << ")";
    }
#endif // defined(BOOST_SPIRIT_DEBUG)

private:
    token_id id;                // the token id
    string_t value;             // the text, which was parsed into this token
    PositionT pos;              // the original file position
};

#if defined(BOOST_SPIRIT_DEBUG)
template <typename IteratorT, typename PositionT>
inline std::ostream &
operator<< (std::ostream &stream, lex_token<IteratorT, PositionT> const &object)
{
    object.print(stream);
    return stream;
}
#endif // defined(BOOST_SPIRIT_DEBUG)

///////////////////////////////////////////////////////////////////////////////
}   // namespace slex
}   // namespace cpplexer

#endif // !defined(_CPP_SLEX_TOKEN_HPP__53A13BD2_FBAA_444B_9B8B_FCB225C2BBA8__INCLUDED_)
