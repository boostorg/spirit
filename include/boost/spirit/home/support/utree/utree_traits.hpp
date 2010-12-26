/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2010-2011 Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM)
#define BOOST_SPIRIT_OUTPUT_UTREE_TRAITS_APR_16_2010_0655AM

#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/utree.hpp>
#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/qi/nonterminal/nonterminal_fwd.hpp>
#include <boost/spirit/home/karma/nonterminal/nonterminal_fwd.hpp>

#include <string>

#include <boost/cstdint.hpp>
#include <boost/variant.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_same.hpp>
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
    struct assign_to_container_from_value<
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
    struct assign_to_container_from_value<utree, Attribute>
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
            typedef typename traits::container_iterator<Attribute const>::type 
                iterator_type;

            // make sure the attribute is a list, at least an empty one
            if (attr.which() != utree_type::list_type)
                attr = utree::list;

            iterator_type end = traits::end(val);
            for (iterator_type i = traits::begin(val); i != end; traits::next(i))
                push_back(attr, traits::deref(i));
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
    struct assign_to_container_from_value<utree, utree>
    {
        static void call(utree const& val, utree& attr)
        {
            if (val.which() != utree_type::list_type)
                attr = val;
            else {
                typedef utree::const_iterator iterator_type;

                // make sure the attribute is a list, at least an empty one
                if (attr.which() != utree_type::list_type)
                    attr = utree::list;

                iterator_type end = val.end();
                for (iterator_type i = val.begin(); i != end; ++i)
                    attr.push_back(*i);
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    template <>
    struct transform_attribute<utree, utree, qi::domain>
    {
        typedef utree& type;

        static utree& pre(utree& val) { return val; }

        // If the rhs' attribute is already a list, push it back into the 
        // rule's attribute, otherwise wrap it into a list and push that back.
        // This ensures that the rule always exposes an utree list node.
        static void post(utree& attr, utree const& val)
        {
            if (val.which() != utree_type::list_type) {
                utree ut (utree::list);
                ut.push_back(val);
                attr.swap(ut);
            }
        }

        // fail() will be called by Qi rule's if the rhs failed parsing
        static void fail(utree&) {}
    };

    template <>
    struct transform_attribute<utree&, utree, qi::domain>
      : transform_attribute<utree, utree, qi::domain>
    {};

    ///////////////////////////////////////////////////////////////////////////
    // this specialization makes sure strings get assigned as a whole and are 
    // not converted into a utree list
    template <>
    struct assign_to_container_from_value<utree, utf8_string_type>
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
    struct assign_to_container_from_value<utree, utf8_symbol_type> 
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
    struct assign_to_container_from_value<utree, utf8_symbol_range_type> 
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
    struct assign_to_container_from_value<utree, std::string>
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
    // this specialization tells Spirit.Qi to allow assignment to an utree from
    // generic iterators
    template <typename Iterator>
    struct assign_to_attribute_from_iterators<utree, Iterator>
    {
        static void
        call(Iterator const& first, Iterator const& last, utree& attr)
        {
            if (attr.empty())
                attr.assign(first, last);
            else {
                for (Iterator i = first; i != last; ++i)
                    push_back(attr, *i);
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Karma only: convert utree node to string
    namespace detail
    {
        struct attribute_as_string_type
        {
            typedef utf8_string_range_type type; 

            static type call(utree const& attr)
            {
                return boost::get<utf8_string_range_type>(attr);
            }
        
            static bool is_valid(utree const& attr)
            {
                switch (attr.which())
                {
                case utree_type::reference_type:
                    return is_valid(attr.deref());

                case utree_type::string_range_type:
                case utree_type::string_type:
                    return true;

                default:
                    return false;
                }
            }
        };
    }
    
    template <>    
    struct attribute_as_xxx<std::string, utree>
      : detail::attribute_as_string_type 
    {};

    template <>    
    struct attribute_as_xxx<utf8_string_type, utree>
      : detail::attribute_as_string_type 
    {};

    template <>    
    struct attribute_as_xxx<utf8_string_range_type, utree>
      : detail::attribute_as_string_type 
    {};

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        struct attribute_as_symbol_type
        {
            typedef utf8_symbol_range_type type; 

            static type call(utree const& attr)
            {
                return boost::get<utf8_symbol_range_type>(attr);
            }
        
            static bool is_valid(utree const& attr)
            {
                switch (attr.which())
                {
                case utree_type::reference_type:
                    return is_valid(attr.deref());

                case utree_type::symbol_type:
                    return true;

                default:
                    return false;
                }
            }
        };
    }

    template <>
    struct attribute_as_xxx<utf8_symbol_type, utree>
      : detail::attribute_as_symbol_type 
    {};

    template <>
    struct attribute_as_xxx<utf8_symbol_range_type, utree>
      : detail::attribute_as_symbol_type 
    {};

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        struct attribute_as_binary_string_type
        {
            typedef binary_range_type type; 

            static type call(utree const& attr)
            {
                return boost::get<binary_range_type>(attr);
            }

            static bool is_valid(utree const& attr)
            {
                switch (attr.which())
                {
                case utree_type::reference_type:
                    return is_valid(attr.deref());

                case utree_type::binary_type:
                    return true;

                default:
                    return false;
                }
            }
        };
    }

    template <>
    struct attribute_as_xxx<binary_string_type, utree>
      : detail::attribute_as_binary_string_type 
    {};

    template <>
    struct attribute_as_xxx<binary_range_type, utree>
      : detail::attribute_as_binary_string_type 
    {};

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
                    c.push_back(val);
                    break;

                default:
                {
                    utree ut;
                    ut.push_back(c);
                    ut.push_back(val);
                    c.swap(ut);
                }
                break;
            }
            return true;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // an utree is a container on its own
    template <>
    struct build_std_vector<utree>
    {
        typedef utree type;
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
    // or a grammar exposes an utree as it's attribute
    namespace detail
    {
        template <typename Attribute>
        struct attribute_is_not_utree
          : mpl::and_<
                mpl::not_<is_same<utree, Attribute> >,
                traits::is_container<Attribute> >
        {};
    }

    template <
        typename Iterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<qi::rule<Iterator, T1, T2, T3, T4>, utree>
      : detail::attribute_is_not_utree<typename attribute_of<
            qi::rule<Iterator, T1, T2, T3, T4> 
        >::type>
    {};

    template <
        typename Iterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<qi::grammar<Iterator, T1, T2, T3, T4>, utree>
      : detail::attribute_is_not_utree<typename attribute_of<
            qi::grammar<Iterator, T1, T2, T3, T4> 
        >::type>
    {};

    template <
        typename OutputIterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<
            karma::rule<OutputIterator, T1, T2, T3, T4>, utree>
      : detail::attribute_is_not_utree<typename attribute_of<
            karma::rule<OutputIterator, T1, T2, T3, T4> 
        >::type>
    {};

    template <
        typename OutputIterator, typename T1, typename T2, typename T3
      , typename T4>
    struct handles_container<
            karma::grammar<OutputIterator, T1, T2, T3, T4>, utree>
      : detail::attribute_is_not_utree<typename attribute_of<
            karma::grammar<OutputIterator, T1, T2, T3, T4> 
        >::type>
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

    template <>
    struct build_optional<utree>
    {
        typedef utree type;
    };

    // an utree is an optional (in any domain)
    template <>
    struct not_is_optional<utree, qi::domain>
      : mpl::false_
    {};

    template <>
    struct not_is_optional<utree, karma::domain>
      : mpl::false_
    {};

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
    struct compute_compatible_component_variant<utree, utree::uninitialized_type>
      : mpl::true_
    {
        typedef utree::uninitialized_type compatible_type;

        static bool is_compatible(int d)
        {
            return d == utree_type::uninitialized_type;
        }
    };

    template <>
    struct compute_compatible_component_variant<utree, utree::nil_type>
      : mpl::true_
    {
        typedef utree::nil_type compatible_type;

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
    namespace detail
    {
        template <typename T>
        inline T get_or_deref(utree const& t)
        {
            if (t.which() == utree_type::list_type)
                return boost::get<T>(t.front());
            return boost::get<T>(t);
        }
    }

    template <>
    struct extract_from_attribute<utree, utree::nil_type>
    {
        typedef utree::nil_type type;

        template <typename Context>
        static type call(utree const&, Context&)
        {
            return utree::nil;
        }
    };

    template <>
    struct extract_from_attribute<utree, char>
    {
        typedef char type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_symbol_range_type r = detail::get_or_deref<utf8_symbol_range_type>(t);
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
            return detail::get_or_deref<bool>(t);
        }
    };

    template <>
    struct extract_from_attribute<utree, int>
    {
        typedef int type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            return detail::get_or_deref<int>(t);
        }
    };

    template <>
    struct extract_from_attribute<utree, double>
    {
        typedef double type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            return detail::get_or_deref<double>(t);
        }
    };

    template <typename Traits, typename Alloc>
    struct extract_from_attribute<utree, std::basic_string<char, Traits, Alloc> >
    {
        typedef std::basic_string<char, Traits, Alloc> type;

        template <typename Context>
        static type call(utree const& t, Context&)
        {
            utf8_string_range_type r = detail::get_or_deref<utf8_string_range_type>(t);
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
            utf8_symbol_range_type r = detail::get_or_deref<utf8_symbol_range_type>(t);
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
            utf8_string_range_type r = detail::get_or_deref<utf8_string_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct transform_attribute<utree const, utree::nil_type, karma::domain>
    {
        typedef utree::nil_type type;

        static type pre(utree const& t)
        {
            return utree::nil;
        }
    };

    template <>
    struct transform_attribute<utree const, char, karma::domain>
    {
        typedef char type;

        static type pre(utree const& t)
        {
            utf8_string_range_type r = detail::get_or_deref<utf8_string_range_type>(t);
            return r.front();
        }
    };

    template <>
    struct transform_attribute<utree const, bool, karma::domain>
    {
        typedef bool type;

        static type pre(utree const& t)
        {
            return detail::get_or_deref<bool>(t);
        }
    };

    template <>
    struct transform_attribute<utree const, int, karma::domain>
    {
        typedef int type;

        static type pre(utree const& t)
        {
            return detail::get_or_deref<int>(t);
        }
    };

    template <>
    struct transform_attribute<utree const, double, karma::domain>
    {
        typedef double type;

        static type pre(utree const& t)
        {
            return detail::get_or_deref<double>(t);
        }
    };

    template <typename Traits, typename Alloc>
    struct transform_attribute<
        utree const, std::basic_string<char, Traits, Alloc>, karma::domain>
    {
        typedef std::basic_string<char, Traits, Alloc> type;

        static type pre(utree const& t)
        {
            utf8_string_range_type r = detail::get_or_deref<utf8_string_range_type>(t);
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
            utf8_string_range_type r = detail::get_or_deref<utf8_string_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    template <>
    struct transform_attribute<utree const, utf8_symbol_type, karma::domain>
    {
        typedef std::string type;

        static type pre(utree const& t)
        {
            utf8_symbol_range_type r = detail::get_or_deref<utf8_symbol_range_type>(t);
            return std::string(r.begin(), r.end());
        }
    };

    // If a rule takes an utree attribute and that utree instance holds nothing
    // more than a list, we dereference this to simplify attribute handling 
    // down the stream, i.e. ( ( 1 2 3 ) ) --> ( 1 2 3 ).
    template <>
    struct transform_attribute<utree const, utree, karma::domain>
    {
        typedef utree const& type;
        static utree const& pre(utree const& val) 
        { 
            if (val.which() == utree_type::list_type && 1 == val.size())
                return val.front();
            return val; 
        }
    };

    template <>
    struct transform_attribute<utree const&, utree, karma::domain>
      : transform_attribute<utree const, utree, karma::domain>
    {};
}}}

#endif
