cmake_minimum_required(VERSION 3.14)
project(obfy VERSION 1.0.0 LANGUAGES CXX)

add_library(obfy INTERFACE)
target_include_directories(obfy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(obfy INTERFACE cxx_std_11)

option(OBFY_BUILD_EXAMPLE "Build example program" ON)
option(OBFY_BUILD_TESTS "Build unit tests" ON)

if(OBFY_BUILD_EXAMPLE)
    add_executable(example example/main.cpp)
    target_link_libraries(example PRIVATE obfy)
endif()

if(OBFY_BUILD_TESTS)
    enable_testing()
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    add_executable(obfy_tests tests/obfy_tests.cpp tests/license_tests.cpp example/main.cpp)
    target_link_libraries(obfy_tests PRIVATE obfy Boost::unit_test_framework)
    target_compile_definitions(obfy_tests PRIVATE UNIT_TESTS)
    add_test(NAME obfy_tests COMMAND obfy_tests)

    add_executable(obfy_tests_debug tests/obfy_tests.cpp tests/license_tests.cpp example/main.cpp)
    target_link_libraries(obfy_tests_debug PRIVATE obfy Boost::unit_test_framework)
    target_compile_definitions(obfy_tests_debug PRIVATE UNIT_TESTS OBFY_DEBUG)
    add_test(NAME obfy_tests_debug COMMAND obfy_tests_debug)

    add_test(NAME invalid_str_literal
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++11 -I${CMAKE_CURRENT_SOURCE_DIR}/include -c ${CMAKE_CURRENT_SOURCE_DIR}/tests/invalid_str_char.cpp -o invalid_str_char.o)
    set_tests_properties(invalid_str_literal PROPERTIES WILL_FAIL TRUE)

    add_test(NAME invalid_wstr_literal
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++11 -I${CMAKE_CURRENT_SOURCE_DIR}/include -c ${CMAKE_CURRENT_SOURCE_DIR}/tests/invalid_wstr_char.cpp -o invalid_wstr_char.o)
    set_tests_properties(invalid_wstr_literal PROPERTIES WILL_FAIL TRUE)
endif()

install(TARGETS obfy)
install(DIRECTORY include/ DESTINATION include)
