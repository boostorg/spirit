//OriginalSource:
//  Post to spirit-general@lists.sourceforge.net
//  with headers
/*
From: =?UTF-8?Q?S=c3=b8ren_Enevoldsen?= <muzzleflame@gmail.com>
Newsgroups: gmane.comp.parsers.spirit.general
Subject: Small grammar, long compilation time
Date: Thu, 26 Oct 2017 16:03:44 +0200
Lines: 123
Approved: news@gmane.org
Message-ID: <a5c089be-f118-3297-147f-588f23818bbb@gmail.com>
 */
//  and archived at url:
/*
https://sourceforge.net/p/spirit/mailman/message/36091645/
 */
//=================================    
#define BOOST_SPIRIT_X3_DEBUG
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace wccs_parser {

namespace x3 = boost::spirit::x3;
namespace ascii = x3::ascii;
namespace qi = boost::spirit::qi;

using x3::long_;
using x3::ulong_;
using x3::lexeme;
using ascii::char_;

//--- Ast structures

struct AstChannel {
     std::string label;
     bool complement;
};

struct AstAction {
     AstChannel channel;
     uint32_t weight;
};

struct AstRenaming {
     std::string from;
     std::string to;
};

struct AstNullProcess;
struct AstActionPrefixProcess;
struct AstChoiceProcess;
struct AstCompositionProcess;
struct AstRestrictionProcess;
struct AstRenamingProcess;
struct AstConstantProcess;

using AstAnyProcess = x3::variant<
     x3::forward_ast<AstNullProcess>,
     x3::forward_ast<AstActionPrefixProcess>,
     x3::forward_ast<AstChoiceProcess>,
     x3::forward_ast<AstCompositionProcess>,
     x3::forward_ast<AstRestrictionProcess>,
     x3::forward_ast<AstRenamingProcess>,
     x3::forward_ast<AstConstantProcess>
>;

struct AstNullProcess {};
struct AstActionPrefixProcess {
     AstAction action;
     AstAnyProcess subProcess;
};
struct AstChoiceProcess {
     std::vector<AstAnyProcess> subProcesses;
};
struct AstCompositionProcess {
     std::vector<AstAnyProcess> subProcesses;
};
struct AstRestrictionProcess {
     AstAnyProcess subProcess;
     std::vector<std::string> labels;
};
struct AstRenamingProcess {
     AstAnyProcess subProcess;
     std::vector<AstRenaming> renamings;
};
struct AstConstantProcess {
     std::string processName;
};

} // End namespace

BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstChannel, label, complement)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstAction, channel, weight)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstRenaming, from, to)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstActionPrefixProcess, action, 
subProcess)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstChoiceProcess, subProcesses)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstCompositionProcess, subProcesses)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstRestrictionProcess, 
subProcess, labels)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstRenamingProcess, subProcess, 
renamings)
BOOST_FUSION_ADAPT_STRUCT(wccs_parser::AstConstantProcess, processName)

namespace wccs_parser {

template <typename T> auto rule = [](const char* name = typeid(T).name()) {
     struct _{};
     return x3::rule<_, T> {name};
};

template <typename T> auto as = [](auto p) { return rule<T>() = p; };

//--- Rules

auto constantName = as<std::string>(x3::lexeme[ascii::upper >> 
  *(ascii::alnum)]);
auto label        = as<std::string>(x3::lexeme[ascii::lower >> 
  *(ascii::alnum)]);

auto channel      = as<AstChannel>(label >> x3::matches['!']);

auto action       = as<AstAction>('<' >> channel >> ',' >> x3::ulong_ >> 
  '>');

auto renamingPair = as<AstRenaming>(label > "=>" > label);

auto nullProcess  = as<AstNullProcess>(x3::omit['0']);
auto constant     = as<AstConstantProcess>(constantName);

x3::rule<struct processRule, AstAnyProcess> process{"process"};

#define USE_PROCESS_IN_PAREN
auto const parenProcess 
  = '(' 
#ifdef USE_PROCESS_IN_PAREN
  ///  Here be compile time dragons.
  > process
#else  
  > nullProcess 
#endif
  > ')'
  ;

auto const primitive = rule<AstAnyProcess>("primitive")
     = parenProcess
     | nullProcess
     | constant;

auto const restrictionActual = as<AstRestrictionProcess> (primitive >> 
'\\' >> '{' >> label % ',' >> '}');
auto const restriction = rule<AstAnyProcess> ("restriction")
     = primitive >> !x3::lit('\\')
     | restrictionActual;

auto const renamingActual = as<AstRenamingProcess> (restriction >> '[' 
 >> renamingPair % ',' >> ']');
auto const renaming = rule<AstAnyProcess> ("renaming")
     = restriction >> !x3::lit('[')
     | renamingActual;

x3::rule<struct actionPrefixingRule, AstAnyProcess> 
actionPrefix{"actionPrefix"};
auto const actionPrefixActual = as<AstActionPrefixProcess> (action > 
('.' > actionPrefix));
auto const actionPrefix_def
     = actionPrefixActual
     | renaming;

auto const compositionActual = as<AstCompositionProcess> (actionPrefix % 
'|');
auto const composition = rule<AstAnyProcess> ("composition")
     = actionPrefix >> !x3::lit('|')
     | compositionActual;

auto const choiceActual = as<AstChoiceProcess> (composition % '+');
auto const choice = rule<AstAnyProcess> ("choice")
     = composition >> !x3::lit('+')
     | choiceActual;

auto const process_def = choice;

BOOST_SPIRIT_DEFINE(actionPrefix)
BOOST_SPIRIT_DEFINE(process)

auto const entry = x3::skip(ascii::space) [process];


} //End namespace

int main() {

     std::string strs[]=
     {
        "0",
        "(0)",
        "<act,4>.0",
        "<act!,0>.0",
        "0 + 0",
        "(0) + 0",
        "0 + <act,4>.0",
        "Name",
        "<act,2>.Name",
        "0 + Name + <foo,1>.<bar,2>.NameB",
        "0 | (P+Q)\\{a,b}",
        "(<act,4>.0) [act => foo, bar => baz]"
     };
     bool all_ok=true;
     for(std::string str : strs)
     {
         wccs_parser::AstAnyProcess root;
         auto iter = str.begin();
         auto end = str.end();
         bool r = parse(iter, end, wccs_parser::entry, root);
         all_ok=all_ok && r;
         if (r)
         {
             std::cout << str << std::endl << std::endl << " Parses OK: " << std::endl;
             std::cout << "\n-------------------------\n";
         }
         else
         {
             std::cout << "Parsing failed\n";
             std::cout << "-------------------------\n";
         }

         if (iter != end) std::cout << "Partial match" << std::endl;
     }
     std::cout<<"all_ok="<<all_ok<<"\n";
     return 0;
}
