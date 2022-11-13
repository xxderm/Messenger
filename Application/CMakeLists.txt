cmake_minimum_required (VERSION 3.7)
project (App)

# Src
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "Source/../*.cpp")
include_directories(Include)
include_directories(../Utils)

# Vendor
include_directories("../Vendor/sdl/vc/include/")
link_directories("../Vendor/sdl/vc/lib/x64/")

set(imguisources
        "../Vendor/imgui/imgui.cpp"
        "../Vendor/imgui/imgui_draw.cpp"
        "../Vendor/imgui/imgui_tables.cpp"
        "../Vendor/imgui/imgui_widgets.cpp"
        "../Vendor/imgui/backends/imgui_impl_sdl.cpp"
        "../Vendor/imgui/backends/imgui_impl_opengl3.cpp")
include_directories("../Vendor/imgui/")
include_directories("../Vendor/imgui/backends/")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(App ${sources} ${imguisources})

target_link_libraries(App SDL2main SDL2 SDL2_net ${OPENGL_LIBRARIES})
