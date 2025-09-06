cmake_minimum_required(VERSION 3.14)
project(obfy LANGUAGES CXX)

add_library(obfy INTERFACE)
target_include_directories(obfy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(obfy INTERFACE cxx_std_11)

add_executable(example example/main.cpp)
target_link_libraries(example PRIVATE obfy)

enable_testing()
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(obfy_tests tests/obfy_tests.cpp example/main.cpp)
target_link_libraries(obfy_tests PRIVATE obfy Boost::unit_test_framework)
target_compile_definitions(obfy_tests PRIVATE UNIT_TESTS)
add_test(NAME obfy_tests COMMAND obfy_tests)

install(TARGETS obfy)
install(DIRECTORY include/ DESTINATION include)
