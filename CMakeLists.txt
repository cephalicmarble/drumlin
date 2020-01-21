cmake_minimum_required (VERSION 3.2.0 FATAL_ERROR)

set (CMAKE_VERBOSE_MAKEFILE ON)
# set C++ language standard
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# set project and version
project (drumlin VERSION 1.0.0 LANGUAGES CXX)
# options
set(BUILD_TAO_JSON_TESTS 0)
# dependencies
set(TAO_JSON_DIR ../json)
set(BOOST_REQUIRED_LIBS  "program_options regex thread timer")
set(CUSTOM_BOOST ON)
#
include(CustomBoost.cmake)
CustomBoost(${BOOST_REQUIRED_LIBS} ${CUSTOM_BOOST})
include_directories(
        ${Boost_INCLUDE_DIRS}
        ${TAO_JSON_DIR}/include
        #        test
        .
)
link_directories(
        ${Boost_LIBRARY_DIRS}
)
link_libraries(
        ${Boost_LIBRARIES}
)
# compile options
if(WIN32)
else()
    set (gccver "8.2.1")
    add_compile_options(-g -fPIC -W -Wall -Wextra -I/usr/include/c++/${gccver})
    add_link_options(-static -lstdc++ -lgcc)
endif()
# sources list
file(GLOB DRUMLIN_SOURCES *.cpp)

include(CustomSharedStatic.cmake)
SharedStaticTarget(drumlin "${DRUMLIN_SOURCES}" "")

# tao json testing
if(BUILD_TAO_JSON_TESTS)
    add_subdirectory (${TAO_JSON_DIR})
endif()

#add_library(RdfTriple src/RdfTriple.cpp)
#link_libraries(RdfTriple)
#
#add_executable(test test/test.cpp)
#add_executable(IriTest test/IriTest.cpp)
