
cmake_minimum_required (VERSION 2.6)
project (nekocord)

set(CMAKE_CXX_STANDARD 17)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif ()

find_package(OpenSSL REQUIRED)
add_subdirectory("external/nekolib")

file(GLOB_RECURSE NEKOCORD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")#define
add_library(nekocord STATIC ${NEKOCORD_SOURCES})
target_link_libraries(nekocord PUBLIC nekolib crypto ssl cpr)
target_include_directories(nekocord PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/neko/discord")
target_include_directories(nekocord PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" nekolib)

# Cotire is well wanted, so many headers
add_definitions(-DASIO_STANDALONE=1)
include("utils/cotire/CMake/cotire.cmake")
cotire(nekocord)

# Example project
add_executable(example example/main.cpp)
target_compile_options(example PRIVATE -ggdb)
target_link_libraries(example PRIVATE nekocord )
target_include_directories(example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
