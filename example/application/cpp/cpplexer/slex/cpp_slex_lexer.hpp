/*=============================================================================
    A Standard compliant C++ preprocessor

    SLex (Spirit Lex) based C++ lexer
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_SLEX_LEXER_HPP__5E8E1DF0_BB41_4938_B7E5_A4BB68222FF6__INCLUDED_)
#define _CPP_SLEX_LEXER_HPP__5E8E1DF0_BB41_4938_B7E5_A4BB68222FF6__INCLUDED_

#include <string>
#if defined(BOOST_SPIRIT_DEBUG)
#include <iostream>
#endif // defined(BOOST_SPIRIT_DEBUG)

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>

#include "../slex/lexer.hpp"            // "spirit/lexer.hpp"

#include "util/time_conversion_helper.hpp"
#include "cpplexer/validate_universal_char.hpp"
#include "cpplexer/cpp_token_ids.hpp"
#include "cpplexer/cpp_lex_interface.hpp"
#include "cpplexer/slex/cpp_slex_token.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace cpplexer {
namespace slex {

///////////////////////////////////////////////////////////////////////////////
// 
//  encapsulation of the boost::spirit::slex based cpp lexer
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT, typename PositionT>
class lexer 
:   public boost::spirit::lexer<boost::spirit::position_iterator<IteratorT> >
{
public:

    typedef boost::spirit::position_iterator<IteratorT>             iterator_t;
    typedef typename std::iterator_traits<IteratorT>::value_type    char_t;
    typedef boost::spirit::lexer<iterator_t>    base_t;
    typedef lex_token<IteratorT, PositionT>     token_t;
    
    lexer();

// get time of last compilation
    std::time_t get_compilation_time() { return compilation_time.get_time(); }

private:
// initialization data (regular expressions for the token definitions)
    struct lexer_data {
        token_id tokenid;                       // token data
        char_t const *tokenregex;               // associated token to match
        typename base_t::callback_t tokencb;    // associated callback function
        unsigned int lexerstate;                // valid for lexer state
    };
    
    static lexer_data const init_data[];

// helper for calculation of the time of last compilation
    static cpp::util::time_conversion_helper compilation_time;
};

///////////////////////////////////////////////////////////////////////////////
//  data required for initialization of the lexer (token definitions)
#define OR      "|"
#define Q(c)    "\\" c
#define TRI(c)  Q("?") Q("?") c

// definition of some subtoken regexps to simplify the regex definitions
#define BLANK           "[ \\t]"
#define CCOMMENT        \
    Q("/") Q("*") "[^*]*" Q("*") "+" "(" "[^/*][^*]*" Q("*") "+" ")*" Q("/")
        
#define PPSPACE         "(" BLANK OR CCOMMENT ")*"

#define OCTALDIGIT      "[0-7]"
#define DIGIT           "[0-9]"
#define HEXDIGIT        "[0-9a-fA-F]"
#define SIGN            "[-+]?"
#define EXPONENT        "(" "[eE]" SIGN "[0-9]+" ")"

#define INTEGER_SUFFIX  "(" "[uU][lL]?|[lL][uU]?" ")"
#define FLOAT_SUFFIX    "(" "[fF][lL]?|[lL][fF]?" ")"
#define CHAR_SPEC       "L?"

#define BACKSLASH       "(" Q("\\") OR TRI(Q("/")) ")"
#define ESCAPESEQ       BACKSLASH "(" \
                            "[abfnrtv?'\"]" OR \
                            BACKSLASH OR \
                            "x" HEXDIGIT "+" OR \
                            OCTALDIGIT OCTALDIGIT "?" OCTALDIGIT "?" \
                        ")"
#define HEXQUAD         HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT 
#define UNIVERSALCHAR   BACKSLASH "(" \
                            "u" HEXQUAD OR \
                            "U" HEXQUAD HEXQUAD \
                        ")" 

#define POUNDDEF        "(" "#" OR TRI("=") OR Q("%:") ")"
#define NEWLINEDEF      "(" "\\n" OR "\\r" OR "\\r\\n" ")"

///////////////////////////////////////////////////////////////////////////////
//  sexer state constants
#define LEXER_STATE_NORMAL  0
#define LEXER_STATE_PP      1

#define NUM_LEXER_STATES    1

//  helper for initializing token data
#define TOKEN_DATA(id, regex) \
    { T_##id, regex, 0, LEXER_STATE_NORMAL }

#define TOKEN_DATA_EX(id, regex, callback) \
    { T_##id, regex, callback, LEXER_STATE_NORMAL }

///////////////////////////////////////////////////////////////////////////////
// C++ token definitions
template <typename IteratorT, typename PositionT>
typename lexer<IteratorT, PositionT>::lexer_data const 
lexer<IteratorT, PositionT>::init_data[] = 
{
    TOKEN_DATA(AND, "&"),
    TOKEN_DATA(AND_ALT, "bitand"),
    TOKEN_DATA(ANDAND, "&&"),
    TOKEN_DATA(ASSIGN, "="),
    TOKEN_DATA(ANDASSIGN, "&="),
    TOKEN_DATA(ANDASSIGN_ALT, "and_eq"),
    TOKEN_DATA(OR, Q("|")),
    TOKEN_DATA(OR_ALT, "bitor"),
    TOKEN_DATA(OR_TRIGRAPH, TRI("!")),
    TOKEN_DATA(ORASSIGN, Q("|=") OR TRI("!=")),
    TOKEN_DATA(ORASSIGN_ALT, "or_eq"),
    TOKEN_DATA(XOR, Q("^")),
    TOKEN_DATA(XOR_ALT, "xor"),
    TOKEN_DATA(XOR_TRIGRAPH, TRI("'")),
    TOKEN_DATA(XORASSIGN, Q("^=") OR TRI("'=")),
    TOKEN_DATA(XORASSIGN_ALT, "xor_eq"),
    TOKEN_DATA(COMMA, ","),
    TOKEN_DATA(COLON, ":"),
    TOKEN_DATA(DIVIDE, Q("/")),
    TOKEN_DATA(DIVIDEASSIGN, Q("/=")),
    TOKEN_DATA(DOT, Q(".")),
    TOKEN_DATA(DOTSTAR, Q(".") Q("*")),
    TOKEN_DATA(ELLIPSIS, Q(".") Q(".") Q(".")),
    TOKEN_DATA(EQUAL, "=="),
    TOKEN_DATA(GREATER, ">"),
    TOKEN_DATA(GREATEREQUAL, ">="),
    TOKEN_DATA(LEFTBRACE, Q("{")),
    TOKEN_DATA(LEFTBRACE_ALT, "<" Q("%")),
    TOKEN_DATA(LEFTBRACE_TRIGRAPH, TRI("<")),
    TOKEN_DATA(LESS, "<"),
    TOKEN_DATA(LESSEQUAL, "<="),
    TOKEN_DATA(LEFTPAREN, Q("(")),
    TOKEN_DATA(LEFTBRACKET, Q("[")),
    TOKEN_DATA(LEFTBRACKET_ALT, OR "<:"),
    TOKEN_DATA(LEFTBRACKET_TRIGRAPH, TRI(Q("("))),
    TOKEN_DATA(MINUS, Q("-")),
    TOKEN_DATA(MINUSASSIGN, Q("-=")),
    TOKEN_DATA(MINUSMINUS, Q("-") Q("-")),
    TOKEN_DATA(PERCENT, Q("%")),
    TOKEN_DATA(PERCENTASSIGN, Q("%=")),
    TOKEN_DATA(NOT, "!"),
    TOKEN_DATA(NOT_ALT, "not"),
    TOKEN_DATA(NOTEQUAL, "!="),
    TOKEN_DATA(NOTEQUAL_ALT, "not_eq"),
    TOKEN_DATA(OROR, Q("|") Q("|") OR TRI("!") Q("|") OR Q("|") TRI("!") OR 
                TRI("!") TRI("!")),
    TOKEN_DATA(PLUS, Q("+")),
    TOKEN_DATA(PLUSASSIGN, Q("+=")),
    TOKEN_DATA(PLUSPLUS, Q("+") Q("+")),
    TOKEN_DATA(ARROW, Q("->")),
    TOKEN_DATA(ARROWSTAR, Q("->") Q("*")),
    TOKEN_DATA(QUESTION_MARK, Q("?")),
    TOKEN_DATA(RIGHTBRACE, Q("}")),
    TOKEN_DATA(RIGHTBRACE_ALT, Q("%>")),
    TOKEN_DATA(RIGHTBRACE_TRIGRAPH, TRI(">")),
    TOKEN_DATA(RIGHTPAREN, Q(")")),
    TOKEN_DATA(RIGHTBRACKET, Q("]")),
    TOKEN_DATA(RIGHTBRACKET_ALT, ":>"),
    TOKEN_DATA(RIGHTBRACKET_TRIRAPH, TRI(Q(")"))),
    TOKEN_DATA(COLON_COLON, "::"),
    TOKEN_DATA(SEMICOLON, ";"),
    TOKEN_DATA(SHIFTLEFT, "<<"),
    TOKEN_DATA(SHIFTLEFTASSIGN, "<<="),
    TOKEN_DATA(SHIFTRIGHT, ">>"),
    TOKEN_DATA(SHIFTRIGHTASSIGN, ">>="),
    TOKEN_DATA(STAR, Q("*")),
    TOKEN_DATA(COMPL, Q("~")),
    TOKEN_DATA(COMPL_ALT, "compl"),
    TOKEN_DATA(COMPL_TRIGRAPH, TRI("-")),
    TOKEN_DATA(STARASSIGN, Q("*=")),
    TOKEN_DATA(ASM, "asm"),
    TOKEN_DATA(AUTO, "auto"),
    TOKEN_DATA(BOOL, "bool"),
    TOKEN_DATA(FALSE, "false"),
    TOKEN_DATA(TRUE, "true"),
    TOKEN_DATA(BREAK, "break"),
    TOKEN_DATA(CASE, "case"),
    TOKEN_DATA(CATCH, "catch"),
    TOKEN_DATA(CHAR, "char"),
    TOKEN_DATA(CLASS, "class"),
    TOKEN_DATA(CONST, "const"),
    TOKEN_DATA(CONSTCAST, "const_cast"),
    TOKEN_DATA(CONTINUE, "continue"),
    TOKEN_DATA(DEFAULT, "default"),
//    TOKEN_DATA(DEFINED, "defined"),
    TOKEN_DATA(DELETE, "delete"),
    TOKEN_DATA(DO, "do"),
    TOKEN_DATA(DOUBLE, "double"),
    TOKEN_DATA(DYNAMICCAST, "dynamic_cast"),
    TOKEN_DATA(ELSE, "else"),
    TOKEN_DATA(ENUM, "enum"),
    TOKEN_DATA(EXPLICIT, "explicit"),
    TOKEN_DATA(EXPORT, "export"),
    TOKEN_DATA(EXTERN, "extern"),
    TOKEN_DATA(FLOAT, "float"),
    TOKEN_DATA(FOR, "for"),
    TOKEN_DATA(FRIEND, "friend"),
    TOKEN_DATA(GOTO, "goto"),
    TOKEN_DATA(IF, "if"),
    TOKEN_DATA(INLINE, "inline"),
    TOKEN_DATA(INT, "int"),
    TOKEN_DATA(LONG, "long"),
    TOKEN_DATA(MUTABLE, "mutable"),
    TOKEN_DATA(NAMESPACE, "namespace"),
    TOKEN_DATA(NEW, "new"),
    TOKEN_DATA(OPERATOR, "operator"),
    TOKEN_DATA(PRIVATE, "private"),
    TOKEN_DATA(PROTECTED, "protected"),
    TOKEN_DATA(PUBLIC, "public"),
    TOKEN_DATA(REGISTER, "register"),
    TOKEN_DATA(REINTERPRETCAST, "reinterpret_cast"),
    TOKEN_DATA(RETURN, "return"),
    TOKEN_DATA(SHORT, "short"),
    TOKEN_DATA(SIGNED, "signed"),
    TOKEN_DATA(SIZEOF, "sizeof"),
    TOKEN_DATA(STATIC, "static"),
    TOKEN_DATA(STATICCAST, "static_cast"),
    TOKEN_DATA(STRUCT, "struct"),
    TOKEN_DATA(SWITCH, "switch"),
    TOKEN_DATA(TEMPLATE, "template"),
    TOKEN_DATA(THIS, "this"),
    TOKEN_DATA(THROW, "throw"),
    TOKEN_DATA(TRY, "try"),
    TOKEN_DATA(TYPEDEF, "typedef"),
    TOKEN_DATA(TYPEID, "typeid"),
    TOKEN_DATA(TYPENAME, "typename"),
    TOKEN_DATA(UNION, "union"),
    TOKEN_DATA(UNSIGNED, "unsigned"),
    TOKEN_DATA(USING, "using"),
    TOKEN_DATA(VIRTUAL, "virtual"),
    TOKEN_DATA(VOID, "void"),
    TOKEN_DATA(VOLATILE, "volatile"),
    TOKEN_DATA(WCHART, "wchar_t"),
    TOKEN_DATA(WHILE, "while"),
    TOKEN_DATA(PP_DEFINE, POUNDDEF PPSPACE "define"),
    TOKEN_DATA(PP_IF, POUNDDEF PPSPACE "if"),
    TOKEN_DATA(PP_IFDEF, POUNDDEF PPSPACE "ifdef"),
    TOKEN_DATA(PP_IFNDEF, POUNDDEF PPSPACE "ifndef"),
    TOKEN_DATA(PP_ELSE, POUNDDEF PPSPACE "else"),
    TOKEN_DATA(PP_ELIF, POUNDDEF PPSPACE "elif"),
    TOKEN_DATA(PP_ENDIF, POUNDDEF PPSPACE "endif"),
    TOKEN_DATA(PP_ERROR, POUNDDEF PPSPACE "error"),
    TOKEN_DATA(PP_QHEADER, POUNDDEF PPSPACE "include" PPSPACE Q("\"") "[^\\n\\r\"]+" Q("\"")),
    TOKEN_DATA(PP_HHEADER, POUNDDEF PPSPACE "include" PPSPACE "<" "[^\\n\\r>]+" ">"),
    TOKEN_DATA(PP_INCLUDE, POUNDDEF PPSPACE "include" PPSPACE),
    TOKEN_DATA(PP_LINE, POUNDDEF PPSPACE "line"),
    TOKEN_DATA(PP_PRAGMA, POUNDDEF PPSPACE "pragma"),
    TOKEN_DATA(PP_UNDEF, POUNDDEF PPSPACE "undef"),
    TOKEN_DATA(PP_WARNING, POUNDDEF PPSPACE "warning"),
    TOKEN_DATA(IDENTIFIER, "([a-zA-Z_]" OR UNIVERSALCHAR ")([a-zA-Z0-9_]" OR UNIVERSALCHAR ")*"),
//  TOKEN_DATA(OCTALINT, "0" OCTALDIGIT "*" INTEGER_SUFFIX "?"),
//  TOKEN_DATA(DECIMALINT, "[1-9]" DIGIT "*" INTEGER_SUFFIX "?"),
//  TOKEN_DATA(HEXAINT, "(0x|0X)" HEXDIGIT "+" INTEGER_SUFFIX "?"),
    TOKEN_DATA(INTLIT, "(" "(0x|0X)" HEXDIGIT "+" OR "0" OCTALDIGIT "*" OR \
            "[1-9]" DIGIT "*" ")" INTEGER_SUFFIX "?"),
    TOKEN_DATA(FLOATLIT, 
        "(" DIGIT "*" Q(".") DIGIT "+" OR DIGIT "+" Q(".") ")" 
        EXPONENT "?" FLOAT_SUFFIX "?" OR
        DIGIT "+" EXPONENT FLOAT_SUFFIX "?"),
    TOKEN_DATA(CCOMMENT, CCOMMENT),
    TOKEN_DATA(CPPCOMMENT, Q("/") Q("/[^\\n\\r]*") NEWLINEDEF ),
    TOKEN_DATA(CHARLIT, CHAR_SPEC "'" 
                "(" ESCAPESEQ OR "[^\\n\\r']" OR UNIVERSALCHAR ")+" "'"),
    TOKEN_DATA(STRINGLIT, CHAR_SPEC Q("\"") 
                "(" ESCAPESEQ OR "[^\\n\\r\"]" OR UNIVERSALCHAR ")*" Q("\"")),
    TOKEN_DATA(SPACE, BLANK "+"),
    TOKEN_DATA(SPACE2, "[\\v\\f]+"),
    TOKEN_DATA(CONTLINE, Q("\\") "\\n"), 
    TOKEN_DATA(NEWLINE, NEWLINEDEF),
    TOKEN_DATA(POUND_POUND, "##"),
    TOKEN_DATA(POUND_POUND_ALT, Q("%:") Q("%:")),
    TOKEN_DATA(POUND_POUND_TRIGRAPH, TRI("=") TRI("=")),
    TOKEN_DATA(POUND, "#"),
    TOKEN_DATA(POUND_ALT, Q("%:")),
    TOKEN_DATA(POUND_TRIGRAPH, TRI("=")),
    TOKEN_DATA(ANY, "."),
#if defined(SUPPORMS_EXTENSIONS)
    TOKEN_DATA(MSEXINT8, "__int8"),
    TOKEN_DATA(MSEXINT16, "__int16"),
    TOKEN_DATA(MSEXINT32, "__int32"),
    TOKEN_DATA(MSEXINT64, "__int64"),
    TOKEN_DATA(MSEXBASED, "_?" "_based"),
    TOKEN_DATA(MSEXDECLSPEC, "_?" "_declspec"),
    TOKEN_DATA(MSEXCDECL, "_?" "_cdecl"),
    TOKEN_DATA(MSEXFASTCALL, "_?" "_fastcall"),
    TOKEN_DATA(MSEXSTDCALL, "_?" "_stdcall"),
    TOKEN_DATA(MSEXTRY , "__try"),
    TOKEN_DATA(MSEXEXCEPT, "__except"),
    TOKEN_DATA(MSEXFINALLY, "__finally"),
    TOKEN_DATA(MSEXLEAVE, "__leave"),
    TOKEN_DATA(MSEXINLINE, "_?" "_inline"),
    TOKEN_DATA(MSEXASM, "_?" "_asm"),
#endif // defined(SUPPORMS_EXTENSIONS)
    { token_id(0) }       // this should be the last entry
};

///////////////////////////////////////////////////////////////////////////////
//  undefine macros, required for regular expression definitions
#undef POUNDDEF
#undef CCOMMENT
#undef PPSPACE
#undef DIGIT
#undef OCTALDIGIT
#undef HEXDIGIT
#undef SIGN
#undef EXPONENT
#undef INTEGER_SUFFIX
#undef FLOAT_SUFFIX
#undef CHAR_SPEC
#undef BACKSLASH    
#undef ESCAPESEQ    
#undef HEXQUAD      
#undef UNIVERSALCHAR

#undef Q
#undef TRI
#undef OR

#undef TOKEN_DATA
#undef TOKEN_DATA_EX

///////////////////////////////////////////////////////////////////////////////
// initialize cpp lexer with token data
template <typename IteratorT, typename PositionT>
inline
lexer<IteratorT, PositionT>::lexer() 
:   base_t(NUM_LEXER_STATES)
{
    int i = 0;
    while (0 != init_data[i].tokenid) {
        this->register_regex(init_data[i].tokenregex, init_data[i].tokenid, 
            init_data[i].tokencb, init_data[i].lexerstate);
        ++i;
    }
}

///////////////////////////////////////////////////////////////////////////////
// get time of last compilation of this file
template <typename IteratorT, typename PositionT>
cpp::util::time_conversion_helper 
    lexer<IteratorT, PositionT>::compilation_time(__DATE__ " " __TIME__);

///////////////////////////////////////////////////////////////////////////////
//  
template <typename IteratorT, typename PositionT>
inline void 
init_lexer (lexer<IteratorT, PositionT> &lexer, bool force_reinit = false)
{
    if (lexer.has_compiled_dfa())
        return;     // nothing to do
        
    using std::ifstream;
    using std::ofstream;
    using std::ios;
    using std::cerr;
    using std::endl;
    
ifstream dfa_in("cpp_slex_lexer.dfa", ios::in|ios::binary);

    if (force_reinit || !dfa_in.is_open() ||
        !lexer.load (dfa_in, lexer.get_compilation_time()))
    {
        cerr << "Compiling regular expressions for slex ...";
        dfa_in.close();
        lexer.create_dfa();

    ofstream dfa_out ("cpp_slex_lexer.dfa", ios::out|ios::binary|ios::trunc);

        if (dfa_out.is_open())
            lexer.save (dfa_out, lexer.get_compilation_time());
        cerr << " Done." << endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_functor
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT, typename PositionT = boost::spirit::file_position>
class lex_functor 
:   public lex_input_interface<typename lexer<IteratorT, PositionT>::token_t>
{
public:

    typedef boost::spirit::position_iterator<IteratorT>     iterator_t;
    typedef typename std::iterator_traits<IteratorT>::value_type    char_t;
    typedef std::basic_string<char_t>                       string_t;
    typedef typename lexer<IteratorT, PositionT>::token_t   token_t;

    lex_functor(IteratorT const &first, IteratorT const &last, 
            PositionT const &pos)
    :   first(first, last, pos)
    {
        init_lexer(lexer);  // initialize lexer dfa tables
    }
    virtual ~lex_functor() {}

// get the next token from the input stream
    token_t get()
    {
        token_t token;
        do {
        iterator_t prev = first;
        token_id id = token_id(lexer.next_token(first, last));

            if (-1 == id)
                break;              // end of input reached

            if (T_CONTLINE != id) {
            // generate and return the next token
            string_t value(prev, first);

                if (T_IDENTIFIER == id) {
                // test identifier characters for validity (throws if invalid 
                // chars found)
                    PositionT const &pos = prev.get_position();
                    impl::validate_identifier_name(value, pos.line, pos.column, 
                        pos.file); 
                }
                else if (T_STRINGLIT == id || T_CHARLIT == id) {
                // test literal characters for validity (throws if invalid 
                // chars found)
                    PositionT const &pos = prev.get_position();
                    impl::validate_literal(value, pos.line, pos.column, 
                        pos.file); 
                }
                return token_t(id, value, prev.get_position());
            }
        
        // skip the T_CONTLINE token
        } while (true);
        return token;       // return T_EOF
    }
    void set_position(PositionT const &pos) 
    { first.set_position(pos); }
    
private:
    iterator_t first;
    iterator_t last;
    static lexer<IteratorT, PositionT> lexer;   // needed only once
};

template <typename IteratorT, typename PositionT>
lexer<IteratorT, PositionT> lex_functor<IteratorT, PositionT>::lexer;

///////////////////////////////////////////////////////////////////////////////
//  
//  The new_lexer_gen<>::new_lexer function (declared in cpp_slex_token.hpp)
//  should be defined inline, if the lex_functor shouldn't be instantiated 
//  separately from the lex_iterator.
//
//  Separate (explicit) instantiation helps to reduce compilation time.
//
///////////////////////////////////////////////////////////////////////////////

#if defined(CPP_SEPARATE_LEXER_INSTANTIATION)
#define SLEX_NEW_LEXER_INLINE
#else
#define SLEX_NEW_LEXER_INLINE inline
#endif 

///////////////////////////////////////////////////////////////////////////////
//
//  The 'new_lexer' function allows the opaque generation of a new lexer object.
//  It is coupled to the token type to allow to decouple the lexer/token 
//  configurations at compile time.
//
//  This function is declared inside the cpp_slex_token.hpp file, which is 
//  referenced by the source file calling the lexer and the sourcefile, which
//  instantiates the lex_functor. But is is defined here, so it will be 
//  instantiated only while compiling the sourcefile, which instantiates the 
//  lex_functor. While the cpp_slex_token.hpp file may be included everywhere,
//  this file (cpp_slex_lexer.hpp) should be included only once. This allows
//  to decouple the lexer interface from the lexer implementation and reduces 
//  compilation time.
//
///////////////////////////////////////////////////////////////////////////////

template <typename IteratorT, typename PositionT>
SLEX_NEW_LEXER_INLINE
lex_input_interface<lex_token<IteratorT, PositionT> > *
new_lexer_gen<IteratorT, PositionT>::new_lexer(IteratorT const &first,
    IteratorT const &last, PositionT const &pos)
{
    return new lex_functor<IteratorT, PositionT>(first, last, pos);
}

#undef SLEX_NEW_LEXER_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace slex
}   // namespace cpplexer
     
#endif // !defined(_CPP_SLEX_LEXER_HPP__5E8E1DF0_BB41_4938_B7E5_A4BB68222FF6__INCLUDED_)
