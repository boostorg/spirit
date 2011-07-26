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
    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    unsigned const int_size = 32;

    struct compiler
    {
        typedef llvm::Value* result_type;

        template <typename ErrorHandler>
        compiler(vmachine& vm, ErrorHandler& error_handler_)
          : vm(vm),
            fpm(vm.module()),
            builder(context())
        {
            using namespace boost::phoenix::arg_names;
            namespace phx = boost::phoenix;
            using boost::phoenix::function;

            error_handler = function<ErrorHandler>(error_handler_)(
                "Error! ", _2, phx::cref(error_handler_.iters)[_1]);

            init_fpm();
        }

        llvm::Value* operator()(ast::nil) { BOOST_ASSERT(0); return 0; }
        llvm::Value* operator()(unsigned int x);
        llvm::Value* operator()(bool x);
        llvm::Value* operator()(ast::literal const& x);
        llvm::Value* operator()(ast::identifier const& x);
        llvm::Value* operator()(ast::unary const& x);
        llvm::Value* operator()(ast::function_call const& x);
        llvm::Value* operator()(ast::expression const& x);
        llvm::Value* operator()(ast::assignment const& x);

        bool operator()(ast::variable_declaration const& x);
        bool operator()(ast::statement_list const& x);
        bool operator()(ast::statement const& x);
        bool operator()(ast::if_statement const& x);
        bool operator()(ast::while_statement const& x);
        bool operator()(ast::return_statement const& x);
        bool operator()(ast::function const& x);
        bool operator()(ast::function_list const& x);

    private:

        boost::function<
            void(int tag, std::string const& what)>
        error_handler;

        bool void_return;
        std::string current_function_name;
        std::map<std::string, llvm::AllocaInst*> named_values;
        llvm::BasicBlock* return_block;
        llvm::AllocaInst* return_alloca;

        vmachine& vm;
        llvm::IRBuilder<> builder;
        llvm::FunctionPassManager fpm;

        llvm::LLVMContext& context() const
        { return llvm::getGlobalContext(); }

        llvm::Module* init_llvm();
        void init_fpm();

        llvm::Value* compile_binary_expression(
            llvm::Value* lhs, llvm::Value* rhs, token::type op);

        llvm::Value* compile_expression(
            int min_precedence,
            llvm::Value* lhs,
            std::list<ast::operation>::const_iterator& rest_begin,
            std::list<ast::operation>::const_iterator rest_end);

        struct statement_compiler;
        statement_compiler& as_statement();

        llvm::Function* function_decl(ast::function const& x);
        void function_allocas(ast::function const& x, llvm::Function* function);
    };
}}

#endif