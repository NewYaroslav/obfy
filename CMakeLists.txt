cmake_minimum_required(VERSION 3.14)
project(obfy LANGUAGES CXX)

add_library(obfy INTERFACE)
target_include_directories(obfy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(obfy INTERFACE cxx_std_11)

add_executable(example example/main.cpp)
target_link_libraries(example PRIVATE obfy)

install(TARGETS obfy)
install(DIRECTORY include/ DESTINATION include)
