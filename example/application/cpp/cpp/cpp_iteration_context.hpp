/*=============================================================================
    A Standard compliant C++ preprocessor

    Definition of the preprocessor context
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software
    is granted provided this copyright notice appears in all copies.
    This software is provided "as is" without express or implied
    warranty, and with no claim as to its suitability for any purpose.

    See Copyright.txt for full copyright notices and acknowledgements.
=============================================================================*/

#if !defined(_CPP_ITERATION_CONTEXT_HPP__00312288_9DDB_4668_AFE5_25D3994FD095__INCLUDED_)
#define _CPP_ITERATION_CONTEXT_HPP__00312288_9DDB_4668_AFE5_25D3994FD095__INCLUDED_

#include <iterator>

///////////////////////////////////////////////////////////////////////////////
namespace cpp {

namespace iteration_context_policies {

///////////////////////////////////////////////////////////////////////////////
//
//      The iteration_context_policies templates are policies for the 
//      cpp::iteration_context which allows to control, how a given input file 
//      is to be represented by a pair of iterators pointing to the begin and 
//      the end of the resulting input sequence.
//
///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    //
    //  load_file_to_string
    //
    //      Loads a file into a string and returns the iterators pointing to 
    //      the beginning and the end of the loaded string.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT>
    struct load_file_to_string {
    
        template <typename IterContextT>
        class inner {
        
        public:
            template <typename TokenT>
            static 
            void init_iterators(IterContextT &iter_ctx, TokenT const &act_tok)
            {
                std::ifstream instream(iter_ctx.filename.c_str());
                if (!instream.is_open()) {
                    CPP_THROW(preprocess_exception, bad_include_file, 
                        iter_ctx.filename, act_tok);
                }
                
                iter_ctx.instring = std::string(
                    std::istreambuf_iterator<char>(instream.rdbuf()),
                    std::istreambuf_iterator<char>());
                    
                iter_ctx.first = IteratorT(iter_ctx.instring.begin(), 
                    iter_ctx.instring.end(), iter_ctx.filename);
                iter_ctx.last = IteratorT();
            }

        private:
            std::string instring;
        };
    };
    
///////////////////////////////////////////////////////////////////////////////
//
//  load_file
//
//      The load_file policy opens a given file and returns the wrapped
//      istreambuf_iterators.
//
///////////////////////////////////////////////////////////////////////////////
    template <typename IteratorT>
    struct load_file {
            
        template <typename IterContextT>
        class inner {

        public:
            ~inner() { if (instream.is_open()) instream.close(); }
            
            template <typename TokenT>
            static 
            void init_iterators(IterContextT &iter_ctx, TokenT const &act_tok)
            {
                iter_ctx.instream.open(iter_ctx.filename.c_str());
                if (!iter_ctx.instream.is_open()) {
                    CPP_THROW(preprocess_exception, bad_include_file, 
                        iter_ctx.filename, act_tok);
                }
                
                using boost::spirit::make_multi_pass;
                iter_ctx.first = IteratorT(
                    make_multi_pass(std::istreambuf_iterator<char>(
                        iter_ctx.instream.rdbuf())),
                    make_multi_pass(std::istreambuf_iterator<char>()));
                iter_ctx.last = IteratorT();
            }

        private:
            std::ifstream instream;
        };
    };
    
}   // namespace context_policies

///////////////////////////////////////////////////////////////////////////////
//  
template <typename IteratorT>
struct base_iteration_context {

public:
    base_iteration_context(std::string const &fname)
    :   filename(fname)
    {}
    base_iteration_context(IteratorT const &first_, IteratorT const &last_, 
            std::string const &fname)
    :   first(first_), last(last_), filename(fname)
    {}

// the actual input stream
    IteratorT first;            // actual input stream position 
    IteratorT last;             // end of input stream
    std::string filename;       // actual processed file
};

///////////////////////////////////////////////////////////////////////////////
//  
template <
    typename IteratorT, 
    typename InputPolicyT = 
        iteration_context_policies::load_file_to_string<IteratorT> 
>
struct iteration_context
:   public base_iteration_context<IteratorT>,
    public InputPolicyT::template 
        inner<iteration_context<IteratorT, InputPolicyT> >
{
    typedef iteration_context<IteratorT, InputPolicyT> self_t;
    typedef typename IteratorT::token_t token_t;

    iteration_context(std::string const &fname, token_t const &act_tok) 
    :   base_iteration_context<IteratorT>(fname)
    {
        InputPolicyT::template inner<self_t>::init_iterators(*this, act_tok);
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpp

#endif // !defined(_CPP_ITERATION_CONTEXT_HPP__00312288_9DDB_4668_AFE5_25D3994FD095__INCLUDED_)
