#ifndef _MAIN_TAO_H
#define _MAIN_TAO_H

#define TAOCPP_JSON_INCLUDE_INTERNAL_ERRORS_HPP
#include <string>

#include "tao/json/external/pegtl.hpp"
#include "tao/json/external/pegtl/normal.hpp"
#include "tao/json/external/pegtl/parse_error.hpp"

#include "tao/json/internal/grammar.hpp"

namespace tao
{
   namespace json
   {
      namespace internal
      {
         template< typename Rule >
         struct errors
               : public tao::json_pegtl::normal< Rule >
         {
            static const std::string error_message;

            template< typename Input, typename ... States >
            static void raise( const Input & in, States && ... )
            {
               throw tao::json_pegtl::parse_error( error_message, in );
            }
         };

         // template<> const std::string errors< rules::text >::error_message /*__attribute__(( weak ))*/ = "no valid JSON";
         //
         // template<> const std::string errors< rules::end_array >::error_message /*__attribute__(( weak ))*/ = "incomplete array, expected ']'";
         // template<> const std::string errors< rules::end_object >::error_message /*__attribute__(( weak ))*/ = "incomplete object, expected '}'";
         // template<> const std::string errors< rules::member >::error_message /*__attribute__(( weak ))*/ = "expected member";
         // template<> const std::string errors< rules::name_separator >::error_message /*__attribute__(( weak ))*/ = "expected ':'";
         // template<> const std::string errors< rules::array_element >::error_message /*__attribute__(( weak ))*/ = "expected value";
         // template<> const std::string errors< rules::value >::error_message /*__attribute__(( weak ))*/ = "expected value";
         //
         // template<> const std::string errors< rules::edigits >::error_message /*__attribute__(( weak ))*/ = "expected at least one exponent digit";
         // template<> const std::string errors< rules::fdigits >::error_message /*__attribute__(( weak ))*/ = "expected at least one fraction digit";
         // template<> const std::string errors< rules::xdigit >::error_message /*__attribute__(( weak ))*/ = "incomplete universal character name";
         // template<> const std::string errors< rules::escaped >::error_message /*__attribute__(( weak ))*/ = "unknown escape sequence";
         // template<> const std::string errors< rules::chars >::error_message /*__attribute__(( weak ))*/ = "invalid character in string";
         // template<> const std::string errors< rules::string::content >::error_message /*__attribute__(( weak ))*/ = "unterminated string";
         // template<> const std::string errors< rules::key::content >::error_message /*__attribute__(( weak ))*/ = "unterminated key";
         //
         // template<> const std::string errors< tao::json_pegtl::eof >::error_message /*__attribute__(( weak ))*/ = "unexpected character after JSON value";

      } // internal

   } // json

} // tao
#include "tao/json.hpp"
using namespace tao;

#endif // _MAIN_TAO_H
