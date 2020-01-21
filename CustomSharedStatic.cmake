function(SharedStaticTarget TARGET SOURCES RELATIVE_PATH)
    list(TRANSFORM SOURCES REPLACE "^/?([^/]+/)+" "${RELATIVE_PATH}")
    # static library
    add_library(${TARGET} STATIC ${SOURCES})
    # dynamic library
    list(TRANSFORM SOURCES APPEND ".o")
    list(TRANSFORM SOURCES PREPEND "CMakeFiles/${TARGET}.dir/")
    add_custom_command(TARGET ${TARGET}
        PRE_LINK
        COMMAND g++ -shared -Wl,-soname,lib${TARGET}.so -o lib${TARGET}.so ${SOURCES}
        BYPRODUCTS lib${TARGET}.so
        WORKING_DIRECTORY .
        )
endfunction()
