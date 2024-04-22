if(OpenCV_FOUND)
    add_executable(webcface-cv-capture main.cc)
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
