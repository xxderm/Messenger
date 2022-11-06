cmake_minimum_required(VERSION 3.14)
project(Test)

set(CMAKE_CXX_STANDARD 20)

#file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        Test
        PacketTest.cpp
)
target_link_libraries(
        Test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Test)

#add_executable(Test ${sources})