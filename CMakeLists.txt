project(drumlin)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
set(PKGCONFIG "/usr/bin/pkg-config")
set (CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-g -W -Wall -Wextra -pedantic -Wno-literal-suffix -DBOOST_ASIO_ENABLE_HANDLER_TRACKING)
cmake_minimum_required(VERSION 3.6)

find_package(Boost REQUIRED COMPONENTS system chrono date_time filesystem log program_options regex)

## options
#set(BUILD_TAO_JSON_TESTS 0)
## dependencies
#set(TAO_JSON_DIR ../tao)
#set(BOOST_REQUIRED_LIBS  "program_options regex thread timer")

#add_executable(${TARGET} test-main.cpp)
#target_link_libraries(${TARGET} "-lunwind -lpthread")
#file(GLOB DRUMLIN_FILES *.cpp)
##FILE(STRINGS "drumlin.files" DRUMLIN_FILES)
#foreach(FILE ${DRUMLIN_FILES})
#	message(${FILE})
#	target_sources(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/${FILE})
#	set(SOURCE_FILES "${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/${FILE}")
#endforeach()

FILE(STRINGS "drumlin.includes" INCLUDE_DIRS)

file(GLOB TESTS ./tests/*)
foreach(DIR ${TESTS})
    message(${DIR})
    # target is name of dir
    GET_FILENAME_COMPONENT(dirname ${DIR} NAME)
    message(rx->${dirname})
    if(dirname MATCHES "tao")
        message(${dirname})
    else()
        if(dirname MATCHES "^[-a-z]+.cpp$")
            # {target}.cpp has main()
            get_filename_component(target ${dirname} NAME_WE)
            message(T->tests/${target})
            add_executable(${target} ./tests/${dirname})
        else()
            set(target ${dirname})
            message(F->tests/${target})
            file(GLOB FILES ./tests/${dirname}/*.cpp)
            add_executable(${target} ./tests/${dirname}/${target}.cpp)
            foreach(FILE ${FILES})
                if(FILE MATCHES "${target}.cpp")
                else()
                    target_sources(${target} PRIVATE ${CMAKE_SOURCE_DIR}/${FILE})
                endif()
            endforeach()
        endif()
        FILE(STRINGS "drumlin.files" DRUMLIN_FILES)
        foreach(FILE ${DRUMLIN_FILES})
          target_sources(${target} PRIVATE ${CMAKE_SOURCE_DIR}/${FILE})
        endforeach()
    endif()
    # always add these
    # always link these
    target_link_libraries(${target} "-lunwind -lpthread -lgtest -lmd")
    target_link_libraries(${target} ${Boost_LIBRARIES})
endforeach()

# compile options
#if(WIN32)
#else()
#    add_compile_options(-g -fPIC -W -Wall -Wextra -I/usr/include/c++/${gccver})
#    add_link_options(-static -lstdc++ -lgcc)
#endif()
# sources list
#include(CustomSharedStatic.cmake)
#SharedStaticTarget(drumlin "${DRUMLIN_SOURCES}" "")

# tao json testing
#if(BUILD_TAO_JSON_TESTS)
#    add_subdirectory (${TAO_JSON_DIR})
#endif()

#add_library(RdfTriple src/RdfTriple.cpp)
#link_libraries(RdfTriple)
#
#add_executable(test test/test.cpp)
#add_executable(IriTest test/IriTest.cpp)
