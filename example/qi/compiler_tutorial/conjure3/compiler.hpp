/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_COMPILER_HPP)
#define BOOST_SPIRIT_CONJURE_COMPILER_HPP

#include "ast.hpp"
#include "error_handler.hpp"
#include "vm.hpp"
#include <map>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/IRBuilder.h>

namespace client { namespace code_gen
{
    unsigned const int_size = 32;
    struct compiler;
    struct llvm_compiler;

    ///////////////////////////////////////////////////////////////////////////
    //  The Value (light abstraction of an LLVM::Value
    ///////////////////////////////////////////////////////////////////////////
    struct value
    {
        value();
        value(value const& rhs);
        operator llvm::Value*() const;

        value& operator=(value const& rhs);
        bool is_lvalue() const { return is_lvalue_; }
        bool is_valid() const { return v != 0; }

        value& assign(value const& rhs);

        friend value operator-(value a);
        friend value operator!(value a);
        friend value operator+(value a, value b);
        friend value operator-(value a, value b);
        friend value operator*(value a, value b);
        friend value operator/(value a, value b);
        friend value operator%(value a, value b);

        friend value operator&(value a, value b);
        friend value operator|(value a, value b);
        friend value operator^(value a, value b);
        friend value operator<<(value a, value b);
        friend value operator>>(value a, value b);

        friend value operator==(value a, value b);
        friend value operator!=(value a, value b);
        friend value operator<(value a, value b);
        friend value operator<=(value a, value b);
        friend value operator>(value a, value b);
        friend value operator>=(value a, value b);

    protected:

        friend struct llvm_compiler;

        value(
            llvm::Value* v,
            bool is_lvalue_,
            llvm::IRBuilder<>* builder);

        llvm::LLVMContext& context() const
        { return llvm::getGlobalContext(); }

    private:

        llvm::Value* v;
        bool is_lvalue_;
        llvm::IRBuilder<>* builder;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The LLVM Compiler. Lower level compiler (does not deal with ASTs)
    ///////////////////////////////////////////////////////////////////////////
    struct llvm_compiler
    {
        llvm_compiler(vmachine& vm)
          : llvm_builder(context())
          , vm(vm)
          , fpm(vm.module())
        {
            init_fpm();
        }

        value val() { return value(); }
        value val(unsigned int x);
        value val(int x);
        value val(bool x);
        value val(llvm::Value* v);

        value var(char const* name);

        value call(
            llvm::Function* callee,
            std::vector<llvm::Value*> const& args);

    protected:

        llvm::LLVMContext& context() const
        { return llvm::getGlobalContext(); }

        llvm::IRBuilder<>& builder()
        { return llvm_builder; }

        vmachine& vm; // $$$ protected for now $$$
        llvm::FunctionPassManager fpm; // $$$ protected for now $$$

    private:

        void init_fpm();
        llvm::IRBuilder<> llvm_builder;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    struct compiler : llvm_compiler
    {
        typedef value result_type;

        template <typename ErrorHandler>
        compiler(vmachine& vm, ErrorHandler& error_handler_)
          : llvm_compiler(vm)
        {
            using namespace boost::phoenix::arg_names;
            namespace phx = boost::phoenix;
            using boost::phoenix::function;

            error_handler = function<ErrorHandler>(error_handler_)(
                "Error! ", _2, phx::cref(error_handler_.iters)[_1]);
        }

        value operator()(ast::nil) { BOOST_ASSERT(0); return val(); }
        value operator()(unsigned int x);
        value operator()(bool x);
        value operator()(ast::primary_expr const& x);
        value operator()(ast::identifier const& x);
        value operator()(ast::unary_expr const& x);
        value operator()(ast::function_call const& x);
        value operator()(ast::expression const& x);
        value operator()(ast::assignment const& x);

        bool operator()(ast::variable_declaration const& x);
        bool operator()(ast::statement_list const& x);
        bool operator()(ast::statement const& x);
        bool operator()(ast::if_statement const& x);
        bool operator()(ast::while_statement const& x);
        bool operator()(ast::return_statement const& x);
        bool operator()(ast::function const& x);
        bool operator()(ast::function_list const& x);

    private:

        value compile_binary_expression(
            value lhs, value rhs, token_ids::type op);

        value compile_expression(
            int min_precedence,
            value lhs,
            std::list<ast::operation>::const_iterator& rest_begin,
            std::list<ast::operation>::const_iterator rest_end);

        struct statement_compiler;
        statement_compiler& as_statement();

        llvm::Function* function_decl(ast::function const& x);
        void function_allocas(ast::function const& x, llvm::Function* function);

        boost::function<
            void(int tag, std::string const& what)>
        error_handler;

        bool void_return;
        std::string current_function_name;
        std::map<std::string, value> named_values;
        llvm::BasicBlock* return_block;
        value return_var;
    };
}}

#endif