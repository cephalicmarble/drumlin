#ifndef _BEHAVIOUR_H
#define _BEHAVIOUR_H

#include <iterator>
#include <iostream>
#include <string_view>
#include "../../tao/pegtl.hpp"

namespace grammar {

struct number
: tao::pegtl::plus< tao::pegtl::digit > {};

struct addition;  // Forward declaration to break the cyclic dependency.

struct bracket
: tao::pegtl::if_must< tao::pegtl::one< '(' >, addition, tao::pegtl::one< ')' > > {};

struct atomic
: tao::pegtl::sor< number, bracket > {};

struct addition
: tao::pegtl::list< atomic, tao::pegtl::one< '+' > > {};

template< typename Rule > struct my_action
: tao::pegtl::nothing< Rule > {};

template<>
struct my_action< grammar::addition >
{
    // template< typename ActionInput >
    // static void apply(const ActionInput&)
    // {
    //     std::cout << "blargle";
    // }
    static void apply0() {
        std::cout << "argle";
    }
};

template< typename ParseInput >
std::string as_string( ParseInput& in )
{
   // Set up the states, here a single std::string as that is
   // what our action requires as additional function argument.
   std::string out;

   // Start the parsing run with our grammar, action and state.
   tao::pegtl::parse< addition, my_action >( in, out );

   // Do something with the result.
   return out;
}

}

#endif // _BEHAVIOUR_H
