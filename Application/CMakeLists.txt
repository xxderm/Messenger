cmake_minimum_required (VERSION 3.7)
project (App)

# Src
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "Source/../*.cpp")
include_directories(Include)
include_directories(../Utils)

# Vendor
include_directories("../Vendor/sdl/vc/include/")
link_directories("../Vendor/sdl/vc/lib/x64/")

add_executable(App ${sources})

target_link_libraries(App SDL2main SDL2 SDL2_net)
