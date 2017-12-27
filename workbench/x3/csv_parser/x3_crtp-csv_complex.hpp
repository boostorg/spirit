#ifndef X3_CRTP_CSV_HPP_INCLUDED_2017_01_22_1801
#define X3_CRTP_CSV_HPP_INCLUDED_2017_01_22_1801
//OriginalSouce:
//  The gist mentioned in post to spirit user's ml:
//    https://sourceforge.net/p/spirit/mailman/message/35549390/
//ModifiedBy:
//  Encapsulating grammar in struct, more like a qi grammar.
//==============================
#include <iostream>
//#define BOOST_SPIRIT_X3_DEBUG
#include <boost/spirit/home/x3.hpp>
#include "csv_attributes.hpp"
namespace x3_crtp {
using namespace boost::spirit::x3;
enum rule_enum
{ e_unescaped
, e_escaped
, e_quoted
, e_field
, e_record
, e_file
};
template<rule_enum ID>
struct id{};

class CsvParser : public gram_base<CsvParser> 
{
  public:
    CsvParser(char a_fieldSep=',') 
    : m_fieldSep(a_fieldSep)
    , m_lineBreak(eol)
    {
    }
    
  template<rule_enum ID, typename Attr>
  using R=rule_b<id<ID>,Attr>;

  auto start()const{ return m_file;}
  private:
    const decltype(ascii::lit(',')) m_fieldSep;
    decltype(eol) m_lineBreak;
    R<e_unescaped, CsvString> m_unescaped;
    R<e_escaped, CsvString> m_escaped;
    R<e_quoted, CsvString> m_quoted;
    R<e_field, CsvString> m_field;
    R<e_record, CsvRecordInformation> m_record;
    R<e_file, CsvFileInformation> m_file;
  public:  
  BOOST_SPIRIT_DER_DEFINE
    ( ( m_unescaped = *(~char_("\"\r\n") - m_fieldSep)
      )
    , ( m_escaped   = *(~char_('"') | '"' >> char_('"'))
      )
    , ( m_quoted    = '"' > m_escaped > '"'
      )
    , ( m_field     = (*lit(' ') >> m_quoted >> *lit(' ')) // the order is important
                    | m_unescaped // because unescaped ID can consume zero characters
      )
    , ( m_record    = m_field % m_fieldSep
      )
    , ( m_file      = ((!eoi > m_record) % m_lineBreak) > -m_lineBreak > eoi
      )
    )
};
}//x3_crtp namespace

const x3_crtp::CsvParser csv_parser(',');
auto bench_parser=csv_parser.start();
 
#endif//NONIUS_X3_CRTP_HPP_INCLUDED_2017_01_22_1801
