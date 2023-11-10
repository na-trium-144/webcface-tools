cmake_minimum_required(VERSION 3.0)
project(webcface-tools
    LANGUAGES CXX
    VERSION 1.1.3
    HOMEPAGE_URL "https://github.com/na-trium-144/webcface-tools"
    DESCRIPTION "WebCFace Client Applications"
)
add_compile_definitions(TOOLS_VERSION="${PROJECT_VERSION}")
find_package(webcface CONFIG REQUIRED)

add_subdirectory(external/tiny-process-library)
add_subdirectory(external/tomlplusplus)
add_subdirectory(external/cli11)

add_executable(webcface-launcher launcher/main.cc launcher/launcher.cc)
target_link_libraries(webcface-launcher
    webcface::webcface
    tiny-process-library
    tomlplusplus_tomlplusplus
    CLI11
)
install(TARGETS webcface-launcher RUNTIME DESTINATION bin)

add_executable(webcface-send send/main.cc)
target_link_libraries(webcface-send
    webcface::webcface
    CLI11
)
install(TARGETS webcface-send RUNTIME DESTINATION bin)

add_executable(webcface-viewer viewer/main.cc viewer/display.cc)
target_link_libraries(webcface-viewer
    webcface::webcface
    CLI11
    ncurses
)
install(TARGETS webcface-viewer RUNTIME DESTINATION bin)

set(CPACK_VERBATIM_VARIABLES yes)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_PACKAGE_CONTACT "nakak_u21@yahoo.co.jp")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "na-trium-144 <${CPACK_PACKAGE_CONTACT}>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS yes)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "webcface")
set(CPACK_STRIP_FILES yes)
set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
include(CPack)
