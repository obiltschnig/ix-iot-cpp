project(PahoSubscribe)
 cmake_minimum_required(VERSION 3.0)
 add_definitions("-std=c++14")

 include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
 conan_basic_setup()

 add_executable(PahoSubscribe PahoSubscribe.cpp)
 target_link_libraries(PahoSubscribe ${CONAN_LIBS})
