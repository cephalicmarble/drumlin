#ifndef TAO_FORWARD_H
#define TAO_FORWARD_H

#ifdef TAOJSON
# include "tao/json.hpp"
#else
# ifndef TAOCPP_JSON_INCLUDE_JSON_HPP
# define TAOCPP_JSON_INCLUDE_JSON_HPP
# endif
#endif // TAOJSON

#include <vector>
#include <map>
#include <string>

namespace tao { namespace json {
#ifndef TAOJSON
    template< template< typename ... > class Traits >
    class basic_value;
    template< typename T, typename >
    struct traits;
#endif // TAOJSON
    using value = basic_value<traits>;
    using array_t = std::vector< value >;
    using object_t = std::map< std::string, value >;
    typedef std::initializer_list< array_t::value_type > array_initializer;
    typedef std::initializer_list< object_t::value_type > object_initializer;
} }

#endif // TAO_FORWARD_H
