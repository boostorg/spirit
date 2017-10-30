#ifndef QI_CSV_COMPLEX_HPP_INCLUDED_2017_01_22_1801
#define QI_CSV_COMPLEX_HPP_INCLUDED_2017_01_22_1801
//OriginalSouce:
//  The gist mentioned in post to spirit user's ml:
//    https://sourceforge.net/p/spirit/mailman/message/35549390/
//==============================

#include <iostream>
#include <list>

#include <boost/spirit/include/qi.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception_ptr.hpp>
#include "csv_attributes.hpp"

template <typename It>
class CsvParser : public boost::spirit::qi::grammar<It, CsvFileInformation()> {
  public:
    CsvParser(const std::string &m_fieldSep) : CsvParser::base_type(m_file, "file") {
        using namespace boost::spirit::qi;
        m_unescaped = *(~char_("\"\r\n") - m_fieldSep);
        m_escaped   = *('"' >> char_('"') | ~char_('"'));

        m_quoted    = '"' > m_escaped > '"';
        m_field     = (*lit(' ') >> m_quoted >> *lit(' ')) // the order is important
                    | m_unescaped; // because unescaped ID can consume zero characters

        m_record    = m_field % m_fieldSep;

        m_file      = ((!eoi > m_record) % m_lineBreak) > -m_lineBreak > eoi;

        // error handling removed
    }

  private:
    boost::spirit::qi::eol_type m_lineBreak;
    boost::spirit::qi::rule<It, CsvString()> m_unescaped, m_escaped, m_quoted, m_field;
    boost::spirit::qi::rule<It, CsvRecordInformation()> m_record;
    boost::spirit::qi::rule<It, CsvFileInformation()> m_file;
};

const CsvParser<std::string::const_iterator> bench_parser(",");

#endif//QI_CSV_COMPLEX_HPP_INCLUDED_2017_01_22_1801
