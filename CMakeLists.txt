cmake_minimum_required(VERSION 3.9)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(dflat CXX)


#### PROJECT SOURCE FILES ####

add_library(dflat_common
    src/lexer.cpp src/lexer.hpp
    src/lexercore.cpp src/lexercore.hpp
    src/token.cpp src/token.hpp
    src/parser.cpp src/parser.hpp
    src/asn.cpp src/asn.hpp
    src/asn_typecheck.cpp src/asn_codegen.cpp
    src/config.hpp
    src/type.cpp src/type.hpp
    src/typechecker.cpp src/typechecker.hpp
    src/typechecker_tools.cpp src/typechecker_tools.hpp
    src/classmeta.cpp src/classmeta.hpp
    src/codegenerator.hpp
    src/codegenerator.cpp
    src/codegenerator_tools.cpp src/codegenerator_tools.hpp
    )

add_executable(dflat
    src/main.cpp
    )

add_executable(tests
    test/main.cpp
    test/lexercore_tests.cpp
    test/lexer_tests.cpp
    test/parser_tests.cpp
    test/token_helpers.hpp
    test/typechecker_tests.cpp
    test/codegenerator_tests.cpp
    )


#### PROJECT LINKED LIBRARIES ####

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(DFLAT_LIBS -lgcov)
else ()
    set(DFLAT_LIBS )
endif ()

target_link_libraries(dflat_common ${DFLAT_LIBS}
    )
    
target_link_libraries(dflat ${DFLAT_LIBS}
    dflat_common
    )

target_link_libraries(tests ${DFLAT_LIBS}
    dflat_common
    )


#### PROJECT INCLUDE DIRECTORIES ####

set(DFLAT_INCLUDES
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    SYSTEM
        ${CMAKE_CURRENT_SOURCE_DIR}/external
    )

target_include_directories(dflat_common ${DFLAT_INCLUDES}
    )

target_include_directories(dflat ${DFLAT_INCLUDES}
    )

target_include_directories(tests ${DFLAT_INCLUDES}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test
    )


#### PROJECT COMPILE OPTIONS ####

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(DFLAT_COMPILE_OPTIONS
        PUBLIC
            -Wall 
            -Wextra 
            -Werror=return-type
            -Werror=switch
            -Wshadow
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(DFLAT_COMPILE_OPTIONS
        PUBLIC
            -Wall 
            -Wextra 
            -Werror=return-type
            -Werror=switch
            -Wfatal-errors
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wlogical-op
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -fprofile-arcs 
            -ftest-coverage
            -Wno-unknown-pragmas
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(DFLAT_COMPILE_OPTIONS
        PUBLIC
            /W4
        )
endif()
   
# Assume same opts for all.
target_compile_options(dflat_common ${DFLAT_COMPILE_OPTIONS})
target_compile_options(dflat ${DFLAT_COMPILE_OPTIONS})
target_compile_options(tests ${DFLAT_COMPILE_OPTIONS})
