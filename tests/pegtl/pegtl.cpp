#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <string_view>
#include "../../drumlin.h"
#include "../../exception.h"
#include "../../tao/pegtl.hpp"
#include "../../tao/pegtl/parse.hpp"

namespace grammar {

template< typename Rule > struct my_action
: tao::pegtl::nothing< Rule >
{};

struct number
: tao::pegtl::plus< tao::pegtl::digit >
{};

struct addition;  // Forward declaration to break the cyclic dependency.

struct bracket
: tao::pegtl::if_must< tao::pegtl::one< '(' >, addition, tao::pegtl::one< ')' > >
{};

struct printing
: tao::pegtl::ascii::print
{};

struct atomic
: tao::pegtl::sor< number, bracket >
{};

struct state {
    static std::stringstream s_ss;
    std::unique_ptr<std::stringstream> m_ss;
    state():m_ss(&s_ss)
    {
        PLATE1("state()");
    }
    state(state & rhs):m_ss(rhs.m_ss.release())
    {
        PLATE1("state(&rhs)");
    }
    ~state(){
        PLATE1("~state");
        (void)m_ss.release();
    }
    state &operator=(state & rhs) {
        PLATE1("state=rhs");
        m_ss.reset(rhs.m_ss.release());
        return *this;
    }
    state &operator=(state && rhs) {
        PLATE1("state=move");
        std::swap(m_ss, rhs.m_ss);
        return *this;
    }
    template <typename ParseInput>
    void success(const ParseInput &, state &st)
    {
        std::cout << st.m_ss->str().c_str();
    }
};
std::stringstream state::s_ss;

template<>
struct my_action< addition >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        (ostream&)*s.m_ss.get() << " " << std::string("addition").c_str();
        std::cout << "addition: " << std::endl << in.string_view() << '\n' << std::setw( in.input().column() ) << '^' << std::endl;
    }
};

template<>
struct my_action< atomic >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        (ostream&)*s.m_ss.get() << " " << std::string("atomic").c_str();
        std::cout << "atomic  : " << std::endl << in.string_view() << '\n' << std::setw( in.input().column() ) << '^' << std::endl;
    }
};

template<>
struct my_action< printing >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        (ostream&)*s.m_ss.get() << " " << std::string("printing").c_str();
        std::cout << "printing: " << std::endl << in.string_view() << '\n' << std::setw( in.input().column() ) << '^' << std::endl;
    }
};

struct addition
: tao::pegtl::action<my_action,
    tao::pegtl::state< state,
        tao::pegtl::list_must< tao::pegtl::rep_min_max<1, 1, atomic>, tao::pegtl::one< '+' > >
    >>
{};

struct grammar
: tao::pegtl::rep_min_max<1, 1, addition>
{};

} // namespace grammar

int main(int argc, char **argv)
{
    if(argc <= 1) {
        std::cout << "Missing argument.";
        return 0;
    }
    std::string instr(argv[1]);
    std::string source("cmdline");
    tao::pegtl::memory_input< tao::pegtl::tracking_mode::eager,
     tao::pegtl::eol::crlf > in( argv[1], argv[1]+instr.length(), source );
    try{
        std::string out;
        grammar::state s;
        bool parsed(tao::pegtl::parse< grammar::grammar, grammar::my_action >( in, s ));
        if(!parsed)
            throw drumlin::Exception("bork!");
    }catch(tao::pegtl::parse_error &e){
        const auto p = e.positions().front();
        std::cerr << e.what() << std::endl << in.line_at( p )<< '\n' << std::setw( p.column ) << '^' << std::endl;
    }catch(std::logic_error &e) {
        std::cerr << e.what() << std::endl;
    }catch(drumlin::Exception &e) {
        std::cerr << e.what();
    }
    std::cout << "finished." << std::endl;
    std::cout << grammar::state::s_ss.str();
    return 0;
}
