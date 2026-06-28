cmake_minimum_required(VERSION 3.14)
project(VectorMappingCore CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Portable core library (MFC-free).
add_library(vmcore
    src/AreaMap.cpp
    src/Nastran.cpp
    src/Adx.cpp
    src/Mapper.cpp
)
target_include_directories(vmcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Command-line front-end.
add_executable(vmap src/cli_main.cpp)
target_link_libraries(vmap PRIVATE vmcore)

# Tests.
enable_testing()
add_executable(vmcore_tests tests/test_core.cpp)
target_link_libraries(vmcore_tests PRIVATE vmcore)
add_test(NAME vmcore_tests COMMAND vmcore_tests)
