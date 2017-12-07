//
// Created by lukas on 11.11.16.
//

#ifndef LINKER_ERROR_EXAMPLE_AST_HPP_HPP
#define LINKER_ERROR_EXAMPLE_AST_HPP_HPP

#include <boost/fusion/adapted/struct.hpp>
//#define BOOST_SPIRIT_X3_DEBUG
//#define PRAGMA_X3_DEBUG
#ifdef PRAGMA_X3_DEBUG
#ifdef BOOST_SPIRIT_X3_DEBUG
    #pragma message "yesdef(BOOST_SPIRIT_X3_DEBUG)"
#else
    #pragma message "notdef(BOOST_SPIRIT_X3_DEBUG)"
#endif
#endif//PRAGMA_X3_DEBUG
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#define VARIABLE_DEC_OPTIONAL
  //The reason for this macro is apparent
  //in example_def.hpp
//#define PRAGMA_VARIABLE_OPTIONAL
#ifdef PRAGMA_VARIABLE_OPTIONAL
#ifdef VARIABLE_DEC_OPTIONAL
    #pragma message "yesdef(VARIABLE_DEC_OPTIONAL)"
    #include <boost/optional/optional_io.hpp>//in response to compile-time err msg.
#else
    #pragma message "notdef(VARIABLE_DEC_OPTIONAL)"
#endif
#endif//PRAGMA_VARIABLE_OPTIONAL

#include <vector>
#include <string>

namespace client { namespace ast {

namespace x3 = boost::spirit::x3;

struct LambdaType;
struct ClassType;
struct nil{};

typedef x3::variant <
    nil,
    x3::forward_ast<LambdaType>,
    x3::forward_ast<ClassType>
> Type;

struct LambdaType {
    std::vector<Type> parameters_;
    Type return_type_;
};

struct ClassType {
    std::vector<std::string> name_;
    std::vector<Type> template_args_;
};

struct VariableDec {
    std::string _name;
  #ifdef VARIABLE_DEC_OPTIONAL
      boost::optional<Type> 
  #else
      Type
  #endif
    _type;
};

}}

BOOST_FUSION_ADAPT_STRUCT(client::ast::LambdaType, parameters_, return_type_)
BOOST_FUSION_ADAPT_STRUCT(client::ast::ClassType, name_, template_args_)
BOOST_FUSION_ADAPT_STRUCT(client::ast::VariableDec, _name, _type)

#endif //LINKER_ERROR_EXAMPLE_AST_HPP_HPP
