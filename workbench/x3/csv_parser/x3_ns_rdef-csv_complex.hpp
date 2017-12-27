#ifndef X3_NS_RDEF_CSV_HPP_INCLUDED_2017_01_22_1801
#define X3_NS_RDEF_CSV_HPP_INCLUDED_2017_01_22_1801
//OriginalSouce:
//  The gist mentioned in post to spirit user's ml:
//    https://sourceforge.net/p/spirit/mailman/message/35549390/
//Method:
//  Just use rule_definition::parse functions.
//==============================
#include <iostream>
//#define BOOST_SPIRIT_X3_DEBUG
#include <boost/spirit/home/x3.hpp>
#include "csv_attributes.hpp"

namespace csv {
    template <typename Sep>
    auto make_csv_parser(Sep fieldSep) {
        using namespace boost::spirit::x3;
        auto lineBreak = eol;
        using R = rule<struct _, CsvString>;

        auto unescaped = R{"raw"}    = *(~char_("\"\r\n") - fieldSep);
        auto escaped   = R{"escaped"} = *(~char_('"') | '"' >> char_('"'));
        auto quoted   = R{"quoted"} = '"' > escaped > '"';

        auto field = R{"field"} =
            (*lit(' ') >> quoted >> *lit(' '))
            | unescaped; // the order is important because unescaped can consume zero characters

        auto record = rule<struct _, CsvRecordInformation>{ "record" } =
            field % fieldSep;

        auto file   = rule<struct _, CsvFileInformation>{ "file" } =
            ((!eoi > record) % lineBreak) > -lineBreak > eoi;

        return file;
    };
}

auto bench_parser=csv::make_csv_parser(',');

#endif//NONIUS_X3_NS_HPP_INCLUDED_2017_01_22_1801
