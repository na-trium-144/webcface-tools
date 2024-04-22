add_executable(webcface-send main.cc)
target_link_libraries(webcface-send
    webcface::webcface
    CLI11
)
set_target_properties(webcface-send PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
)
install(TARGETS webcface-send RUNTIME DESTINATION bin)
