add_executable(webcface-tui main.cc value.cc text.cc view.cc)
target_link_libraries(webcface-tui
    webcface::webcface
    CLI11
    ftxui::screen
    ftxui::dom
    ftxui::component
)
set_target_properties(webcface-tui PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
)
install(TARGETS webcface-tui RUNTIME DESTINATION bin)
