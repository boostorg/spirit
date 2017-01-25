#ifndef NONIUS_X3_NS_HPP_INCLUDED_2017_01_22_1801
#define NONIUS_X3_NS_HPP_INCLUDED_2017_01_22_1801
//OriginalSouce:
//  The gist mentioned in post to spirit user's ml:
//    https://sourceforge.net/p/spirit/mailman/message/35549390/
//==============================
#include <iostream>
//#define BOOST_SPIRIT_X3_DEBUG
#include <boost/spirit/home/x3.hpp>

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

namespace csv_fixed_separator_no_semantic_actions {
    using namespace boost::spirit::x3;
    auto static const file = csv::make_csv_parser(ascii::lit(',')); // just making a point
}

#include <boost/preprocessor/stringize.hpp>
NONIUS_BENCHMARK(BOOST_PP_STRINGIZE(BENCH_NAME)"-x3_ns", [](nonius::chronometer cm) { 
    bench(csv::make_csv_parser(','), cm); 
});

#endif//NONIUS_X3_NS_HPP_INCLUDED_2017_01_22_1801
