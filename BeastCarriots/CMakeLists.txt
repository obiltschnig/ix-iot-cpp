project(BeastCarriots)
 cmake_minimum_required(VERSION 3.0)
 add_definitions("-std=c++14")

 include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
 conan_basic_setup()

 add_executable(BeastCarriots BeastCarriots.cpp)
 target_link_libraries(BeastCarriots ${CONAN_LIBS})
