cmake_minimum_required(VERSION 3.5)
project(webcface-tools
    LANGUAGES CXX
    VERSION 1.4.2
    HOMEPAGE_URL "https://github.com/na-trium-144/webcface-tools"
    DESCRIPTION "WebCFace Client Applications"
)
set(WEBCFACE_TOOLS_VERSION_SUFFIX "" CACHE STRING "version suffix")
option(WEBCFACE_TOOLS_INSTALL_SERVICE "generate install target for systemd service file" off)
set(WEBCFACE_TOOLS_LAUNCHER_CONFIG_PATH "/etc/webcface/webcface-launcher.toml" CACHE STRING "webcface-launcher config file path for systemd service")

set(TOOLS_VERSION ${PROJECT_VERSION})
if(NOT WEBCFACE_TOOLS_VERSION_SUFFIX STREQUAL "")
    set(TOOLS_VERSION "${PROJECT_VERSION}-${WEBCFACE_TOOLS_VERSION_SUFFIX}")
endif()

add_compile_definitions(TOOLS_VERSION="${TOOLS_VERSION}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-utf-8)
endif()

find_package(webcface CONFIG)

include(FetchContent)

FetchContent_Declare(tiny-process-library
    GIT_REPOSITORY https://gitlab.com/eidheim/tiny-process-library.git
    GIT_TAG master
)
FetchContent_MakeAvailable(tiny-process-library)

FetchContent_Declare(tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11)

find_package(OpenCV QUIET)

add_executable(webcface-launcher launcher/main.cc launcher/launcher.cc)
target_link_libraries(webcface-launcher
    webcface::webcface
    tiny-process-library
    tomlplusplus_tomlplusplus
    CLI11
)
set_target_properties(webcface-launcher PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
)
install(TARGETS webcface-launcher RUNTIME DESTINATION bin)

add_executable(webcface-send send/main.cc)
target_link_libraries(webcface-send
    webcface::webcface
    CLI11
)
set_target_properties(webcface-send PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
)
install(TARGETS webcface-send RUNTIME DESTINATION bin)

if(OpenCV_FOUND)
    add_executable(webcface-cv-capture cv-capture/main.cc)
    target_link_libraries(webcface-cv-capture
        webcface::webcface
        CLI11
        ${OpenCV_LIBS}
    )
    set_target_properties(webcface-cv-capture PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
    )
    install(TARGETS webcface-cv-capture RUNTIME DESTINATION bin)
else()
    message(STATUS "OpenCV Not Found, Skipping target webcface-cv-capture")
endif()

if(WEBCFACE_TOOLS_INSTALL_SERVICE)
    configure_file(
        cmake/webcface-launcher.service.in
        webcface-launcher.service
        @ONLY
    )
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/webcface-launcher.service
        DESTINATION lib/systemd/system
    )
endif()

install(FILES
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    ${CMAKE_CURRENT_LIST_DIR}/LICENSE
    DESTINATION share/webcface/tools
)

install(FILES
    ${tiny-process-library_SOURCE_DIR}/LICENSE
    DESTINATION share/webcface/3rd_party/tiny-process-library
)
install(FILES
    ${tomlplusplus_SOURCE_DIR}/LICENSE
    DESTINATION share/webcface/3rd_party/tomlplusplus
)

set(CPACK_VERBATIM_VARIABLES yes)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_VERSION ${TOOLS_VERSION})
set(CPACK_PACKAGE_CONTACT "na-trium-144@users.noreply.github.com")
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
