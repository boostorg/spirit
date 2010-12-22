/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM)
#define BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM

#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/utree.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/nonterminal/nonterminal_fwd.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/variant.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/utility/enable_if.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost
{
    template <typename T>
    inline T get(boost::spirit::utree const& x)
    {
        return x.get<T>();
    }
}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // this specialization lets Spirit know that typed basic_strings
    // are strings
    template <typename Base, utree_type::info I>
    struct is_string<spirit::basic_string<Base, I> > 
      : mpl::true_ 
    {};

    ///////////////////////////////////////////////////////////////////////////
    // these specializations extract the character type of a utree typed string 
    template <typename T, utree_type::info I>
    struct char_type_of<spirit::basic_string<iterator_range<T>, I> >
      : char_type_of<T> 
    {};

    template <utree_type::info I>
    struct char_type_of<spirit::basic_string<std::string, I> > 
      : mpl::identity<char>
    {};

    ///////////////////////////////////////////////////////////////////////////
    // these specializations extract a c string from a utree typed string
    template <typename String>
    struct get_c_string;

    template <typename T, utree_type::info I>
    struct get_c_string<spirit::basic_string<iterator_range<T const*>, I> >
    {
        typedef T char_type;

        typedef spirit::basic_string<iterator_range<T const*>, I> string;

        static T const* call (string& s)
        {
            return s.begin();
        }

        static T const* call (string const& s)
        {
            return s.begin();
        }
    };
    
    template <utree_type::info I>
    struct get_c_string<spirit::basic_string<std::string, I> >
    {
        typedef char char_type;

        typedef spirit::basic_string<std::string, I> string;

        static char const* call (string& s)
        {
            return s.c_str();
        }

        static char const* call (string const& s)
        {
            return s.c_str();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit.Qi to allow assignment to an utree from
    // a variant
    namespace detail
    {
        struct assign_to_utree_visitor : static_visitor<>
        {
            assign_to_utree_visitor(utree& ut) : ut_(ut) {}

            template <typename T>
            void operator()(T& val) const
            {
                ut_ = val;
            }

            utree& ut_;
        };
    }

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct assign_to_attribute_from_value<
        utree, variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
    {
        static void
        call(variant<BOOST_VARIANT_ENUM_PARAMS(T)> const& val, utree& attr)
        {
            apply_visitor(detail::assign_to_utree_visitor(attr), val);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit.Qi to allow assignment to an utree from
    // a STL container
    template <typename Attribute>
    struct assign_to_attribute_from_value<utree, Attribute>
    {
        // any non-container type will be either directly assigned or appended
        static void call(Attribute const& val, utree& attr, mpl::false_)
        {
            if (attr.empty())
                attr = val;
            else
                push_back(attr, val);
        }

        // any container type will be converted into a list_type utree
        static void call(Attribute const& val, utree& attr, mpl::true_)
        {
            attr = make_iterator_range(traits::begin(val), traits::end(val));
        }
        
        static void call(Attribute const& val, utree& attr)
        {
            call(val, attr, is_container<Attribute>());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization is required to disambiguate the specializations
    // related to utree
    template <>
    struct assign_to_attribute_from_value<utree, utree>
    {
        static void call(utree const& val, utree& attr)
        {
            attr = val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization makes sure strings get assigned as a whole and are 
    // not converted into a utree list
    template <>
    struct assign_to_attribute_from_value<utree, utf8_string_type>
    {
        static void call(utf8_string_type const& val, utree& attr)
        {
            if (attr.empty())
                attr = val;
            else
                push_back(attr, val);
        }
    };

    // this specialization keeps symbols from being transformed into strings  
    template<>
    struct assign_to_attribute_from_value<utree, utf8_symbol_type> 
    {
        static void call (utf8_symbol_type const& val, utree& attr) 
        {
            if (attr.empty())
                attr = val;
            else
                push_back(attr, val);
        }
    };

    template<>
    struct assign_to_attribute_from_value<utree, utf8_symbol_range_type> 
    {
        static void call (utf8_symbol_range_type const& val, utree& attr) 
        {
            if (attr.empty())
                attr = val;
            else
                push_back(attr, val);
        }
    };

    template <>
    struct assign_to_attribute_from_value<utree, std::string>
    {
        static void call(std::string const& val, utree& attr)
        {
            if (attr.empty())
                attr = val;
            else
                push_back(attr, val);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // this specialization allows the use of utree as the attribute for single 
    // character parsers
    // FIXME: should we leave that in?
    template<utree_type::info I>
    struct assign_to_attribute_from_value<
        spirit::basic_string<std::string, I>, char>
    {
        typedef spirit::basic_string<std::string, I> attribute_type;

        static void call (char val, attribute_type& attr)
        {
            attr.assign(1, val);
        }
    }; 

    ///////////////////////////////////////////////////////////////////////////
    // this specialization tells Spirit.Qi to allow assignment to an utree from
    // generic iterators
    template <typename Iterator>
    struct assign_to_attribute_from_iterators<utree, Iterator>
    {
        static void
        call(Iterator const& first, Iterator const& last, utree& attr)
        {
            attr.assign(first, last);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Karma only: convert utree node to string
    template <>
    struct attribute_as_string<utree>
    {
        typedef utf8_string_range_type type; 

        static type call(utree const& attr)
        {
            return boost::get<utf8_string_range_type>(attr);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // push_back support for utree 
    template <typename T>
    struct push_back_container<utree, T>
    {
        static bool call(utree& c, T const& val)
        {
            switch (c.which())
            {
                case utree_type::uninitialized_type:
                case utree_type::nil_type:
                case utree_type::list_type:
                    {
                        c.push_back(val);
                        break;
                    }
                default:
                    {
                        utree ut;
                        ut.push_back(c);
                        ut.push_back(val);
                        c.swap(ut);
                    }
            }
            return true;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // debug support for utree
    template <typename Out>
    struct print_attribute_debug<Out, utree>
    {
        static void call(Out& out, utree const& val)
        {
            out << val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // force utree list attribute in a sequence to be dereferenced if a rule
    // or a grammar takes an utree
    template <
        typename OutputIterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<
            karma::rule<OutputIterator, T1, T2, T3, T4>, utree>
      : mpl::false_
    {};

    template <
        typename OutputIterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<
            karma::grammar<OutputIterator, T1, T2, T3, T4>, utree>
      : mpl::false_
    {};

    ///////////////////////////////////////////////////////////////////////////
    // the specialization below tells Spirit how to handle utree if it is used
    // with an optional component
    template <>
    struct optional_attribute<utree>
    {
        typedef utree const& type;

        static type call(utree const& val)
        {
            return val;
        }

        // only 'uninitialized_type' utree nodes are not valid
        static bool is_valid(utree const& val)
        {
            return val.which() != utree_type::uninitialized_type;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // the specialization below tells Spirit to handle utree as if it
    // where a 'real' variant (in the context of karma)
    template <>
    struct not_is_variant<utree, karma::domain>
      : mpl::false_ 
    {};

    // this specialization tells Spirit how to extract the type of the value
    // stored in the given utree node
    template <>
    struct variant_which<utree>
    {
        static int call(utree const& u) { return u.which(); }
    };

    // The specializations below tell Spirit to verify whether an attribute
    // type is compatible with a given variant type
    template <>
    struct compute_compatible_component_variant<
            utree, iterator_range<utree::iterator> >
      : mpl::true_
    {
        typedef iterator_range<utree::iterator> compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::list_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, iterator_range<utree::const_iterator> >
      : mpl::true_
    {
        typedef iterator_range<utree::const_iterator> compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::list_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, uninitialized_type>
      : mpl::true_
    {
        typedef uninitialized_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::uninitialized_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, nil_type>
      : mpl::true_
    {
        typedef nil_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::nil_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, bool>
      : mpl::true_
    {
        typedef bool compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::bool_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, int>
      : mpl::true_
    {
        typedef int compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::int_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, double>
      : mpl::true_
    {
        typedef double compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::double_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_string_range_type>
      : mpl::true_
    {
        typedef utf8_string_range_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::string_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_string_type>
      : mpl::true_
    {
        typedef utf8_string_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::string_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_symbol_range_type>
      : mpl::true_
    {
        typedef utf8_symbol_range_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::symbol_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, utf8_symbol_type>
      : mpl::true_
    {
        typedef utf8_symbol_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::symbol_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, binary_range_type>
      : mpl::true_
    {
        typedef binary_range_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::binary_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, binary_string_type>
      : mpl::true_
    {
        typedef binary_string_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::binary_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, utree>
      : mpl::true_
    {
        typedef utree compatible_type;

        static bool is_compatible(int d)
        {
            return d >= utree_type::uninitialized_type &&
                   d <= utree_type::reference_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<
            utree, std::vector<utree> >
      : mpl::true_
    {
        typedef utree compatible_type;

        static bool is_compatible(int d)
        {
            return d >= utree_type::uninitialized_type &&
                   d <= utree_type::reference_type;
        }
    };

    template <typename Sequence>
    struct compute_compatible_component_variant<utree, Sequence
          , mpl::false_
          , typename enable_if<fusion::traits::is_sequence<Sequence> >::type>
      : mpl::true_
    {
        typedef iterator_range<utree::const_iterator> compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::list_type;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct symbols_lookup<utree, utf8_symbol_type>
    {
        typedef std::string type;

        static type call(utree const& t)
        {
            utf8_symbol_range_type r = boost::get<utf8_symbol_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct symbols_lookup<utf8_symbol_type, utf8_symbol_type>
    {
        typedef std::string type;

        static type call(utf8_symbol_type const& t)
        {
            return t;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct extract_from_attribute<utree, spirit::nil_type>
    {
        typedef spirit::nil_type type;

        template <typename Context>
        static type call(utree const&, Context&)
        {
            return spirit::nil;
        }
    };

    template <>
    struct extract_from_attribute<utree, char>
    {
        typedef char type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_symbol_range_type r = boost::get<utf8_symbol_range_type>(t);
            return r.front();
        }
    };

    template <>
    struct extract_from_attribute<utree, bool>
    {
        typedef bool type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            return boost::get<bool>(t);
        }
    };

    template <>
    struct extract_from_attribute<utree, int>
    {
        typedef int type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            return boost::get<int>(t);
        }
    };

    template <>
    struct extract_from_attribute<utree, double>
    {
        typedef double type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            return boost::get<double>(t);
        }
    };

    template <typename Traits, typename Alloc>
    struct extract_from_attribute<utree, std::basic_string<char, Traits, Alloc> >
    {
        typedef std::basic_string<char, Traits, Alloc> type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_string_range_type r = boost::get<utf8_string_range_type>(t);
            return std::basic_string<char, Traits, Alloc>(r.begin(), r.end());
        }
    };

    template <>
    struct extract_from_attribute<utree, utf8_symbol_type>
    {
        typedef std::string type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_symbol_range_type r = boost::get<utf8_symbol_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct extract_from_attribute<utree, utf8_string_type>
    {
        typedef std::string type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_string_range_type r = boost::get<utf8_string_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<utree const, spirit::nil_type, karma::domain>
    {
        typedef spirit::nil_type type;

        static type pre(utree const& t)
        {
            return spirit::nil;
        }
    };

    template <>
    struct transform_attribute<utree const, char, karma::domain>
    {
        typedef char type;

        static type pre(utree const& t)
        {
            utf8_string_range_type r = boost::get<utf8_string_range_type>(t);
            return r.front();
        }
    };

    template <>
    struct transform_attribute<utree const, bool, karma::domain>
    {
        typedef bool type;

        static type pre(utree const& t)
        {
            return boost::get<bool>(t);
        }
    };

    template <>
    struct transform_attribute<utree const, int, karma::domain>
    {
        typedef int type;

        static type pre(utree const& t)
        {
            return boost::get<int>(t);
        }
    };

    template <>
    struct transform_attribute<utree const, double, karma::domain>
    {
        typedef double type;

        static type pre(utree const& t)
        {
            return boost::get<double>(t);
        }
    };

    template <typename Traits, typename Alloc>
    struct transform_attribute<
        utree const, std::basic_string<char, Traits, Alloc>, karma::domain>
    {
        typedef std::basic_string<char, Traits, Alloc> type;

        static type pre(utree const& t)
        {
            utf8_string_range_type r = boost::get<utf8_string_range_type>(t);
            return std::basic_string<char, Traits, Alloc>(r.begin(), r.end());
        }
    };

    // this specialization is used whenever a utree is passed to a rule as part
    // of a sequence
    template <typename Iterator>
    struct transform_attribute<
        iterator_range<Iterator> const, utree, karma::domain>
    {
        typedef utree type;

        static type pre(iterator_range<Iterator> const& t)
        {
            // return utree the begin iterator points to
            return utree(boost::ref(t.front()));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<utree const, utf8_string_type, karma::domain>
    {
        typedef std::string type;

        static type pre(utree const& t)
        {
            utf8_string_range_type r = boost::get<utf8_string_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct transform_attribute<utree const, utf8_symbol_type, karma::domain>
    {
        typedef std::string type;

        static type pre(utree const& t)
        {
            utf8_symbol_range_type r = boost::get<utf8_symbol_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };
}}}

#endif
