if(WEBCFACE_USE_SDL2)
    add_executable(webcface-joystick main.cc)
    target_link_libraries(webcface-joystick
        webcface::webcface
        CLI11
        ${sdl2}
    )
    set_target_properties(webcface-joystick PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
    )
    install(TARGETS webcface-joystick RUNTIME DESTINATION bin)
else()
    message(STATUS "Skipping target webcface-joystick")
endif()
