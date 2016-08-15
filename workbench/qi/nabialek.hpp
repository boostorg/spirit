#include <boost/preprocessor.hpp>

#define N_NAME(s) N_NAME##s(n)

#define N_NAME_KEYWORD kwd_
#define N_NAME_RULE r_

#define N_EXPR qi::lit('=') >> qi::int_
#define N_QUALIFIER g.

#define N_SYMN_RULE sym1
#define N_SYMN_PTR  sym2
#define N_SYMN_NABIALEK sym3

#define N_INPUT(z,n,text) N_BB_KWD(n) = n

#define N_BB_OP(s,n) BOOST_PP_IF(BOOST_PP_EQUAL(n,BOOST_PP_SUB(N_NR_KEYWORDS,1)), BOOST_PP_EMPTY(), N_BB_OP_##s)
#define N_BB_OP_ALTERNATIVE |
#define N_BB_OP_KEYWORD /

#define N_PREFIX(s) N_PREFIX_##s
#define N_PREFIX_ALTERNATIVE +(
#define N_PREFIX_NABIALEK    +nabialek(
#define N_PREFIX_KEYWORD     (
#define N_PREFIX_SOURCE

#define N_SUFFIX(s) N_SUFFIX_##s
#define N_SUFFIX_ALTERNATIVE )
#define N_SUFFIX_NABIALEK    )
#define N_SUFFIX_QNABIALEK   )
#define N_SUFFIX_KEYWORD     )
#define N_SUFFIX_SOURCE      

#define N_CREATE(n, t) N_CREATE_(N_NR_KEYWORDS,n,t)
#define N_CREATE_(c,n,t) N_PREFIX_##n N_BODY_##n(c, n, t) N_SUFFIX_##n
#define N_BODY_ALTERNATIVE(c,n,t) BOOST_PP_REPEAT(c, N_BB_##n, t)
#define N_BODY_KEYWORD(c,n,t) N_BODY_ALTERNATIVE(c,n,t)
#define N_BODY_SOURCE(c,n,t) BOOST_PP_STRINGIZE(N_BODY_ALTERNATIVE(c,n,t))
#define N_BODY_NABIALEK(c,n,t) N_SYM_##t(N_SYMN_NABIALEK) BOOST_PP_ENUM(c, N_BB_##n, t)

#define N_BB_ALTERNATIVE(z,n,s) (qi::lit(N_BB_SKWD(n)) >> N_BB_##s(n)) N_BB_OP(ALTERNATIVE,n)
#define N_BB_KEYWORD(z,n,s) repo::kwd(N_BB_SKWD(n))[N_BB_##s(n)] N_BB_OP(KEYWORD,n)
#define N_BB_NABIALEK(z,n,s) N_BB_##s(n)
#define N_BB_SOURCE(z,n,s)   N_BB_KWD(n) = n

#define N_BB_EXPR(n)    N_EXPR
#define N_BB_EXPR2(n)   N_EXPR
#define N_BB_RULE(n)    BOOST_PP_CAT(N_NAME_RULE,n)
#define N_BB_QRULE(n)   BOOST_PP_CAT(N_QUALIFIER, N_BB_RULE(n))
#define N_BB_KWD(n)     BOOST_PP_CAT(N_NAME_KEYWORD,n)
#define N_BB_SKWD(n)    BOOST_PP_STRINGIZE(N_BB_KWD(n))
#define N_BB_RULEDEF(n) BOOST_PP_CAT(N_NAME_RULE,n) = N_EXPR;

#define N_SYM_QRULE(m)  BOOST_PP_CAT(N_QUALIFIER,m),
#define N_SYM_RULE(m)   m,
#define N_SYM_EXPR(m)   m,
#define N_SYM_EXPR2(m)  BOOST_PP_CAT(N_QUALIFIER,m),

#define N_RREF(n)   N_RREF##n
#define N_RREF_PTR  &
#define N_RREF_RULE

#define N_TRICK_SYMBOL(z, n, m)(N_BB_SKWD(n), N_RREF_##m BOOST_PP_CAT(r_,n))
#define N_TRICK_ADD_SYMBOLS(n) BOOST_PP_CAT(N_SYMN_##n,.add) BOOST_PP_REPEAT(N_NR_KEYWORDS, N_TRICK_SYMBOL, n);

#define N_SYMBOL(z, n, m)(N_BB_SKWD(n), BOOST_PP_INC(n))
#define N_ADD_SYMBOLS BOOST_PP_CAT(N_SYMN_NABIALEK,.add) BOOST_PP_REPEAT(N_NR_KEYWORDS, N_SYMBOL,BOOST_PP_EMPTY());

#define N_DECLARE_RULES() qi::rule<char *, qi::space_type> alternative_d, alternative_r, kwd_d, kwd_r, nabialek_d, nabialek_r, BOOST_PP_ENUM(N_NR_KEYWORDS, N_BB_NABIALEK, RULE)
#define N_DEFINE_RULES() BOOST_PP_REPEAT(N_NR_KEYWORDS, N_BB_NABIALEK, RULEDEF)

namespace qi = boost::spirit::qi;
namespace repo = boost::spirit::repository::qi;
using mxc::qitoo::nabialek;

struct tg
{
    qi::symbols<char, qi::rule<char *, qi::space_type>>  N_SYMN_RULE;
    qi::symbols<char, qi::rule<char*, qi::space_type>*>  N_SYMN_PTR;
    qi::symbols<char, int>  N_SYMN_NABIALEK;

    qi::rule<char*, qi::locals<qi::rule<char*, qi::space_type>>, qi::space_type > ntrick_org;
    qi::rule<char*, qi::locals<qi::rule<char*, qi::space_type>*>, qi::space_type > ntrick_ptr;

    char* source = N_CREATE(SOURCE,EXPR);
    char* last = &source[strlen(source)];

    N_DECLARE_RULES();

    tg() {
        using qi::_1;
        using qi::_a;

        N_DEFINE_RULES()
        N_TRICK_ADD_SYMBOLS(RULE)
        N_TRICK_ADD_SYMBOLS(PTR)
        N_ADD_SYMBOLS

        kwd_d = N_CREATE(KEYWORD, EXPR);
        kwd_r = N_CREATE(KEYWORD, RULE);

        alternative_d = N_CREATE(ALTERNATIVE, EXPR);
        alternative_r = N_CREATE(ALTERNATIVE, RULE);

        nabialek_d = N_CREATE(NABIALEK, EXPR);
        nabialek_r = N_CREATE(NABIALEK, RULE);

        ntrick_org = +(N_SYMN_RULE[_a = _1] >> qi::lazy(_a));
        ntrick_ptr = +(N_SYMN_PTR[_a = _1] >> qi::lazy(*_a));
    }
};

tg g;


