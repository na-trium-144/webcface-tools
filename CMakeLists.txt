cmake_minimum_required(VERSION 3.5)
project(webcface-tools
    LANGUAGES CXX
    VERSION 1.4.3
    HOMEPAGE_URL "https://github.com/na-trium-144/webcface-tools"
    DESCRIPTION "WebCFace Client Applications"
)
set(WEBCFACE_TOOLS_VERSION_SUFFIX "" CACHE STRING "version suffix")
option(WEBCFACE_TOOLS_INSTALL_SERVICE "generate install target for systemd service file" off)
set(WEBCFACE_TOOLS_LAUNCHER_CONFIG_PATH "/etc/webcface/webcface-launcher.toml" CACHE STRING "webcface-launcher config file path for systemd service")
option(WEBCFACE_USE_OPENCV "use opencv to enable webcface-cv-capture" ON)
option(WEBCFACE_USE_SDL2 "use sdl2 to enable webcface-joystick" ON)

set(TOOLS_VERSION ${PROJECT_VERSION})
if(NOT WEBCFACE_TOOLS_VERSION_SUFFIX STREQUAL "")
    set(TOOLS_VERSION "${PROJECT_VERSION}-${WEBCFACE_TOOLS_VERSION_SUFFIX}")
endif()

add_compile_definitions(TOOLS_VERSION="${TOOLS_VERSION}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-utf-8)
endif()

find_package(webcface 1.3 CONFIG)
if(WEBCFACE_USE_OPENCV)
    find_package(OpenCV REQUIRED)
endif()

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

if(WEBCFACE_USE_SDL2)
    include(FindPkgConfig)
    if(MSVC)
        message(STATUS "Fetching SDL2 ...")
        FetchContent_Declare(sdl2
            URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.2/SDL2-devel-2.30.2-VC.zip
        )
        FetchContent_Populate(sdl2)
        set(sdl2_DIR ${sdl2_SOURCE_DIR}/cmake)
        find_package(sdl2 REQUIRED)
        set(sdl2 SDL2::SDL2)
        configure_file(${sdl2_SOURCE_DIR}/lib/x64/SDL2.dll SDL2.dll COPYONLY)
        install(FILES SDL2.dll DESTINATION bin)
    else()
        pkg_search_module(sdl2 sdl2)
        if(sdl2_FOUND)
            add_library(sdl2 INTERFACE)
            target_link_directories(sdl2 INTERFACE ${sdl2_LIBDIR})
            target_link_libraries(sdl2 INTERFACE ${sdl2_LIBRARIES})
            target_include_directories(sdl2 INTERFACE ${sdl2_INCLUDE_DIRS})
            set(sdl2 sdl2)
        else()
            message(FATAL_ERROR "sdl2 not found (SDL2_FOUND = '${SDL2_FOUND}')")
        endif()
    endif()
endif()

foreach(dir launcher send cv-capture joystick)
    add_subdirectory(${dir})
endforeach()

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
