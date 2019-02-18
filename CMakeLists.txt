cmake_minimum_required(VERSION 3.9)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(dflat CXX)

set(DFLAT_COMMON_FILES
    src/lexer.cpp src/lexer.hpp
    src/lexercore.cpp src/lexercore.hpp
    src/token.cpp src/token.hpp
    )

project(tests CXX)

add_executable(dflat ${DFLAT_COMMON_FILES}
    src/main.cpp
    )

add_executable(tests ${DFLAT_COMMON_FILES}
    test/main.cpp
    test/lexercore_tests.cpp
    test/lexer_tests.cpp
    )
    
target_link_libraries(dflat 
    )

target_include_directories(dflat
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    SYSTEM
        ${CMAKE_CURRENT_SOURCE_DIR}/external
    )

target_include_directories(tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
        ${CMAKE_CURRENT_SOURCE_DIR}/test
    SYSTEM
        ${CMAKE_CURRENT_SOURCE_DIR}/external
    )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(dflat
        PUBLIC
            -Wall 
            -Wextra 
            -Werror=return-type
            -Werror=switch
        
            -Wshadow
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(dflat
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
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()
