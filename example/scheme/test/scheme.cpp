/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include "../input/sexpr.hpp"
#include "../input/parse_sexpr_impl.hpp"
#include <iostream>
#include <fstream>

inline std::ostream& println(std::ostream& out, scheme::utree const& val)
{
    out << val << std::endl;
    return out;
}

#include <boost/unordered_map.hpp>

namespace scheme
{
    class environment
    {
    public:

        environment(environment* parent = 0)
          : bindings(), parent(parent) {}

        void define(std::string const& name, utree const& def)
        {
            // check for duplicate names
            BOOST_ASSERT(bindings.find(name) == bindings.end());
            // $$$ TODO Use exceptions $$$
            bindings[name] = def;
        }

        utree* find(std::string const& name)
        {
            map::iterator i = bindings.find(name);
            if (i == bindings.end())
            {
                if (parent)
                    return parent->find(name);
                return 0;
            }
            return &i->second;
        }

    private:

        typedef boost::unordered_map<std::string, utree> map;
        map bindings;
        environment* parent;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Intrinsic functions
    ///////////////////////////////////////////////////////////////////////////
    struct arithmetic_function
    {
        typedef bool result_type;

        template <typename A, typename B>
        bool dispatch(const A&, const B&, boost::mpl::false_) const
        {
            return false; // cannot compare different types by default
        }

        template <typename A, typename B>
        bool dispatch(const A& a, const B& b, boost::mpl::true_) const
        {
            return a == b; // for arithmetic types
        }

        template <typename A, typename B>
        bool operator()(const A& a, const B& b) const
        {
            return dispatch(a, b,
                boost::mpl::and_<
                    boost::is_arithmetic<A>,
                    boost::is_arithmetic<B> >());
        }
    };

    utree plus(environment*, utree& args)
    {

    }
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    // Ignore the BOM marking the beginning of a UTF-8 file in Windows
    char c = in.peek();
    if (c == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "Error: Unexpected characters from input file: "
                << filename << std::endl;
            return 1;
        }
    }

    scheme::utree result;
    if (scheme::input::parse_sexpr(in, result))
    {
        std::cout << "success: ";
        println(std::cout, result);
        std::cout << std::endl;
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}


