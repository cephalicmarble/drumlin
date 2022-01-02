#ifndef _BEHAVIOUR_H
#define _BEHAVIOUR_H

#include <iterator>
#include <iostream>
#include <string_view>
#include "../drumlin/drumlin.h"
#include "../drumlin/exception.h"
#include "../drumlin/string_list.h"
#include "../tao/pegtl.hpp"

namespace grammar {

template< typename Rule > struct my_action
: tao::pegtl::nothing< Rule >
{};

struct number
: tao::pegtl::plus< tao::pegtl::digit >
{};

struct dot
: tao::pegtl::if_must< tao::pegtl::one<'.'>, tao::pegtl::ascii::identifier >
{};

struct bracket
: tao::pegtl::if_must< tao::pegtl::one< '[' >, tao::pegtl::seq< number, tao::pegtl::one< ']' > > >
{};

struct source
: tao::pegtl::seq< tao::pegtl::ascii::identifier >
{};

struct sourceRange
: tao::pegtl::seq< tao::pegtl::ascii::identifier, bracket >
{};

struct sourceRangeDot
: tao::pegtl::seq< tao::pegtl::ascii::identifier, bracket, dot >
{};

struct state;

struct line
: tao::pegtl::action<my_action,
    tao::pegtl::state< state,
        tao::pegtl::sor<sourceRangeDot, sourceRange, source>
    > >
{};

struct state {
    static std::stringstream s_ss;
    std::unique_ptr<std::stringstream> m_ss;
    string_list m_list;
    state():m_ss(&s_ss)
    {
        m_ss->clear();
        PLATE1("state()");
    }
    state(state & rhs):m_ss(rhs.m_ss.release())
    {
        PLATE1("state(&rhs)");
    }
    ~state(){
        PLATE1("~state");
        std::cerr << m_ss.get()->str() << std::endl;
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

template<>
struct my_action< line >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        *s.m_ss << " grammar:" << in.string_view() << std::endl;
        s.m_list << std::make_pair(std::string("sourceRange"), in.string_view());
    }
};

template<>
struct my_action< number >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        *s.m_ss << " number:" << in.string_view() << std::endl;
    }
};

template<>
struct my_action< tao::pegtl::ascii::identifier >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        *s.m_ss << " identifier:" << in.string_view() << std::endl;
    }
};

template<>
struct my_action< bracket >
{
    template< typename ActionInput >
    static void apply(const ActionInput&in, state &s)
    {
        *s.m_ss << " bracket:" << in.string_view() << std::endl;
    }
};

template< typename ParseInput >
std::string as_string( ParseInput& in )
{
   // Set up the states, here a single std::string as that is
   // what our action requires as additional function argument.
   state s;

   // Start the parsing run with our grammar, action and state.
   tao::pegtl::parse< line, my_action >( in, s );

   // Do something with the result.
   return s.m_ss->str();
}

template< typename ParseInput, typename Grammar >
std::string parse_as_string( ParseInput& in )
{
    std::string instr(in);
    std::string source("cmdline");
    bool finished = true;
    tao::pegtl::memory_input< tao::pegtl::tracking_mode::eager, tao::pegtl::eol::crlf > input( instr, source );
    grammar::state s;
    try{
        finished = tao::pegtl::parse< Grammar >( input, s );
        if(!finished)
            throw drumlin::Exception("!parsed");
    }catch(tao::pegtl::parse_error &e){
        const auto p = e.positions().front();
        std::cerr << e.what() << std::endl << input.line_at( p )<< '\n' << std::setw( p.column ) << '^' << std::endl;
        finished = false;
    }catch(std::logic_error &e) {
        std::cerr << e.what() << std::endl;
        finished = false;
    }catch(drumlin::Exception &e) {
        std::cerr << e.what();
        finished = false;
    }
    std::cout << s.m_ss->str() << std::endl << "fin" << std::endl;
    if(finished)
        std::cout << "finished";
    return s.m_ss->str();
}

} // namespace grammar

#endif // _BEHAVIOUR_H
