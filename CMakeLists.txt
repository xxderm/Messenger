cmake_minimum_required (VERSION 3.7)
project (Messenger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -EHsc)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Application)
add_subdirectory(DedicatedHost)
add_subdirectory(Test)