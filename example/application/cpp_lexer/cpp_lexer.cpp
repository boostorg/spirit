//
// C++ Lexer implemented with Spirit (http://spirit.sourceforge.net/)
//
// Copyright© 2002 Juan Carlos Arevalo-Baeza, All rights reserved
// email: jcab@JCABs-Rumblings.com
// Created: 8-Nov-2002
//

#include "cpp_lexer.hpp"

#include <iostream>
#include <fstream>

#include <stdio.h>

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/functor_parser.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/symbols.hpp>

#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/casts.hpp>
#include <boost/spirit/phoenix/binders.hpp>

///////////////////////////////////////////////////////////////////////////////
// used namespaces
using namespace boost::spirit;
//using namespace phoenix;
//using namespace std;

using std::stringstream;
using std::string;
using std::cout;
using std::cerr;
using std::endl;

using phoenix::var;
using phoenix::val;
using phoenix::value;
using phoenix::actor;
using phoenix::arg1;
using phoenix::arg2;
using phoenix::construct_;
using phoenix::function_ptr;

///////////////////////////////////////////////////////////////////////////////
// Utility parsers for debugging and error handling.

namespace {

    template <typename ErrorDescrT>
    class trace_ {

    public:
        typedef nil_t result_t;

        trace_(ErrorDescrT const& what): info(what) {}

        template <typename ScannerT>
        int
        operator()(ScannerT const& scan, result_t& result) const {
            file_position lc = (*scan).filePos;
            cout << lc << "Trace: " << info() << "\n";
            return 0;
        }

    private:

        ErrorDescrT info;
    };

    template < class ActorT >
    static
    functor_parser<trace_<ActorT> >
    trace_p(ActorT const& str) {
        return trace_<ActorT>(str);
    }

    static
    functor_parser<trace_<actor<value<std::string> > > >
    trace_p(std::string const& str) {
        return trace_<actor<value<std::string> > >(val(str));
    }

    static
    functor_parser<trace_<actor<value<char const*> > > >
    trace_p(char const* str) {
        return trace_<actor<value<char const*> > >(val(str));
    }

}

///////////////////////////////////////////////////////////////////////////////
// The C++ lexer grammars.

namespace {

    template < typename ResultT >
    struct result_closure: closure<result_closure<ResultT>, ResultT> {
        member1 result_;
    };

    template <>
    struct result_closure<nil_t> {
        typedef parser_context context_t;
    };

    struct IDENTIFIER:
        grammar<IDENTIFIER, result_closure<std::string>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            definition(IDENTIFIER const& self) {
                main = (
                    lexeme_d[
                        ((alpha_p | '_' | '$') >> *(alnum_p | '_' | '$'))
                        [self.result_ = construct_<std::string>(arg1, arg2)]
                    ]
                );
            }
        };
    } IDENTIFIER;

    struct STRING_LITERAL:
        grammar<STRING_LITERAL, result_closure<std::string>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            definition(STRING_LITERAL const& self) {
                bool is_wchar = false;

                main = (
                    lexeme_d[
                        (
                            !(nocase_d[chlit<>('L')] [var(is_wchar) = true])
                         >> '\"'
                         >> *(str_p("\\\\") | "\\\"" | anychar_p - '\"' )
                        )
                        [self.result_ = construct_<std::string>(arg1, arg2)]
                     >> chlit<>('\"')
                    ] >> *lexeme_d[
                        !nocase_d[chlit<>('L')] >> '\"'
                     >> ( *( str_p("\\\\") | "\\\"" | anychar_p - '\"' ) )
                        [self.result_ += construct_<std::string>(arg1, arg2)]
                     >> chlit<>('\"')
                    ]
                );
            }
        };
    } STRING_LITERAL;

    struct CHARACTER_LITERAL:
        grammar<CHARACTER_LITERAL, result_closure<std::string>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            definition(CHARACTER_LITERAL const& self) {
                bool is_wchar = false;

                main = (
                    lexeme_d[
                        (
                            !(nocase_d[chlit<>('L')] [var(is_wchar) = true])
                         >> '\''
                         >> +(str_p("\\\\") | "\\\'" | anychar_p - '\'' )
                        )
                        [self.result_ = construct_<std::string>(arg1, arg2)]
                     >> chlit<>('\'')
                    ]
                );
            }
        };
    } CHARACTER_LITERAL;

    struct INT_CONSTANT:
        grammar<INT_CONSTANT, result_closure<std::string>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            definition(INT_CONSTANT const& self) {
                subrule<0> submain;
                subrule<1> hex_int;
                subrule<2> oct_int;
                subrule<3> dec_int;
                subrule<4> char_int;
                subrule<5> suffix_part;

                main = (
                    (
                        submain =
                            (hex_int | oct_int | dec_int | char_int) [
                                self.result_ =
                                    construct_<std::string>(arg1, arg2)
                            ],

                        hex_int =
                            lexeme_d[
                                '0' >> nocase_d[chlit<>('x')]   // prefix
                             >> +xdigit_p                       // the number
                             >> suffix_part                     // suffix
                            ],

                        oct_int =
                            lexeme_d[
                                '0'                             // prefix
                             >> +range<>('0', '7')              // the number
                             >> suffix_part                     // suffix
                            ],

                        dec_int =
                            lexeme_d[
                                +digit_p                        // the number
                             >> suffix_part                     // suffix
                            ],

                        char_int = CHARACTER_LITERAL,

                        suffix_part = !nocase_d[chlit<>('l') | chlit<>('u')]
                    )
                );
            }
        };
    } INT_CONSTANT;

    struct FLOAT_CONSTANT:
        grammar<FLOAT_CONSTANT, result_closure<std::string>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            definition(FLOAT_CONSTANT const& self) {
                subrule<0> submain;
                subrule<1> exponent_part;

                main = (
                    (
                        submain =
                            lexeme_d[
                                (
                                    chlit<>('.') >> +digit_p >> !exponent_part
                                  | +digit_p >> (
                                        (chlit<>('.') >> *digit_p)
                                     || exponent_part
                                    )
                                ) >> !nocase_d[chlit<>('l') | chlit<>('f')]
                            ]
                            [
                                self.result_ =
                                    construct_<std::string>(arg1, arg2)
                            ],

                        exponent_part =
                                nocase_d[chlit<>('e')]
                             >> !(chlit<>('+') | chlit<>('-')) >> +digit_p
                    )
                );
            }
        };
    } FLOAT_CONSTANT;

}

///////////////////////////////////////////////////////////////////////////////
// TokenID type tag.

// Internal transfer. Defined in cpp_lexer_tokens.cpp.
extern boost::spirit::parser<boost::spirit::symbols<TokenID> > const& cpp_operator_p;

namespace {

    struct get_file_position_parser {
        file_position& filePos;
        get_file_position_parser(file_position& filePos_):
            filePos(filePos_)
        {}
        typedef nil_t result_t;
        template < typename ScannerT >
        int operator()(ScannerT const& scan, result_t& result) const {
            filePos = scan.first.get_position();
            return 0;
        }
    };

    functor_parser<get_file_position_parser>
    get_file_position_p(file_position& filePos)
    {
        return get_file_position_parser(filePos);
    }

    struct token_lexer:
        grammar<token_lexer, result_closure<Token>::context_t>
    {
        template < typename ScannerT >
        struct definition {
            typedef rule<ScannerT> rule_t;
            rule_t main;

            rule_t const& start() const {
                return main;
            }

            Token token;
            file_position filePos;

            definition(token_lexer const& self);
        };
    } token_lexer;

    template < typename ScannerT >
    token_lexer::definition<ScannerT>::definition(token_lexer const& self)
    {
        subrule<0> submain;
        subrule<1> skip_until_eol;
        subrule<2> singleline_comment;
        subrule<3> multiline_comment;
        subrule<4> directive;

        main = (
            submain =
                *(blank_p | ('\\' >> eol_p))
             >> get_file_position_p(filePos)
             >> (
                    eol_p >> (
                        // TODO: Don't ignore directives like this.
                        directive         [SetEOLToken       (token, filePos)]
                      | epsilon_p         [SetEOLToken       (token, filePos)]
                    )
                  | singleline_comment    [SetCommentToken   (token, filePos)]
                  | multiline_comment     [SetCommentToken   (token, filePos)]
                  | CHARACTER_LITERAL     [SetStringToken    (token, filePos)]
                  | STRING_LITERAL        [SetStringToken    (token, filePos)]
                  | FLOAT_CONSTANT        [SetFloatingToken  (token, filePos)]
                  | INT_CONSTANT          [SetIntegerToken   (token, filePos)]
                  | IDENTIFIER            [SetIdentifierToken(token, filePos)]
                  | cpp_operator_p        [SetOperatorToken  (token, filePos)]
                )
             >> epsilon_p [self.result_ = var(token)],

            skip_until_eol = *(('\\' >> eol_p) | (anychar_p - eol_p)),

            singleline_comment = "//" >> skip_until_eol,

            multiline_comment = "/*" >> *(anychar_p - "*/") >> "*/",

            directive = *blank_p >> '#' >> skip_until_eol
        );
    }

    template < typename IteratorT >
    struct lex_input_interface_iterator: lex_input_interface {
    public:
        typedef Token result_type;
        unsigned refCount;
        IteratorT first;
        scanner<IteratorT> scan;

        lex_input_interface_iterator(IteratorT const& first_,
                                     IteratorT const& last_):
            refCount(1),
            first(first_),
            scan (first, last_)
        {}

        virtual void add_ref() {
            ++refCount;
        }
        virtual void dec_ref() {
            --refCount;
            if (refCount == 0) {
                delete this;
            }
        }

        virtual Token get() {
            Token result;
            if (token_lexer[assign(result)].parse(scan)) {
                return result;
            } else {
                return eof();
            }
        }

        virtual boost::spirit::file_position get_position() {
            return scan.first.get_position();
        }
    };

    template < typename IteratorT >
    lex_input_interface*
    NewLexerImpl(IteratorT const& first,
                 IteratorT const& last,
                 char const* fname = "<filename>")
    {
        typedef position_iterator<IteratorT> Iterator;

        Iterator pfirst(first, last, fname);
        Iterator plast;

        return new lex_input_interface_iterator<Iterator>(pfirst, plast);
    }

}

Token const& lex_input_interface::eof()
{
    static Token const result = Token(file_position(), "", EOF_token);
    return result;
}

lex_input_interface*
NewLexer(char const* first, char const* last, char const* fname)
{
    return NewLexerImpl(first, last, fname);
}

Token const lex_input_policy::eof
    = Token(boost::spirit::file_position(), "", EOF_token);
