if(WEBCFACE_USE_OPENCV)
    add_executable(webcface-cv-capture main.cc)
    target_link_libraries(webcface-cv-capture
        webcface::webcface
        CLI11
        opencv_core
        opencv_videoio
    )
    set_target_properties(webcface-cv-capture PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
    )
    install(TARGETS webcface-cv-capture RUNTIME DESTINATION bin)
else()
    message(STATUS "Skipping target webcface-cv-capture")
endif()
