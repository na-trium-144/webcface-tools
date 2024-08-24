tools_tui = executable('webcface-tui',
  sources: [
    'main.cc',
    'value.cc',
    'text.cc',
    'view.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    ftxui_screen_dep,
    ftxui_dom_dep,
    ftxui_component_dep,
  ],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build tui', not is_disabler(tools_tui), bool_yn: true)
