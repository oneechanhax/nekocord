
cmake_minimum_required (VERSION 2.6)
project (nekocord)

set(CMAKE_CXX_STANDARD 17)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif ()
if (NOT NEKOCORD_TYPE)
    set(NEKOCORD_TYPE STATIC)
endif ()

file(GLOB_RECURSE NEKOCORD_SOURCES "src/*.cpp")
add_library(nekocord ${NEKOCORD_TYPE} ${NEKOCORD_SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG") # Debug
        target_compile_options(nekocord PUBLIC -g3 -ggdb -fasynchronous-unwind-tables -Og)
    else() # Release
        target_compile_definitions(nekocord PUBLIC -DNDEBUG)
        target_compile_options(nekocord PUBLIC -s -mtune=generic -Ofast -DNDEBUG -fshort-enums -fno-plt -fvisibility=hidden -fno-operator-names)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(/D_CRT_SECURE_NO_WARNINGS=1) # So we can use our normal libc functions
    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG") # Debug
        target_compile_definitions(nekocord PUBLIC /RTC /W2 /Z7 CONFIG Debug)
    else() # Release
        target_compile_definitions(nekocord PUBLIC /w /GR- CONFIG Release)
    endif()
endif ()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Dependancies
# Cpr
set(USE_SYSTEM_CURL ON)
set(NEKOCORD_LINK_LIBS cpr)

# Websocketpp
find_package(OpenSSL REQUIRED)
set(NEKOCORD_LINK_LIBS ${NEKOCORD_LINK_LIBS} ${OPENSSL_LIBRARIES})

# Nekolib
add_subdirectory("external/nekolib")
set(NEKOCORD_INCLUDE_DIRS ${NEKOCORD_INCLUDE_DIRS} ${NEKOLIB_INCLUDE_DIRS})
set(NEKOCORD_LINK_LIBS ${NEKOCORD_LINK_LIBS} ${NEKOLIB_LIBRARIES})

# Export info
set(NEKOCORD_FOUND true PARENT_SCOPE)
set(NEKOCORD_LIBRARIES ${NEKOCORD_LINK_LIBS} nekocord PARENT_SCOPE)
set(NEKOCORD_INCLUDE_DIRS ${NEKOCORD_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

target_link_libraries(nekocord ${NEKOCORD_LINK_LIBS})
target_include_directories(nekocord PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/neko/discord)
target_include_directories(nekocord PRIVATE ${NEKOCORD_INCLUDE_DIRS})

# Cotire is well wanted, so many headers
include("utils/cotire/CMake/cotire.cmake")
cotire(nekocord)

# Example project
add_executable(example example/main.cpp)
target_compile_options(example PRIVATE -ggdb)
target_link_libraries(example PRIVATE ${NEKOCORD_LINK_LIBS} nekocord)
target_include_directories(example PRIVATE ${NEKOCORD_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
