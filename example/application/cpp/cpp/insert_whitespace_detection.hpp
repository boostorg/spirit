/*=============================================================================
    A Standard compliant C++ preprocessor

    Detect the need to inserte a whitespace token into the output stream
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/
#if !defined(_INSERT_WHITESPACE_DETECTION_HPP__765EF77B_0513_4967_BDD6_6A38148C4C96__INCLUDED_)
#define _INSERT_WHITESPACE_DETECTION_HPP__765EF77B_0513_4967_BDD6_6A38148C4C96__INCLUDED_

#include "cpplexer/cpp_token_ids.hpp"   

///////////////////////////////////////////////////////////////////////////////
namespace cpp {
namespace util {

namespace impl {

// T_IDENTIFIER
    inline bool
    would_form_universal_char (std::string const &value)
    {
        if ('u' != value[0] && 'U' != value[0])
            return false;
        if ('u' == value[0] && value.size() < 5)
            return false;
        if ('U' == value[0] && value.size() < 9)
            return false;
    
    std::string::size_type pos = 
        value.find_first_not_of("0123456789abcdefABCDEF", 1);
        
        if (std::string::npos == pos || 
            ('u' == value[0] && pos > 5) ||
            ('U' == value[0] && pos > 9))
        {
            return true;        // would form an universal char
        }
        return false;
    }
    inline bool 
    handle_identifier(cpplexer::token_id prev, cpplexer::token_id before,
        std::string const &value)
    {
        using namespace cpplexer;
        switch (static_cast<unsigned int>(prev)) {
        case T_IDENTIFIER:
        case T_NONREPLACABLE_IDENTIFIER:
        case T_INTLIT:
        case T_FLOATLIT:
         // avoid constructing universal characters (\u1234)
        case TOKEN_FROM_ID('\\', UnknownTokenType):
        case T_COMPL_ALT:
        case T_OR_ALT:
        case T_AND_ALT:
        case T_NOT_ALT:
        case T_XOR_ALT:
        case T_ANDASSIGN_ALT:
        case T_ORASSIGN_ALT:
        case T_XORASSIGN_ALT:
        case T_NOTEQUAL_ALT:
            return would_form_universal_char(value);
        }
        return false;
    }
// T_INTLIT
    inline bool 
    handle_intlit(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_IDENTIFIER:
        case T_NONREPLACABLE_IDENTIFIER:
        case T_INTLIT:
        case T_FLOATLIT:
            return true;
        }
        return false;
    }
// T_FLOATLIT
    inline bool 
    handle_floatlit(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_IDENTIFIER:
        case T_NONREPLACABLE_IDENTIFIER:
        case T_INTLIT:
        case T_FLOATLIT:
            return true;
        }
        return false;
    }
// <% T_LEFTBRACE
    inline bool 
    handle_alt_leftbrace(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<%
        case T_SHIFTLEFT:   // <<<%
            return true;
        }
        return false;
    }
// <: T_LEFTBRACKET
    inline bool 
    handle_alt_leftbracket(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<:
        case T_SHIFTLEFT:   // <<<:
            return true;
        }
        return false;
    }
// T_DOT
    inline bool 
    handle_dot(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_DOT:
            if (T_DOT == before)
                return true;    // ...
            break;
        }
        return false;
    }
// T_QUESTION_MARK
    inline bool 
    handle_questionmark(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(static_cast<unsigned int>(prev)) {
        case TOKEN_FROM_ID('\\', UnknownTokenType):     // \?
        case T_QUESTION_MARK:   // ??
            return true;
        }
        return false;
    }
// T_NEWLINE
    inline bool
    handle_newline(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(static_cast<unsigned int>(prev)) {
        case TOKEN_FROM_ID('\\', UnknownTokenType): // \ \n
        case T_DIVIDE:
            if (T_QUESTION_MARK == before)
                return true;    // ?/\n     // may be \\n
            break;
        }
        return false;
    }
    
}   // namespace impl

class insert_whitespace_detection 
{
public:
    insert_whitespace_detection() 
    :   prev(cpplexer::T_EOF), beforeprev(cpplexer::T_EOF) 
    {}
    
    bool must_insert(cpplexer::token_id current, std::string const &value)
    {
        using namespace cpplexer;
        switch (current) {
        case T_NONREPLACABLE_IDENTIFIER:
        case T_IDENTIFIER: 
            return impl::handle_identifier(prev, beforeprev, value); 
        case T_INTLIT:
            return impl::handle_intlit(prev, beforeprev); 
        case T_FLOATLIT:
            return impl::handle_floatlit(prev, beforeprev); 
            
        case T_LEFTBRACE_ALT:
            return impl::handle_alt_leftbrace(prev, beforeprev); 
        case T_LEFTBRACKET_ALT:
            return impl::handle_alt_leftbracket(prev, beforeprev); 
        case T_DOT:
            return impl::handle_dot(prev, beforeprev); 
        case T_QUESTION_MARK:
            return impl::handle_questionmark(prev, beforeprev); 
        case T_NEWLINE:
            return impl::handle_newline(prev, beforeprev); 

        case T_LEFTPAREN:
        case T_RIGHTPAREN:
        case T_LEFTBRACKET:
        case T_RIGHTBRACKET:
        case T_SEMICOLON:
        case T_COMMA:
        case T_COLON:
            switch (prev) {
            case T_LEFTPAREN:
            case T_RIGHTPAREN:
            case T_LEFTBRACKET:
            case T_RIGHTBRACKET:
            case T_LEFTBRACE:
            case T_RIGHTBRACE:
                return false;   // no insertion between parens/brackets/braces

            default:
                break;
            }        
            break;
            
        case T_LEFTBRACE:
        case T_RIGHTBRACE:
            switch (prev) {
            case T_LEFTPAREN:
            case T_RIGHTPAREN:
            case T_LEFTBRACKET:
            case T_RIGHTBRACKET:
            case T_LEFTBRACE:
            case T_RIGHTBRACE:
            case T_SEMICOLON:
            case T_COMMA:
            case T_COLON:
                return false;   // no insertion between parens/brackets/braces

            case T_QUESTION_MARK:
                if (T_QUESTION_MARK == beforeprev)
                    return true;
                break;
                
            default:
                break;
            }
            break;
                            
        case T_MINUS:
        case T_MINUSMINUS:
        case T_MINUSASSIGN:
        case T_LESS:
        case T_EQUAL:
        case T_ASSIGN:
        case T_GREATER:
        case T_DIVIDE:
        case T_DIVIDEASSIGN:
        case T_CHARLIT:
        case T_NOT:
        case T_NOTEQUAL:
            if (T_QUESTION_MARK == prev && T_QUESTION_MARK == beforeprev)
                return true;    // ??{op}
            break;

        case T_COMPL_ALT:
        case T_OR_ALT:
        case T_AND_ALT:
        case T_NOT_ALT:
        case T_XOR_ALT:
        case T_ANDASSIGN_ALT:
        case T_ORASSIGN_ALT:
        case T_XORASSIGN_ALT:
        case T_NOTEQUAL_ALT:
            if (T_IDENTIFIER == prev || T_NONREPLACABLE_IDENTIFIER == prev)
                return true;
            break;
        }

    // else, handle operators separately
        if (IS_CATEGORY(current, OperatorTokenType) && 
            IS_CATEGORY(prev, OperatorTokenType))
        {
            return true;    // operators must be delimited always
        }
        return false;
    }
    void shift_tokens (cpplexer::token_id next_id)
    {
        beforeprev = prev;
        prev = next_id;
    }
    
private:
    cpplexer::token_id prev;        // the previuos analyzed token
    cpplexer::token_id beforeprev;  // the token before the previous
};

///////////////////////////////////////////////////////////////////////////////
}   //  namespace util
}   //  namespace cpp 

#endif // !defined(_INSERT_WHITESPACE_DETECTION_HPP__765EF77B_0513_4967_BDD6_6A38148C4C96__INCLUDED_)
