/*=============================================================================
    Copyright (c) 1999-2003 Jeremiah Willcock
    Copyright (c) 1999-2003 Jaakko Järvi
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_SEQUENCE_DETAIL_MANIP_HPP
#define FUSION_SEQUENCE_DETAIL_MANIP_HPP

#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <vector>

// Tuple I/O manipulators

// Based partially on Matt Austern's article at
// http://www.cuj.com/experts/1902/austern.htm?topic=experts
// and Langer's and Kreft's book: Standard C++ IOStreams and Locales

#include <boost/spirit/fusion/detail/config.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
FUSION_MSVC_ETI_WRAPPER(char_type)
FUSION_MSVC_ETI_WRAPPER(traits_type)
# define FUSION_GET_CHAR_TYPE(T) get_char_type<T>::type
# define FUSION_GET_TRAITS_TYPE(T) get_traits_type<T>::type
#else
# define FUSION_GET_CHAR_TYPE(T) typename T::char_type
# define FUSION_GET_TRAITS_TYPE(T) typename T::traits_type
#endif

namespace boost { namespace fusion
{
    namespace detail
    {
        template <class Tag>
        struct string_ios_manip_helper
        {
            static int index()
            {
                static int index_ = std::ios_base::xalloc();
                return index_;
            }
        };

#if !BOOST_WORKAROUND(__BORLANDC__, <= 0x551)

        // The type of string that is stored can vary (wchar streams etc.)
        // The pword mechanism need to be able to allocate/clone/etc.
        // objects of some fixed type. Hence this virtual construction
        // wrapper.
        struct any_string
        {
            virtual ~any_string() {} // [JDG] Added 2003-09-09
            virtual any_string* clone() = 0;
        };

        template <class Str>
        class string_holder : public any_string
        {
            Str s;

        public:

            string_holder(const Str& s_) : s(s_) {}
            Str const& get() const { return s; }
            virtual any_string* clone() { return new string_holder(s); }
        };

        void
        string_manip_callback(
            std::ios_base::event ev
          , std::ios_base& b
          , int n)
        {
            any_string*  p = (any_string*) b.pword(n);

            if (ev == std::ios_base::erase_event)
            {
                // maybe the delete should be wrapped in a try_catch as well?
                delete p;
                b.pword(n) = 0;
            }
            else if (ev == std::ios_base::copyfmt_event && p != 0)
            {
                try
                {
                    b.pword(n) = p->clone();
                }
                catch(std::bad_alloc&)
                {
                    // Standard prohibits this callback function from
                    // throwing exceptions. Bad_alloc could occur, so it must
                    // be supressed.  We cannot directly set the stream state
                    // into bad, as the we only have access to ios_base, not
                    // the stream :( One way to indicate that error has
                    // occurred is to set the corresponding iword slot into
                    // some error value and examine that after cpyfmt
                    // call. That is not specified in the tr1 though and we
                    // just suppress the exception for now. It is a hack :(
                }
            }
        }

        template <class Tag, class Stream>
        class string_ios_manip
        {
            int index; Stream& stream;

        public:

            typedef std::basic_string<
                FUSION_GET_CHAR_TYPE(Stream)
              , FUSION_GET_TRAITS_TYPE(Stream)
            > string_type;

            typedef string_holder<string_type> string_holder;

            string_ios_manip(Stream& str_)
                : stream(str_)
            {
                index = string_ios_manip_helper<Tag>::index();
            }

            void
            set(const string_type& s)
            {
                int registered = stream.iword(index);
                // avoid registering the callback twice
                if (!registered)
                {
                    stream.iword(index) = 1;
                    stream.register_callback(&string_manip_callback, index);
                }
                any_string* p = (any_string*)(stream.pword(index));
                if (p) delete p;
                stream.pword(index) = (void*)(new string_holder(s));
            }

            template <typename Default>
            void
            print(Default const& default_) const
            {
                any_string* a = (any_string*)stream.pword(index);
                if (a)
                    stream << (static_cast<string_holder* >(a))->get();
                else
                    stream << default_;
            }
        };

#else // borland 5.5.1

        template <class Tag, class Stream>
        class string_ios_manip
        {
            int index; Stream& stream;

        public:

            typedef std::basic_string<
                FUSION_GET_CHAR_TYPE(Stream)
              , FUSION_GET_TRAITS_TYPE(Stream)
            > string_type;

            string_ios_manip(Stream& str_)
                : stream(str_)
            {
                index = string_ios_manip_helper<Tag>::index();
            }

            static string_type*
            new_string(string_type const& s)
            {
                // our arena of strings
                static std::vector<string_type> formats;
                formats.push_back(s);
                return &formats.back();
            }

            void
            set(string_type const& s)
            {
                stream.pword(index) = (void*)(new_string(s));
            }

            template <typename Default>
            void
            print(Default const& default_) const
            {
                string_type* p = (string_type*)stream.pword(index);
                if (p)
                    stream << *p;
                else
                    stream << default_;
            }
        };
#endif

    } // detail

#define STD_TUPLE_DEFINE_MANIPULATOR(name)                                      \
    namespace detail                                                            \
    {                                                                           \
        struct name##_tag;                                                      \
                                                                                \
        template <class Char, class Traits = std::char_traits<Char> >           \
        struct name##_type                                                      \
        {                                                                       \
            typedef std::basic_string<Char,Traits> string_type;                 \
            string_type data;                                                   \
            name##_type(const string_type& d): data(d) {}                       \
        };                                                                      \
                                                                                \
        template <class Stream, class Char, class Traits>                       \
        Stream& operator>>(Stream& s, const name##_type<Char,Traits>& m)        \
        {                                                                       \
            string_ios_manip<name##_tag, Stream>(s).set(m.data);                \
            return s;                                                           \
        }                                                                       \
                                                                                \
        template <class Stream, class Char, class Traits>                       \
        Stream& operator<<(Stream& s, const name##_type<Char,Traits>& m)        \
        {                                                                       \
            string_ios_manip<name##_tag, Stream>(s).set(m.data);                \
            return s;                                                           \
        }                                                                       \
    }                                                                           \
                                                                                \
    template <class Char, class Traits>                                         \
    detail::name##_type<Char,Traits>                                            \
    name(const std::basic_string<Char,Traits>& s)                               \
    {                                                                           \
        return detail::name##_type<Char,Traits>(s);                             \
    }                                                                           \
                                                                                \
    template <class Char>                                                       \
    detail::name##_type<Char>                                                   \
    name(const Char c[])                                                        \
    {                                                                           \
        return detail::name##_type<Char>(std::basic_string<Char>(c));           \
    }                                                                           \
                                                                                \
    template <class Char>                                                       \
    detail::name##_type<Char>                                                   \
    name(Char c)                                                                \
    {                                                                           \
        return detail::name##_type<Char>(std::basic_string<Char>(1,c));         \
    }

    STD_TUPLE_DEFINE_MANIPULATOR(tuple_open)
    STD_TUPLE_DEFINE_MANIPULATOR(tuple_close)
    STD_TUPLE_DEFINE_MANIPULATOR(tuple_delimiter)
    #undef STD_TUPLE_DEFINE_MANIPULATOR
}}

#endif
