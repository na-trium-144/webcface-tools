add_executable(webcface-ls main.cc)
target_link_libraries(webcface-ls
    webcface::webcface
    CLI11
    ftxui::screen
    ftxui::dom
)
set_target_properties(webcface-ls PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/;$ORIGIN/;@loader_path/../lib/;@loader_path/"
)
install(TARGETS webcface-ls RUNTIME DESTINATION bin)
