/*=============================================================================
    A Standard compliant C++ preprocessor

    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_EXCEPTIONS_HPP__5190E447_A781_4521_A275_5134FF9917D7__INCLUDED_)
#define _EXCEPTIONS_HPP__5190E447_A781_4521_A275_5134FF9917D7__INCLUDED_

#include <exception>
#include <string>

#include <boost/assert.hpp>
#include <boost/config.hpp>

///////////////////////////////////////////////////////////////////////////////
// helper macro for throwing exceptions
#ifdef BOOST_NO_STRINGSTREAM
#include <strstream>
#define CPP_THROW(cls, code, msg, act_tok) \
    { \
    using namespace cpp; \
    std::strstream stream; \
        stream << cls::severity_text(cls::code) << ": " \
        << cls::error_text(cls::code) << ": " << msg << std::ends; \
    std::string throwmsg = stream.str(); stream.freeze(false); \
    throw cls(throwmsg.c_str(), cls::code, act_tok.get_position().line, \
        act_tok.get_position().column, act_tok.get_position().file.c_str()); \
    } \
    /**/
#else
#include <sstream>
#define CPP_THROW(cls, code, msg, act_tok) \
    { \
    using namespace cpp; \
    std::stringstream stream; \
        stream << cls::severity_text(cls::code) << ": " \
        << cls::error_text(cls::code) << ": " << msg << std::ends; \
    throw cls(stream.str().c_str(), cls::code, act_tok.get_position().line, \
        act_tok.get_position().column, act_tok.get_position().file.c_str()); \
    } \
    /**/
#endif // BOOST_NO_STRINGSTREAM

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

///////////////////////////////////////////////////////////////////////////////
// generic file related exception
namespace severity_util {

    enum severity {
        severity_remark = 0,
        severity_warning,
        severity_error,
        severity_fatal
    };
    
    inline char const *
    get_severity(severity level) 
    {
        static char const *severity_text[] = 
        {
            "remark",           // severity_remark
            "warning",          // severity_warning
            "error",            // severity_error
            "fatal error"       // severity_fatal
        };
        BOOST_ASSERT(severity_remark <= level && level <= severity_fatal);
        return severity_text[level];
    }
}

///////////////////////////////////////////////////////////////////////////////
//  cpp_exception, the base class for all specific C preprocessor exceptions 
class cpp_exception
:   public std::exception
{
public:
    cpp_exception(int line_, int column_, char const *filename_) throw() 
    :   line(line_), column(column_) 
    {
        unsigned int off = 0;
        while (off < sizeof(filename) && *filename_)
            filename[off++] = *filename_++;
        filename[off] = 0;
    }
    ~cpp_exception() throw() {}
    
    virtual char const *what() const throw() = 0;   // to be overloaded
    virtual char const *description() const throw() = 0;
    
    int line_no() const throw() { return line; }
    int column_no() const throw() { return column; }
    char const *file_name() const throw() { return filename; }
    
protected:
    char filename[512];
    int line;
    int column;
};

///////////////////////////////////////////////////////////////////////////////
//  
class file_exception :
    public cpp_exception
{
public:
    enum error_code {
        invalid_argument = 0,
        missing_unc
    };

    file_exception(char const *what_, error_code, int line_, int column_, 
            char const *filename_) throw()
    :   cpp_exception(line_, column_, filename_)
    {
        unsigned int off = 0;
        while (off < sizeof(buffer) && *what_)
            buffer[off++] = *what_++;
        buffer[off] = 0;
    }
    ~file_exception() throw() {}
    
    virtual char const *what() const throw()
    {
        return "cpp::file_exception";
    }
    virtual char const *description() const throw()
    {
        return buffer;
    }

    static char const *error_text(int code)
    {
    // error texts in this array must apear in the same order as the items in
    // the error enum above
        static char const *file_exception_errors[] = 
        {
            "Invalid argument",                        // invalid_argument
            "The object must have an UNC (Universal Naming Convention) component",
                                                        // missing_unc
        };
        return file_exception_errors[code];
    }

    static char const *severity_text(int code)
    {
        static severity_util::severity file_exception_severity[] = 
        {
            severity_util::severity_error,              // invalid_argument
            severity_util::severity_error               // missing_unc
        };
        return severity_util::get_severity(file_exception_severity[code]);
    }

private:
    char buffer[512];
};

///////////////////////////////////////////////////////////////////////////////
// preprocessor error
class preprocess_exception :
    public cpp_exception
{
public:
    enum error_code {
        unexpected_error = 0,
        macro_redefinition,
        macro_insertion_error,
        bad_include_file,
        bad_include_statement,
        ill_formed_directive,
        error_directive,
        warning_directive,
        ill_formed_expression,
        missing_matching_if,
        ill_formed_operator
    };

    preprocess_exception(char const *what_, error_code code, int line_, 
        int column_, char const *filename_) throw() 
    :   cpp_exception(line_, column_, filename_), level(severity_level(code))
    {
        unsigned int off = 0;
        while (off < sizeof(buffer) && *what_)
            buffer[off++] = *what_++;
        buffer[off] = 0;
    }
    ~preprocess_exception() throw() {}
    
    virtual char const *what() const throw()
    {
        return "cpp::preprocess_exception";
    }
    virtual char const *description() const throw()
    {
        return buffer;
    }
    severity_util::severity get_severity()
    {
        return level;
    }

    static char const *error_text(int code)
    {
    // error texts in this array must apear in the same order as the items in
    // the error enum above
        static char const *preprocess_exception_errors[] = {
            "unexpected error (should not happen)",     // unexpected_error
            "illegal macro redefinition",               // macro_redefinition
            "macro definition failed (out of memory?)", // macro_insertion_error
            "could not find include file",              // bad_include_file
            "ill formed include statement",             // bad_include_statement
            "unknown preprocessor directive (ignored)", // ill_formed_directive
            "encountered #error directive",             // error_directive
            "encountered #warning directive",           // warning_directive
            "ill formed preprocessor expression",       // ill_formed_expression
            "the #if for this directive is missing",    // missing_matching_if
            "ill formed preprocessing operator"         // ill_formed_operator
        };
        return preprocess_exception_errors[code];
    }

    static severity_util::severity severity_level(int code)
    {
        static severity_util::severity preprocess_exception_severity[] = {
            severity_util::severity_fatal,              // unexpected_error
            severity_util::severity_warning,            // macro_redefinition
            severity_util::severity_fatal,              // macro_insertion_error
            severity_util::severity_error,              // bad_include_file
            severity_util::severity_error,              // bad_include_statement
            severity_util::severity_error,              // ill_formed_directive
            severity_util::severity_fatal,              // error_directive
            severity_util::severity_warning,            // warning_directive
            severity_util::severity_error,              // ill_formed_expression
            severity_util::severity_error,              // missing_matching_if
            severity_util::severity_error               // ill_formed_operator
        };
        return preprocess_exception_severity[code];
    }
    static char const *severity_text(int code)
    {
        return severity_util::get_severity(severity_level(code));
    }

private:
    char buffer[512];
    severity_util::severity level;
};

///////////////////////////////////////////////////////////////////////////////
// abort compilation
struct abort_preprocess_exception 
:   public cpp_exception
{
    abort_preprocess_exception(int error_count_, int level_, int line_, 
        int column_, char const *filename_) throw() 
    :   cpp_exception(line_, column_, filename_)
    {
    }
    ~abort_preprocess_exception() throw() {}
    
    virtual char const *what() const throw()
    {
        return "cpp::abort_preprocess_exception";
    }
    virtual char const *description() const throw()
    {
        return "fatal error";
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_EXCEPTIONS_HPP__5190E447_A781_4521_A275_5134FF9917D7__INCLUDED_)
