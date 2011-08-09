/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "config.hpp"
#include "compiler.hpp"
#include "annotation.hpp"
#include "vm.hpp"

#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <set>

namespace client { namespace code_gen
{
    value::value()
      : v(0),
        is_lvalue_(false),
        builder(0)
    {}

    value::value(
        llvm::Value* v,
        bool is_lvalue_,
        llvm::IRBuilder<>* builder)
      : v(v),
        is_lvalue_(is_lvalue_),
        builder(builder)
    {}

    value::value(value const& rhs)
      : v(rhs.v),
        is_lvalue_(rhs.is_lvalue_),
        builder(rhs.builder)
    {}

    value::operator llvm::Value*() const
    {
        if (is_lvalue_)
        {
            BOOST_ASSERT(builder != 0);
            return builder->CreateLoad(v, v->getName());
        }
        return v;
    }

    value& value::operator=(value const& rhs)
    {
        v = rhs.v;
        is_lvalue_ = rhs.is_lvalue_;
        builder = rhs.builder;
        return *this;
    }

    value& value::assign(value const& rhs)
    {
        BOOST_ASSERT(is_lvalue());
        BOOST_ASSERT(builder != 0);
        builder->CreateStore(rhs, v);
        return *this;
    }

    value operator-(value a)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateNeg(a, "neg_tmp"),
            false, a.builder
        );
    }

    value operator!(value a)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateNot(a, "not_tmp"),
            false, a.builder
        );
    }

    value operator+(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateAdd(a, b, "add_tmp"),
            false, a.builder
        );
    }

    value operator-(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateSub(a, b, "sub_tmp"),
            false, a.builder
        );
    }

    value operator*(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateMul(a, b, "mul_tmp"),
            false, a.builder
        );
    }

    value operator/(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateSDiv(a, b, "div_tmp"),
            false, a.builder
        );
    }

    value operator%(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateSRem(a, b, "mod_tmp"),
            false, a.builder
        );
    }

    value operator&(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateAnd(a, b, "and_tmp"),
            false, a.builder
        );
    }

    value operator|(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateOr(a, b, "or_tmp"),
            false, a.builder
        );
    }

    value operator^(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateXor(a, b, "xor_tmp"),
            false, a.builder
        );
    }

    value operator<<(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateShl(a, b, "shl_tmp"),
            false, a.builder
        );
    }

    value operator>>(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateLShr(a, b, "shr_tmp"),
            false, a.builder
        );
    }

    value operator==(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpEQ(a, b, "eq_tmp"),
            false, a.builder
        );
    }

    value operator!=(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpNE(a, b, "ne_tmp"),
            false, a.builder
        );
    }

    value operator<(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpSLT(a, b, "slt_tmp"),
            false, a.builder
        );
    }

    value operator<=(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpSLE(a, b, "sle_tmp"),
            false, a.builder
        );
    }

    value operator>(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpSGT(a, b, "sgt_tmp"),
            false, a.builder
        );
    }

    value operator>=(value a, value b)
    {
        BOOST_ASSERT(a.builder != 0);
        return value(
            a.builder->CreateICmpSGE(a, b, "sge_tmp"),
            false, a.builder
        );
    }

    value llvm_compiler::val(unsigned int x)
    {
        return value(
            llvm::ConstantInt::get(context(), llvm::APInt(int_size, x)),
            false, &llvm_builder);
    }

    value llvm_compiler::val(int x)
    {
        return value(
            llvm::ConstantInt::get(context(), llvm::APInt(int_size, x)),
            false, &llvm_builder);
    }

    value llvm_compiler::val(bool x)
    {
        return value(
            llvm::ConstantInt::get(context(), llvm::APInt(1, x)),
            false, &llvm_builder);
    }

    value llvm_compiler::val(llvm::Value* v)
    {
        return value(v, false, &llvm_builder);
    }

    namespace
    {
        //  Create an alloca instruction in the entry block of
        //  the function. This is used for mutable variables etc.
        llvm::AllocaInst*
        create_entry_block_alloca(
            llvm::Function* f,
            char const* name,
            llvm::LLVMContext& context)
        {
            llvm::IRBuilder<> builder(
                &f->getEntryBlock(),
                f->getEntryBlock().begin());

            return builder.CreateAlloca(
                llvm::Type::getIntNTy(context, int_size), 0, name);
        }
    }

    value llvm_compiler::var(char const* name)
    {
        llvm::Function* f = llvm_builder.GetInsertBlock()->getParent();
        llvm::IRBuilder<> builder(
            &f->getEntryBlock(),
            f->getEntryBlock().begin());

        llvm::AllocaInst* alloca = builder.CreateAlloca(
            llvm::Type::getIntNTy(context(), int_size), 0, name);

        return value(alloca, true, &llvm_builder);
    }

    namespace
    {
        struct llvm_value
        {
            typedef llvm::Value* result_type;
            llvm::Value* operator()(value const& x) const
            {
                return x;
            }
        };

        template <typename C>
        llvm::Value* call_impl(
            llvm::IRBuilder<>& llvm_builder,
            function callee,
            C const& args_)
        {
            // Sigh. LLVM requires CreateCall arguments to be random access.
            // It would have been better if it can accept forward iterators.
            // I guess it needs the arguments to be in contiguous memory.
            // So, we have to put the args into a temporary std::vector.
            std::vector<llvm::Value*> args(
                args_.begin(), args_.end());

            // Check the args for null values. We can't have null values.
            // Return 0 if we find one to flag error.
            BOOST_FOREACH(llvm::Value* arg, args)
            {
                if (arg == 0)
                    return 0;
            }

            return llvm_builder.CreateCall(
                callee, args.begin(), args.end(), "call_tmp");
        }
    }

    template <typename Container>
    value llvm_compiler::call(
        function callee,
        Container const& args)
    {
        llvm::Value* call = call_impl(
            llvm_builder, callee,
            args | boost::adaptors::transformed(llvm_value()));

        if (call == 0)
            return val();
        return value(call, false, &llvm_builder);
    }

    function llvm_compiler::get_function(char const* name) const
    {
        return vm.module()->getFunction(name);
    }

    function llvm_compiler::get_current_function() const
    {
        // get the current function
        return llvm_builder.GetInsertBlock()->getParent();
    }

    void llvm_compiler::init_fpm()
    {
        // Set up the optimizer pipeline.  Start with registering info about how the
        // target lays out data structures.
        fpm.add(new llvm::TargetData(*vm.execution_engine()->getTargetData()));
        // Provide basic AliasAnalysis support for GVN.
        fpm.add(llvm::createBasicAliasAnalysisPass());
        // Promote allocas to registers.
        fpm.add(llvm::createPromoteMemoryToRegisterPass());
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        fpm.add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        fpm.add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        fpm.add(llvm::createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        fpm.add(llvm::createCFGSimplificationPass());

        fpm.doInitialization();
    }

    value compiler::operator()(unsigned int x)
    {
        return val(x);
    }

    value compiler::operator()(bool x)
    {
        return val(x);
    }

    value compiler::operator()(ast::primary_expr const& x)
    {
        return boost::apply_visitor(*this, x.get());
    }

    value compiler::operator()(ast::identifier const& x)
    {
        // Look this variable up in the function.
        if (named_values.find(x.name) == named_values.end())
        {
            error_handler(x.id, "Undeclared variable: " + x.name);
            return val();
        }
        return named_values[x.name];
    }

    value compiler::operator()(ast::unary_expr const& x)
    {
        value operand = boost::apply_visitor(*this, x.operand_);
        if (!operand.is_valid())
            return val();

        switch (x.operator_)
        {
            case token_ids::compl_:     return operand ^ val(-1);
            case token_ids::minus:      return -operand;
            case token_ids::not_:       return !operand;
            case token_ids::plus:       return operand;
            case token_ids::plus_plus:
            {
                if (!operand.is_lvalue())
                {
                    error_handler(x.id, "++ needs an var");
                    return val();
                }

                value r = operand + val(1);
                operand.assign(r);
                return operand;
            }
            case token_ids::minus_minus:
            {
                if (!operand.is_lvalue())
                {
                    error_handler(x.id, "-- needs an var");
                    return val();
                }

                value r = operand - val(1);
                operand.assign(r);
                return operand;
            }
            default:
                BOOST_ASSERT(0);
                return val();
        }
    }

    namespace
    {
        struct compile_args
        {
            compiler& c;
            compile_args(compiler& c) : c(c) {}

            typedef value result_type;
            value operator()(ast::expression const& expr) const
            {
                return c(expr);
            }
        };
    }

    value compiler::operator()(ast::function_call const& x)
    {
        function callee = get_function(x.function_name.name);
        if (!callee)
        {
            error_handler(x.function_name.id,
                "Function not found: " + x.function_name.name);
            return val();
        }

        if (callee.arg_size() != x.args.size())
        {
            error_handler(x.function_name.id,
                "Wrong number of arguments: " + x.function_name.name);
            return val();
        }

        return call(callee,
            x.args | boost::adaptors::transformed(compile_args(*this)));
    }

    namespace
    {
        int precedence[] = {
            // precedence 1
            1, // op_comma

            // precedence 2
            2, // op_assign
            2, // op_plus_assign
            2, // op_minus_assign
            2, // op_times_assign
            2, // op_divide_assign
            2, // op_mod_assign
            2, // op_bit_and_assign
            2, // op_bit_xor_assign
            2, // op_bitor_assign
            2, // op_shift_left_assign
            2, // op_shift_right_assign

            // precedence 3
            3, // op_logical_or

            // precedence 4
            4, // op_logical_and

            // precedence 5
            5, // op_bit_or

            // precedence 6
            6, // op_bit_xor

            // precedence 7
            7, // op_bit_and

            // precedence 8
            8, // op_equal
            8, // op_not_equal

            // precedence 9
            9, // op_less
            9, // op_less_equal
            9, // op_greater
            9, // op_greater_equal

            // precedence 10
            10, // op_shift_left
            10, // op_shift_right

            // precedence 11
            11, // op_plus
            11, // op_minus

            // precedence 12
            12, // op_times
            12, // op_divide
            12 // op_mod
        };
    }

    inline int precedence_of(token_ids::type op)
    {
        return precedence[op & 0xFF];
    }

    value compiler::compile_binary_expression(
        value lhs, value rhs, token_ids::type op)
    {
        switch (op)
        {
            case token_ids::plus:           return lhs + rhs;
            case token_ids::minus:          return lhs - rhs;
            case token_ids::times:          return lhs * rhs;
            case token_ids::divide:         return lhs / rhs;
            case token_ids::mod:            return lhs % rhs;

            case token_ids::logical_or:
            case token_ids::bit_or:         return lhs | rhs;

            case token_ids::logical_and:
            case token_ids::bit_and:        return lhs & rhs;

            case token_ids::bit_xor:        return lhs ^ rhs;
            case token_ids::shift_left:     return lhs << rhs;
            case token_ids::shift_right:    return lhs >> rhs;

            case token_ids::equal:          return lhs == rhs;
            case token_ids::not_equal:      return lhs != rhs;
            case token_ids::less:           return lhs < rhs;
            case token_ids::less_equal:     return lhs <= rhs;
            case token_ids::greater:        return lhs > rhs;
            case token_ids::greater_equal:  return lhs >= rhs;

            default: BOOST_ASSERT(0);       return val();
        }
    }

    // The Shunting-yard algorithm
    value compiler::compile_expression(
        int min_precedence,
        value lhs,
        std::list<ast::operation>::const_iterator& rest_begin,
        std::list<ast::operation>::const_iterator rest_end)
    {
        while ((rest_begin != rest_end) &&
            (precedence_of(rest_begin->operator_) >= min_precedence))
        {
            token_ids::type op = rest_begin->operator_;
            value rhs = boost::apply_visitor(*this, rest_begin->operand_);
            if (!rhs.is_valid())
                return val();
            ++rest_begin;

            while ((rest_begin != rest_end) &&
                (precedence_of(rest_begin->operator_) > precedence_of(op)))
            {
                token_ids::type next_op = rest_begin->operator_;
                rhs = compile_expression(
                    precedence_of(next_op), rhs, rest_begin, rest_end);
            }

            lhs = compile_binary_expression(lhs, rhs, op);
        }
        return lhs;
    }

    value compiler::operator()(ast::expression const& x)
    {
        value lhs = boost::apply_visitor(*this, x.first);
        if (!lhs.is_valid())
            return val();
        std::list<ast::operation>::const_iterator rest_begin = x.rest.begin();
        return compile_expression(0, lhs, rest_begin, x.rest.end());
    }

    value compiler::operator()(ast::assignment const& x)
    {
        if (named_values.find(x.lhs.name) == named_values.end())
        {
            error_handler(x.lhs.id, "Undeclared variable: " + x.lhs.name);
            return val();
        }

        value lhs = named_values[x.lhs.name];
        value rhs = (*this)(x.rhs);
        if (!rhs.is_valid())
            return val();

        if (x.operator_ == token_ids::assign)
        {
            lhs.assign(rhs);
            return lhs;
        }

        value result;
        switch (x.operator_)
        {
            case token_ids::plus_assign:        result = lhs + rhs; break;
            case token_ids::minus_assign:       result = lhs - rhs; break;
            case token_ids::times_assign:       result = lhs * rhs; break;
            case token_ids::divide_assign:      result = lhs / rhs; break;
            case token_ids::mod_assign:         result = lhs % rhs; break;
            case token_ids::bit_and_assign:     result = lhs & rhs; break;
            case token_ids::bit_xor_assign:     result = lhs ^ rhs; break;
            case token_ids::bit_or_assign:      result = lhs | rhs; break;
            case token_ids::shift_left_assign:  result = lhs << rhs; break;
            case token_ids::shift_right_assign: result = lhs >> rhs; break;
            default: BOOST_ASSERT(0);           return val();
        }

        lhs.assign(result);
        return lhs;
    }

    bool compiler::operator()(ast::variable_declaration const& x)
    {
        if (named_values.find(x.lhs.name) != named_values.end())
        {
            error_handler(x.lhs.id, "Duplicate variable: " + x.lhs.name);
            return false;
        }

        value init;
        std::string const& name = x.lhs.name;

        if (x.rhs) // if there's an RHS initializer
        {
            init = (*this)(*x.rhs);
            if (!init.is_valid()) // don't add the variable if the RHS fails
                return false;
        }

        value var_ = var(name.c_str());
        if (init)
            var_.assign(init);

        // Remember this binding.
        named_values[name] = var_;
        return true;
    }

    struct compiler::statement_compiler : compiler
    {
        typedef bool result_type;
    };

    compiler::statement_compiler& compiler::as_statement()
    {
        return *static_cast<statement_compiler*>(this);
    }

    bool compiler::operator()(ast::statement const& x)
    {
        if (boost::get<ast::nil>(&x) != 0) // empty statement
            return true;
        return boost::apply_visitor(as_statement(), x);
    }

    bool compiler::operator()(ast::statement_list const& x)
    {
        BOOST_FOREACH(ast::statement const& s, x)
        {
            if (!(*this)(s))
                return false;
        }
        return true;
    }

    bool compiler::operator()(ast::if_statement const& x)
    {
        value condition = (*this)(x.condition);
        if (!condition.is_valid())
            return false;

        llvm::Function* f = get_current_function();

        // Create blocks for the then and else cases.  Insert the 'then' block at the
        // end of the function.
        llvm::BasicBlock* then_block = llvm::BasicBlock::Create(context(), "if.then", f);
        llvm::BasicBlock* else_block = 0;
        llvm::BasicBlock* exit_block = 0;

        if (x.else_)
        {
            else_block = llvm::BasicBlock::Create(context(), "if.else");
            builder().CreateCondBr(condition, then_block, else_block);
        }
        else
        {
            exit_block = llvm::BasicBlock::Create(context(), "if.end");
            builder().CreateCondBr(condition, then_block, exit_block);
        }

        // Emit then value.
        builder().SetInsertPoint(then_block);
        if (!(*this)(x.then))
            return false;
        if (then_block->getTerminator() == 0)
        {
            if (exit_block == 0)
                exit_block = llvm::BasicBlock::Create(context(), "if.end");
            builder().CreateBr(exit_block);
        }
        // Codegen of 'then' can change the current block, update then_block
        then_block = builder().GetInsertBlock();

        if (x.else_)
        {
            // Emit else block.
            f->getBasicBlockList().push_back(else_block);
            builder().SetInsertPoint(else_block);
            if (!(*this)(*x.else_))
                return false;
            if (else_block->getTerminator() == 0)
            {
                if (exit_block == 0)
                    exit_block = llvm::BasicBlock::Create(context(), "if.end");
                builder().CreateBr(exit_block);
            }
            // Codegen of 'else' can change the current block, update else_block
            else_block = builder().GetInsertBlock();
        }

        if (exit_block != 0)
        {
            // Emit exit block
            f->getBasicBlockList().push_back(exit_block);
            builder().SetInsertPoint(exit_block);
        }
        return true;
    }

    bool compiler::operator()(ast::while_statement const& x)
    {
        llvm::Function* f = get_current_function();

        llvm::BasicBlock* cond_block = llvm::BasicBlock::Create(context(), "while.cond", f);
        llvm::BasicBlock* body_block = llvm::BasicBlock::Create(context(), "while.body");
        llvm::BasicBlock* exit_block = llvm::BasicBlock::Create(context(), "while.end");

        builder().CreateBr(cond_block);
        builder().SetInsertPoint(cond_block);
        value condition = (*this)(x.condition);
        if (!condition.is_valid())
            return false;
        builder().CreateCondBr(condition, body_block, exit_block);
        f->getBasicBlockList().push_back(body_block);
        builder().SetInsertPoint(body_block);

        if (!(*this)(x.body))
            return false;

        if (body_block->getTerminator() == 0)
            builder().CreateBr(cond_block); // loop back

        // Emit exit block
        f->getBasicBlockList().push_back(exit_block);
        builder().SetInsertPoint(exit_block);

        return true;
    }

    bool compiler::operator()(ast::return_statement const& x)
    {
        if (void_return)
        {
            if (x.expr)
            {
                error_handler(
                    x.id, "'void' function returning a value: ");
                return false;
            }
        }
        else
        {
            if (!x.expr)
            {
                error_handler(
                    x.id, current_function_name
                    + " function must return a value: ");
                return false;
            }
        }

        if (x.expr)
        {
            value return_val = (*this)(*x.expr);
            if (!return_val.is_valid())
                return false;
            return_var.assign(return_val);
        }

        builder().CreateBr(return_block);
        return true;
    }

    llvm::Function* compiler::function_decl(ast::function const& x)
    {
        void_return = x.return_type == "void";
        current_function_name = x.function_name.name;

        llvm::Type* int_type =
            llvm::Type::getIntNTy(context(), int_size);
        llvm::Type* void_type = llvm::Type::getVoidTy(context());

        std::vector<llvm::Type*> ints(x.args.size(), int_type);
        llvm::Type* return_type = void_return ? void_type : int_type;

        llvm::FunctionType* function_type =
            llvm::FunctionType::get(void_return ? void_type : int_type, ints, false);

        llvm::Function* f =
            llvm::Function::Create(
                function_type, llvm::Function::ExternalLinkage,
                current_function_name, vm.module());

        // If function conflicted, the function already exixts. If it has a
        // body, don't allow redefinition or reextern.
        if (f->getName() != current_function_name)
        {
            // Delete the one we just made and get the existing one.
            f->eraseFromParent();
            f = get_function(current_function_name);

            // If function already has a body, reject this.
            if (!f->empty())
            {
                error_handler(
                    x.function_name.id,
                    "Duplicate function: " + x.function_name.name);
                return 0;
            }

            // If function took a different number of args, reject.
            if (f->arg_size() != x.args.size())
            {
                error_handler(
                    x.function_name.id,
                    "Redefinition of function with different # args: "
                        + x.function_name.name);
                return 0;
            }

            // Set names for all arguments.
            llvm::Function::arg_iterator iter = f->arg_begin();
            BOOST_FOREACH(ast::identifier const& arg, x.args)
            {
                iter->setName(arg.name);
                ++iter;
            }
        }
        return f;
    }

    void compiler::function_allocas(ast::function const& x, llvm::Function* f)
    {
        // Create an variables for each argument and register the
        // argument in the symbol table so that references to it will succeed.
        llvm::Function::arg_iterator iter = f->arg_begin();
        BOOST_FOREACH(ast::identifier const& arg, x.args)
        {
            // Create an arg_ for this variable.
            value arg_ = var(arg.name.c_str());

            // Store the initial value into the arg_.
            arg_.assign(val(iter));

            // Add arguments to variable symbol table.
            named_values[arg.name] = arg_;
            ++iter;
        }

        if (!void_return)
        {
            // Create an alloca for the return value
            return_var = var("return.val");
        }
    }

    bool compiler::operator()(ast::function const& x)
    {
        ///////////////////////////////////////////////////////////////////////
        // the signature:
        llvm::Function* f = function_decl(x);
        if (f == 0)
            return false;

        ///////////////////////////////////////////////////////////////////////
        // the body:
        if (x.body) // compile the body if this is not a prototype
        {
            // Create a new basic block to start insertion into.
            llvm::BasicBlock* block =
                llvm::BasicBlock::Create(context(), "entry", f);
            builder().SetInsertPoint(block);

            function_allocas(x, f);
            return_block = llvm::BasicBlock::Create(context(), "return");

            if (!(*this)(*x.body))
            {
                // Error reading body, remove function.
                f->eraseFromParent();
                return false;
            }

            llvm::BasicBlock* last_block =
                &f->getBasicBlockList().back();

            // If the last block is unterminated, connect it to return_block
            if (last_block->getTerminator() == 0)
            {
                builder().SetInsertPoint(last_block);
                builder().CreateBr(return_block);
            }

            f->getBasicBlockList().push_back(return_block);
            builder().SetInsertPoint(return_block);

            if (void_return)
                builder().CreateRetVoid();
            else
                builder().CreateRet(return_var);

            //~ vm.module()->dump();

            // Validate the generated code, checking for consistency.
            llvm::verifyFunction(*f);

            // Optimize the function.
            fpm.run(*f);
        }

        return true;
    }

    bool compiler::operator()(ast::function_list const& x)
    {
        BOOST_FOREACH(ast::function const& f, x)
        {
            named_values.clear(); // clear the variables
            if (!(*this)(f))
                return false;
        }
        return true;
    }
}}

