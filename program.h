#ifdef _PROGRAM_H_
#define _PROGRAM_H_

#include <string>
#include <boost/spirit/include/qi.hpp>

namespace drumlin {

namespace Work {

template <typename Iterator, typename Skipper>
struct my_grammar
    : boost::qi::grammar<Iterator,
    boost::tuple<string, std::vector<statement>>(), Skipper>
{
    my_grammar() : my_grammar::base_type{program}
    {
        program             = head > boost::qi::char_('\n') > statements;
        head                = +boost::qi::alnum;
        statements          = +(statement > boost::qi::char_('\n'));
        statement           = boost::qi::char_('\n') | boost::qi::str("Thread");
    }

    qi::rule<Iterator, boost::tuple<string, std::vector<statement>>(), Skipper> program;
    qi::rule<Iterator, std::string, Skipper> head;
    qi::rule<Iterator, std::vector<statement>, Skipper> statements;
    qi::rule<Iterator, statement, Skipper> statement;
};

struct print : public boost::static_visitor<>
{
  template <typename T>
  void operator()(T t) const
  {
    std::cout << std::boolalpha << t << ';';
  }
};

template <typename Iterator, typename Skipper>
class ProgramParser {
    // program          ::= head '\n' (statement '\n')+
    // head             ::= [-A-Z0-9]+
    // statement        ::= ('\n' | nominator | descriptor)
    // nominator        ::= 'Thread:' identifier
    // descriptor       ::= (tilde? word brackets '->')? (brackets '=')? (prefix-ident | identifier) parenthetical?
    // tilde            ::= '~'
    // word             ::= 'every' | 'each' | 'wait' | 'expect'
    // brackets         ::= '[' specifier ']'
    // prefix-ident     ::= <url> | (identifier ':' identifier)
    // identifier       ::= [-a-zA-Z0-9]+
    // parenthetical    ::= '(' specifier ')'
    // specifier        ::= (star{1,2} | percent{1,2} | anglebrackets | identifier | flag-ident)
    // star             ::= '*'
    // percent          ::= '%'
    // anglebrackets    ::= '<' identifier '>'
    // flag-ident       ::= (star{1,2}|percent{1,2}|[-*%a-zA-Z0-9])+

    // descriptor{
    //     tilde?
    //     prompt?
    //     brackets?
    //     prefixIdentifierOrIdentifier
    //     specifier
    // }
    // brackets {
    //     specifier
    // }
    // prefixIdentifierOrIdentifier {
    //     string
    // }
    // specifier{
    //     string
    //     bracketed | starred | percented
    // }
public:
    static ProgramParser parse(std::string)
    {
        auto ret = new ProgramParser();
        auto it = text.begin();
        my_grammar grammar;
        bool match = qi::phrase_parse(it, s.end(), grammar, ascii::space);
        std::cerr << std::boolalpha << match << std::endl;
        std::cerr << std::string(text.begin(), it) << std::endl;
        return ret;
    }
};

} // Work

} // drumlin

#endif // _PROGRAM_H_
