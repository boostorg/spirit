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
        switch (prev) {
        case T_IDENTIFIER:
        case T_NONREPLACABLE_IDENTIFIER:
        case T_INTLIT:
        case T_FLOATLIT:
         // avoid constructing universal characters (\u1234)
        case TOKEN_FROM_ID('\\', UnknownTokenType):
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
// T_PLUS
    inline bool 
    handle_plus(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_PLUS:        // ++
        case T_PLUSPLUS:    // +++
            return true;
        }
        return false;
    }
// T_PLUSPLUS
    inline bool 
    handle_plusplus(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_PLUS:        // +++
        case T_PLUSPLUS:    // ++++
            return true;
        }
        return false;
    }
// T_PLUSASSIGN
    inline bool 
    handle_plusassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_PLUS:        // ++=
        case T_PLUSPLUS:    // +++=
            return true;
        }
        return false;
    }
// T_MINUS
    inline bool 
    handle_minus(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_MINUS:       // --
        case T_MINUSMINUS:  // ---
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??-
            break;
        }
        return false;
    }
// T_MINUSMINUS
    inline bool 
    handle_minusminus(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_MINUS:       // ---
        case T_MINUSMINUS:  // ----
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??--
            break;
        }
        return false;
    }
// T_MINUSASSIGN
    inline bool 
    handle_minusassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_MINUS:       // --=
        case T_MINUSMINUS:  // ---=
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??-=
            break;
        }
        return false;
    }
// T_PERCENT
    inline bool 
    handle_percent(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <%
        case T_SHIFTLEFT:   // <<%
            return true;
        }
        return false;
    }
// T_PERCENTASSIGN
    inline bool 
    handle_percentassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <%=
        case T_SHIFTLEFT:   // <<%=
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
// T_POUND
    inline bool 
    handle_pound(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_POUND:       // ##
        case T_POUND_POUND: // ###
            return true;
        }
        return false;
    }
// T_POUND_POUND
    inline bool 
    handle_poundpound(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_POUND:       // ###
        case T_POUND_POUND: // ####
            return true;
        }
        return false;
    }
// T_COLON
    inline bool 
    handle_colon(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_COLON:       // ::
        case T_COLON_COLON: // :::
        case T_LESS:        // <:
        case T_SHIFTLEFT:   // <<:
        case T_PERCENT:     // %:
        case T_POUND:       // %::
        case T_POUND_POUND: // %:%::
            return true;
        }
        return false;
    }
// T_COLON_COLON
    inline bool 
    handle_coloncolon(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_COLON:       // :::
        case T_COLON_COLON: // ::::
        case T_PERCENT:     // %::
        case T_POUND:       // %:::
        case T_POUND_POUND: // %:%:::
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
// T_LESS
    inline bool 
    handle_less(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<
        case T_SHIFTLEFT:   // <<<
            return true;

            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??<
            break;
        }
        return false;
    }
// T_LESSEQUAL
    inline bool 
    handle_lessequal(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<=
        case T_SHIFTLEFT:   // <<<=
            return true;
        }
        return false;
    }
// T_SHIFTLEFT
    inline bool 
    handle_shiftleft(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<<
        case T_SHIFTLEFT:   // <<<<
            return true;
        }
        return false;
    }
// T_SHIFTLEFTASSIGN
    inline bool 
    handle_shiftleftassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_LESS:        // <<<=
        case T_SHIFTLEFT:   // <<<<=
            return true;
        }
        return false;
    }
// T_EQUAL
    inline bool 
    handle_equal(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_ASSIGN:      // ===
        case T_EQUAL:       // ====
        case T_LESS:        // <==
        case T_SHIFTLEFT:   // <<==
        case T_SHIFTLEFTASSIGN:   // <<===
        case T_GREATER:     // >==
        case T_SHIFTRIGHT:  // >>==
        case T_SHIFTRIGHTASSIGN:  // >>===
        case T_RIGHTBRACE:  // %>==
        case T_PERCENT:     // %==
        case T_PLUS:        // +==
        case T_PLUSASSIGN:  // +===
        case T_MINUS:       // -==
        case T_MINUSASSIGN: // -===
        case T_STAR:        // *==
        case T_STARASSIGN:  // *===
        case T_DIVIDE:      // /==
        case T_DIVIDEASSIGN:    // /===
        case T_DOTSTAR:     // .*==
        case T_XOR:         // ^==
        case T_XORASSIGN:   // ^===
        case T_AND:         // &==
        case T_ANDASSIGN:   // &===
        case T_ANDAND:      // &&==
        case T_OR:          // |==
        case T_ORASSIGN:    // |===
        case T_OROR:        // ||==
        case T_NOT:         // !==
        case T_ARROW:       // ->==
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??==
            break;
        }
        return false;
    }
// T_ASSIGN
    inline bool 
    handle_assign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_ASSIGN:      // ==
        case T_EQUAL:       // ===
        case T_LESS:        // <=
        case T_SHIFTLEFT:   // <<=
        case T_SHIFTLEFTASSIGN:   // <<==
        case T_GREATER:     // >=
        case T_SHIFTRIGHT:  // >>=
        case T_SHIFTRIGHTASSIGN:  // >>==
        case T_RIGHTBRACE:  // %>=
        case T_PERCENT:     // %=
        case T_PLUS:        // +=
        case T_PLUSASSIGN:  // +==
        case T_MINUS:       // -=
        case T_MINUSASSIGN: // -==
        case T_STAR:        // *=
        case T_STARASSIGN:  // *==
        case T_DIVIDE:      // /=
        case T_DIVIDEASSIGN:    // /==
        case T_DOTSTAR:     // .*=
        case T_XOR:         // ^=
        case T_XORASSIGN:   // ^==
        case T_AND:         // &=
        case T_ANDASSIGN:   // &==
        case T_ANDAND:      // &&=
        case T_OR:          // |=
        case T_ORASSIGN:    // |==
        case T_OROR:        // ||=
        case T_NOT:         // !=
        case T_ARROW:       // ->=
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??=
            break;
        }
        return false;
    }
// T_GREATER
    inline bool 
    handle_greater(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_GREATER:     // >>
        case T_SHIFTRIGHT:  // >>>
        case T_ARROW:       // ->>
        case T_RIGHTBRACE:  // %>>
        case T_MINUS:       // ->
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??>
            break;
        }
        return false;
    }
// T_GREATEREQUAL
    inline bool 
    handle_greaterequal(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_GREATER:     // >>=
        case T_SHIFTRIGHT:  // >>>=
        case T_ARROW:       // ->>=
        case T_RIGHTBRACE:  // %>>=
        case T_MINUS:       // ->=
            return true;
        }
        return false;
    }
// T_SHIFTRIGHT
    inline bool 
    handle_shiftright(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_GREATER:     // >>>
        case T_SHIFTRIGHT:  // >>>>
        case T_ARROW:       // ->>>
        case T_RIGHTBRACE:  // %>>>
            return true;
        }
        return false;
    }
// T_SHIFTRIGHTASSIGN
    inline bool 
    handle_shiftrightassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_GREATER:     // >>>=
        case T_SHIFTRIGHT:  // >>>>=
        case T_ARROW:       // ->>>=
        case T_RIGHTBRACE:  // %>>>=
            return true;
        }
        return false;
    }
// T_STAR
    inline bool 
    handle_star(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_ARROW:       // ->*
        case T_DOT:         // .*
        case T_DIVIDE:      // /*
            return true;
        }
        return false;
    }
// T_STARASSIGN
    inline bool 
    handle_starassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_ARROW:       // ->*=
        case T_DOT:         // .*=
        case T_DIVIDE:      // /*=
            return true;
        }
        return false;
    }
// T_AND
    inline bool 
    handle_and(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_AND:         // &&
        case T_ANDAND:      // &&&
            return true;
        }
        return false;
    }
// T_ANDAND
    inline bool 
    handle_andand(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_AND:         // &&&
        case T_ANDAND:      // &&&&
            return true;
        }
        return false;
    }
// T_ANDASSIGN
    inline bool 
    handle_andassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_AND:         // &&=
        case T_ANDAND:      // &&&=
            return true;
        }
        return false;
    }
// T_OR
    inline bool 
    handle_or(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_OR:          // ||
        case T_OROR:        // |||
            return true;
        }
        return false;
    }
// T_OROR
    inline bool 
    handle_oror(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_OR:          // |||
        case T_OROR:        // ||||
            return true;
        }
        return false;
    }
// T_ORASSIGN
    inline bool 
    handle_orassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_OR:          // ||=
        case T_OROR:        // |||=
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
// T_DIVIDE
    inline bool 
    handle_divide(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_DIVIDE:      // //
        case T_STAR:        // */
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??/ 
            break;
        }
        return false;
    }
// T_DIVIDEASSIGN
    inline bool 
    handle_divideassign(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_DIVIDE:      // //=
        case T_STAR:        // */=
            return true;
            
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??/=
            break;
        }
        return false;
    }
// T_QUESTION_MARK
    inline bool 
    handle_questionmark(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case TOKEN_FROM_ID('\\', UnknownTokenType):     // \?
        case T_QUESTION_MARK:   // ??
            return true;
        }
        return false;
    }
// T_CHARLIT
    inline bool 
    handle_charlit(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??'...'
            break;
        }
        return false;
    }
// T_LEFTPAREN
    inline bool 
    handle_leftparen(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??(
            break;
        }
        return false;
    }
// T_RIGHTPAREN
    inline bool 
    handle_rightparen(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??)
            break;
        }
        return false;
    }
// T_NOT
    inline bool 
    handle_not(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??!
            break;
        }
        return false;
    }
// T_NOTEQUAL
    inline bool 
    handle_notequal(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
        case T_QUESTION_MARK:
            if (T_QUESTION_MARK == before)
                return true;    // ??!=
            break;
        }
        return false;
    }
// T_NEWLINE
    inline bool
    handle_newline(cpplexer::token_id prev, cpplexer::token_id before)
    {
        using namespace cpplexer;
        switch(prev) {
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

        bool must_insert = false;
        switch (current) {
        case T_NONREPLACABLE_IDENTIFIER:
        case T_IDENTIFIER: 
            must_insert = impl::handle_identifier(prev, beforeprev, value); 
            break;
        case T_INTLIT:
            must_insert = impl::handle_intlit(prev, beforeprev); 
            break;
        case T_FLOATLIT:
            must_insert = impl::handle_floatlit(prev, beforeprev); 
            break;
        case T_PLUS:
            must_insert = impl::handle_plus(prev, beforeprev); 
            break;
        case T_PLUSPLUS:
            must_insert = impl::handle_plusplus(prev, beforeprev); 
            break;
        case T_PLUSASSIGN:
            must_insert = impl::handle_plusassign(prev, beforeprev); 
            break;
        case T_MINUS:
            must_insert = impl::handle_minus(prev, beforeprev); 
            break;
        case T_MINUSMINUS:
            must_insert = impl::handle_minusminus(prev, beforeprev); 
            break;
        case T_MINUSASSIGN:
            must_insert = impl::handle_minusassign(prev, beforeprev); 
            break;
        case T_PERCENT:
            must_insert = impl::handle_percent(prev, beforeprev); 
            break;
        case T_PERCENTASSIGN:
            must_insert = impl::handle_percentassign(prev, beforeprev); 
            break;
        case T_LEFTBRACE:
            must_insert = impl::handle_alt_leftbrace(prev, beforeprev); 
            break;
        case T_POUND:
            must_insert = impl::handle_pound(prev, beforeprev); 
            break;
        case T_POUND_POUND:
            must_insert = impl::handle_poundpound(prev, beforeprev); 
            break;
        case T_COLON:
            must_insert = impl::handle_colon(prev, beforeprev); 
            break;
        case T_COLON_COLON:
            must_insert = impl::handle_coloncolon(prev, beforeprev); 
            break;
        case T_LEFTBRACKET:
            must_insert = impl::handle_alt_leftbracket(prev, beforeprev); 
            break;
        case T_LESS:
            must_insert = impl::handle_less(prev, beforeprev); 
            break;
        case T_LESSEQUAL:
            must_insert = impl::handle_lessequal(prev, beforeprev); 
            break;
        case T_SHIFTLEFT:
            must_insert = impl::handle_shiftleft(prev, beforeprev); 
            break;
        case T_SHIFTLEFTASSIGN:
            must_insert = impl::handle_shiftleftassign(prev, beforeprev); 
            break;
        case T_EQUAL:
            must_insert = impl::handle_equal(prev, beforeprev); 
            break;
        case T_ASSIGN:
            must_insert = impl::handle_assign(prev, beforeprev); 
            break;
        case T_GREATER:
            must_insert = impl::handle_greater(prev, beforeprev); 
            break;
        case T_GREATEREQUAL:
            must_insert = impl::handle_greaterequal(prev, beforeprev); 
            break;
        case T_SHIFTRIGHT:
            must_insert = impl::handle_shiftright(prev, beforeprev); 
            break;
        case T_SHIFTRIGHTASSIGN:
            must_insert = impl::handle_shiftrightassign(prev, beforeprev); 
            break;
        case T_STAR:
            must_insert = impl::handle_star(prev, beforeprev); 
            break;
        case T_STARASSIGN:
            must_insert = impl::handle_starassign(prev, beforeprev); 
            break;
        case T_AND:
            must_insert = impl::handle_and(prev, beforeprev); 
            break;
        case T_ANDAND:
            must_insert = impl::handle_andand(prev, beforeprev); 
            break;
        case T_ANDASSIGN:
            must_insert = impl::handle_andassign(prev, beforeprev); 
            break;
        case T_OR:
            must_insert = impl::handle_or(prev, beforeprev); 
            break;
        case T_OROR:
            must_insert = impl::handle_oror(prev, beforeprev); 
            break;
        case T_ORASSIGN:
            must_insert = impl::handle_orassign(prev, beforeprev); 
            break;
        case T_DOT:
            must_insert = impl::handle_dot(prev, beforeprev); 
            break;
        case T_DIVIDE:
            must_insert = impl::handle_divide(prev, beforeprev); 
            break;
        case T_DIVIDEASSIGN:
            must_insert = impl::handle_divideassign(prev, beforeprev); 
            break;
        case T_QUESTION_MARK:
            must_insert = impl::handle_questionmark(prev, beforeprev); 
            break;
        case T_CHARLIT:
            must_insert = impl::handle_charlit(prev, beforeprev); 
            break;
        case T_LEFTPAREN:
            must_insert = impl::handle_leftparen(prev, beforeprev); 
            break;
        case T_RIGHTPAREN:
            must_insert = impl::handle_rightparen(prev, beforeprev); 
            break;
        case T_NOT:
            must_insert = impl::handle_not(prev, beforeprev); 
            break;
        case T_NOTEQUAL:
            must_insert = impl::handle_notequal(prev, beforeprev); 
            break;
        }
        return must_insert;
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
