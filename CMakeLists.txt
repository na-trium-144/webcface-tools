cmake_minimum_required(VERSION 3.0)
project(webcface-tools CXX)
find_package(webcface CONFIG REQUIRED)

add_subdirectory(external/tiny-process-library)
add_subdirectory(external/tomlplusplus)

add_executable(webcface-launcher launcher/main.cc)
target_link_libraries(webcface-launcher
    webcface::webcface
    tiny-process-library
    tomlplusplus_tomlplusplus
)
install(TARGETS webcface-launcher
    RUNTIME DESTINATION bin
)

