cmake_minimum_required(VERSION 3.26)
project(GeminiBook VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find the OpenSSL package
find_package(OpenSSL)
if (OpenSSL_FOUND)
    link_libraries(OpenSSL::SSL OpenSSL::Crypto)
    set(WITH_OPENSSL ON CACHE BOOL "" FORCE)
else ()
    message(WARNING "OpenSSL not found! Deprecated OS libraries such as Apple Security or GnuTLS may end up being used!")
endif ()

# libhv
include(FetchContent)
FetchContent_Declare(
        libhv
        GIT_REPOSITORY https://github.com/ithewei/libhv.git
        GIT_TAG 4f05ec33106df4f6af14b474b61543156af25db9
)
option(BUILD_STATIC "Build as static" ON)
if (BUILD_STATIC)
    set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(BUILD_STATIC ON CACHE BOOL "" FORCE)
else ()
    set(BUILD_SHARED ON CACHE BOOL "" FORCE)
    set(BUILD_STATIC OFF CACHE BOOL "" FORCE)
endif ()
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WITH_HTTP_SERVER OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libhv)

# Setup GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)

include_directories(.)
add_subdirectory(external)
add_subdirectory(OrderBook)
add_subdirectory(GeminiInterface)

add_executable(
        GeminiBook
        main.cpp
)
target_link_libraries(
        GeminiBook
        GeminiInterface
)