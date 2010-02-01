/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/range/iterator_range.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

// We place the data here. Each line comprises various fields
typedef std::vector<std::string> ucd_line;
typedef std::vector<ucd_line> ucd_vector;
typedef std::vector<ucd_line>::iterator ucd_iterator;

// spirit and phoenix using declarations
using boost::spirit::qi::parse;
using boost::spirit::qi::hex;
using boost::spirit::qi::char_;
using boost::spirit::qi::eol;
using boost::spirit::qi::rule;
using boost::spirit::qi::omit;
using boost::spirit::qi::_1;
using boost::spirit::qi::_val;
using boost::phoenix::push_back;
using boost::phoenix::ref;

// basic unsigned types
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;

// a char range
struct ucd_range
{
    ucd_range(uint32_t start, uint32_t finish)
        : start(start), finish(finish) {}
            
    // we need this so we can use ucd_range as a multimap key 
    friend bool operator<(ucd_range const& a, ucd_range const& b)
    {
        return a.start < b.start;
    }
            
    uint32_t start;
    uint32_t finish;
};

class ucd_info
{
public:

    ucd_info(char const* filename)
    {
        std::ifstream in(filename, std::ios_base::in);
        if (!in)
        {
            std::cerr << "Error: Could not open input file: "
                << filename << std::endl;
        }
        else
        {
            std::string data;               // We will read the contents here.
            in.unsetf(std::ios::skipws);    // No white space skipping!
            std::copy(
                std::istream_iterator<char>(in),
                std::istream_iterator<char>(),
                std::back_inserter(data));

            typedef std::string::const_iterator iterator_type;
            iterator_type f = data.begin();
            iterator_type l = data.end();

            rule<iterator_type> endl = -('#' >> *(char_-eol)) >> eol;
            rule<iterator_type, std::string()> field = *(char_-(';'|endl)) >> (';'|&endl);
            rule<iterator_type, ucd_line()> line = +(field-endl) >> endl;
            rule<iterator_type, std::vector<ucd_line>()> file = +(endl | line[push_back(_val, _1)]);
            
            parse(f, l, file, info);
        }
    }
    
    template <typename Array>
    void collect(Array& data, int field) const
    {
        BOOST_ASSERT(!info.empty());
        ucd_vector::const_iterator current = info.begin();
        ucd_vector::const_iterator end = info.end();
        
        while (current != end)
        {
            std::string range = (*current)[0];
            boost::trim(range);
            
            std::string::const_iterator f = range.begin();
            std::string::const_iterator l = range.end();

            // get the code-point range
            uint32_t start;
            uint32_t finish;
            parse(f, l, hex[ref(start) = ref(finish) = _1] >> -(".." >> hex[ref(finish) = _1]));
            
            // special case for UnicodeData.txt ranges:
            if ((*current)[1].find("First>") != std::string::npos)
            {
                ++current;
                BOOST_ASSERT(current != end);
                BOOST_ASSERT((*current)[1].find("Last>") != std::string::npos);
                                
                std::string range = (*current)[0];
                boost::trim(range);
                f = range.begin();
                l = range.end();

                parse(f, l, hex[ref(finish) = _1]);
            }
            
            std::string code = (*current)[field];
            boost::trim(code);
            if (!ignore_property(code)) // Only collect properties we are interested in
            {
                for (uint32_t i = start; i <= finish; ++i)
                    data[i] |= map_property(code);
            }
            ++current;
        }
    }
    
private:

    static bool ignore_property(std::string const& p)
    {
        // We don't handle all properties
        std::map<std::string, int>& pm = get_property_map();
        std::map<std::string, int>::iterator i = pm.find(p);
        return i == pm.end();        
    }

    static int
    map_property(std::string const& p)
    {
        std::map<std::string, int>& pm = get_property_map();
        std::map<std::string, int>::iterator i = pm.find(p);
        BOOST_ASSERT(i != pm.end());
        return i->second;
    }

    static std::map<std::string, int>& 
    get_property_map()
    {
        // The properties we are interested in:
        static std::map<std::string, int> map;
        if (map.empty())
        {
            // General_Category
            map["Lu"] = 0;
            map["Ll"] = 1;
            map["Lt"] = 2;
            map["Lm"] = 3;
            map["Lo"] = 4;
            
            map["Mn"] = 8;
            map["Me"] = 9;
            map["Mc"] = 10;
            
            map["Nd"] = 16;
            map["Nl"] = 17;
            map["No"] = 18;
            
            map["Zs"] = 24;
            map["Zl"] = 25;
            map["Zp"] = 26;
            
            map["Cc"] = 27;
            map["Cf"] = 28;
            map["Co"] = 29;
            map["Cs"] = 30;
            map["Cn"] = 31;
            
            map["Pd"] = 40;
            map["Ps"] = 41;
            map["Pe"] = 42;
            map["Pc"] = 43;
            map["Po"] = 44;
            map["Pi"] = 45;
            map["Pf"] = 46;
            
            map["Sm"] = 48;
            map["Sc"] = 49;
            map["Sk"] = 50;
            map["So"] = 51;
            
            // Derived Properties.
            map["Alphabetic"] = 64;
            map["Uppercase"] = 128;
            map["Lowercase"] = 256;
            map["White_Space"] = 512;
            map["Noncharacter_Code_Point"] = 1024;
            map["Default_Ignorable_Code_Point"] = 2048;

            // Script
            map["Arabic"] = 0;
            map["Imperial_Aramaic"] = 1;
            map["Armenian"] = 2;
            map["Avestan"] = 3;
            map["Balinese"] = 4;
            map["Bamum"] = 5;
            map["Bengali"] = 6;
            map["Bopomofo"] = 7;
            map["Braille"] = 8;
            map["Buginese"] = 9;
            map["Buhid"] = 10;
            map["Canadian_Aboriginal"] = 11;
            map["Carian"] = 12;
            map["Cham"] = 13;
            map["Cherokee"] = 14;
            map["Coptic"] = 15;
            map["Cypriot"] = 16;
            map["Cyrillic"] = 17;
            map["Devanagari"] = 18;
            map["Deseret"] = 19;
            map["Egyptian_Hieroglyphs"] = 20;
            map["Ethiopic"] = 21;
            map["Georgian"] = 22;
            map["Glagolitic"] = 23;
            map["Gothic"] = 24;
            map["Greek"] = 25;
            map["Gujarati"] = 26;
            map["Gurmukhi"] = 27;
            map["Hangul"] = 28;
            map["Han"] = 29;
            map["Hanunoo"] = 30;
            map["Hebrew"] = 31;
            map["Hiragana"] = 32;
            map["Katakana_Or_Hiragana"] = 33;
            map["Old_Italic"] = 34;
            map["Javanese"] = 35;
            map["Kayah_Li"] = 36;
            map["Katakana"] = 37;
            map["Kharoshthi"] = 38;
            map["Khmer"] = 39;
            map["Kannada"] = 40;
            map["Kaithi"] = 41;
            map["Tai_Tham"] = 42;
            map["Lao"] = 43;
            map["Latin"] = 44;
            map["Lepcha"] = 45;
            map["Limbu"] = 46;
            map["Linear_B"] = 47;
            map["Lisu"] = 48;
            map["Lycian"] = 49;
            map["Lydian"] = 50;
            map["Malayalam"] = 51;
            map["Mongolian"] = 52;
            map["Meetei_Mayek"] = 53;
            map["Myanmar"] = 54;
            map["Nko"] = 55;
            map["Ogham"] = 56;
            map["Ol_Chiki"] = 57;
            map["Old_Turkic"] = 58;
            map["Oriya"] = 59;
            map["Osmanya"] = 60;
            map["Phags_Pa"] = 61;
            map["Inscriptional_Pahlavi"] = 62;
            map["Phoenician"] = 63;
            map["Inscriptional_Parthian"] = 64;
            map["Rejang"] = 65;
            map["Runic"] = 66;
            map["Samaritan"] = 67;
            map["Old_South_Arabian"] = 68;
            map["Saurashtra"] = 69;
            map["Shavian"] = 70;
            map["Sinhala"] = 71;
            map["Sundanese"] = 72;
            map["Syloti_Nagri"] = 73;
            map["Syriac"] = 74;
            map["Tagbanwa"] = 75;
            map["Tai_Le"] = 76;
            map["New_Tai_Lue"] = 77;
            map["Tamil"] = 78;
            map["Tai_Viet"] = 79;
            map["Telugu"] = 80;
            map["Tifinagh"] = 81;
            map["Tagalog"] = 82;
            map["Thaana"] = 83;
            map["Thai"] = 84;
            map["Tibetan"] = 85;
            map["Ugaritic"] = 86;
            map["Vai"] = 87;
            map["Old_Persian"] = 88;
            map["Cuneiform"] = 89;
            map["Yi"] = 90;
            map["Inherited"] = 91;
            map["Common"] = 92;
            map["Unknown"] = 93;
        }
        return map;
    }

    ucd_vector info;
};

template <typename T, uint32_t block_size = 256>
class ucd_table_builder
{
public:

    static uint32_t const full_span = 0x110000;

    ucd_table_builder() : p(new T[full_span])
    {
        for (uint32_t i = 0; i < full_span; ++i)
            p[i] = 0;
    }
    
    void collect(char const* filename, int field)
    {
        ucd_info info(filename);
        info.collect(p, field);
    }
    
    void build(std::vector<uint8_t>& stage1, std::vector<T const*>& stage2)
    {        
        std::map<block_ptr, std::vector<T const*> > blocks;
        for (T const* i = p.get(); i < (p.get() + full_span); i += block_size)
            blocks[block_ptr(i)].push_back(i);
        
        // Not enough bits to store the block indices.
        BOOST_ASSERT(blocks.size() < (1 << (sizeof(uint8_t) * 8)));
        
        typedef std::pair<block_ptr, std::vector<T const*> > blocks_value_type;
        std::map<T const*, std::vector<T const*> > sorted_blocks;
        BOOST_FOREACH(blocks_value_type const& val, blocks)
        {
            sorted_blocks[val.first.p] = val.second;
        }

        stage1.clear();
        stage1.reserve(full_span / block_size);
        stage1.resize(full_span / block_size);
        stage2.clear();
        stage2.reserve(blocks.size());

        typedef std::pair<T const*, std::vector<T const*> > sorted_blocks_value_type;
        BOOST_FOREACH(sorted_blocks_value_type const& val, sorted_blocks)
        {
            stage2.push_back(val.first);
            BOOST_FOREACH(T const* val2, val.second)
            {
                stage1[(val2 - p.get()) / block_size] = stage2.size() - 1;
            }
        }
    }
        
private:
    
    struct block_ptr
    {
        block_ptr(T const* p) : p(p) {}

        friend bool operator<(block_ptr a, block_ptr b)
        {
            return std::lexicographical_compare(
                a.p, a.p + block_size, b.p, b.p + block_size);
        }
        
        T const* p;
    };

    boost::scoped_array<T> p;
};

template <typename Out>
void print_tab(Out& out, int tab)
{
    for (int i = 0; i < tab; ++i)
        out << ' ';
}

template <typename Out, typename C>
void print_table(Out& out, C const& c, int tab, int width = 4, int group = 16)
{
    C::size_type size = c.size();
    BOOST_ASSERT(size > 1);
    print_tab(out, tab);
    out << std::setw(width) << int(c[0]);
    for (C::size_type i = 1; i < size; ++i)
    {
        out << ", ";
        if ((i % group) == 0)
        {
            out << std::endl;
            print_tab(out, tab);
        }
        out << std::setw(width) << int(c[i]);
    }
}

int main()
{
    std::cout 
        << "/*=============================================================================\n"
        << "    Copyright (c) 2001-2010 Joel de Guzman\n"
        << "\n"
        << "    Distributed under the Boost Software License, Version 1.0. (See accompanying\n"
        << "    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)\n"
        << "\n"
        << "    AUTOGENERATED. DO NOT EDIT!!!\n"
        << "==============================================================================*/\n"
        << "\n"
        << "namespace boost { namespace spirit { namespace unicode { namespace detail\n"
        << "{\n"
        ;

    const int block_size = 256;
    
    // The category tables
    {
        ucd_table_builder<uint16_t, block_size> builder;
        builder.collect("UnicodeData.txt", 2);
        builder.collect("DerivedCoreProperties.txt", 1);
        builder.collect("PropList.txt", 1);
        
        std::vector<uint8_t> stage1;
        std::vector<uint16_t const*> stage2;
        builder.build(stage1, stage2);

        std::cout
            << "    static const ::boost::uint8_t category_stage1[] = {\n"
            << "\n"
            ;
        
        print_table(std::cout, stage1, 4, 3);

        std::cout 
            << "\n"
            << "    };"
            << "\n"
            << "\n"
            << "    static const ::boost::uint8_t category_stage2[] = {"
            ;

        int block_n = 0;
        BOOST_FOREACH(uint16_t const* p, stage2)
        {
            std::cout << "\n\n    // block " << block_n++ << std::endl;
            print_table(std::cout, 
                boost::iterator_range<uint16_t const*>(p, p+block_size), 4, 4);
        }
        
        std::cout 
            << "\n"
            << "    };"
            << "\n"
            ;
    }
    
    // The script tables
    {
        ucd_table_builder<uint8_t, block_size> builder;
        builder.collect("Scripts.txt", 1);
        
        std::vector<uint8_t> stage1;
        std::vector<uint8_t const*> stage2;
        builder.build(stage1, stage2);

        std::cout
            << "\n"
            << "    static const ::boost::uint8_t script_stage1[] = {\n"
            << "\n"
            ;
        
        print_table(std::cout, stage1, 4, 3);

        std::cout 
            << "\n"
            << "    };"
            << "\n"
            << "\n"
            << "    static const ::boost::uint8_t script_stage2[] = {"
            ;

        int block_n = 0;
        BOOST_FOREACH(uint8_t const* p, stage2)
        {
            std::cout << "\n\n    // block " << block_n++ << std::endl;
            print_table(std::cout, 
                boost::iterator_range<uint8_t const*>(p, p+block_size), 4, 3);
        }
        
        std::cout 
            << "\n"
            << "    };"
            << "\n"
            ;
    }

    std::cout 
        << "\n"
        << "}}}} // namespace boost::spirit::unicode::detail\n"
        ;

    return 0;
}
