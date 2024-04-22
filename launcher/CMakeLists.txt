add_executable(webcface-launcher main.cc launcher.cc)
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
