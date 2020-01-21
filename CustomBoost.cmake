function(CustomBoost BOOST_REQUIRED_LIBS CUSTOM_BOOST)
    if(CUSTOM_BOOST)
        if(WIN32)
            set (Boost_INCLUDE_DIRS "D:\\\\boost_1_72_0")
            set (Boost_LIBRARY_DIRS "D:\\\\boost_1_72_0\\\\stage\\\\lib")
        else()
            set (Boost_INCLUDE_DIRS "/code/boost_1_72_0")
            set (Boost_LIBRARY_DIRS "/code/boost_1_72_0/stage/lib")
        endif()
        list(TRANSFORM ${BOOST_REQUIRED_LIBS} PREPEND "-l")
        set(Boost_LIBRARIES ${BOOST_REQUIRED_LIBS})
    else()
        find_package(Boost
                1.72.0
                REQUIRED
                COMPONENTS ${BOOST_REQUIRED_LIBS}
                )
    endif()
endfunction()
