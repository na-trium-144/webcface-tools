tools_ls = executable('webcface-ls',
  sources: [
    'main.cc',
  ],
  dependencies: [
    webcface_dep,
    spdlog_dep,
    cli11_dep,
    ftxui_screen_dep,
    ftxui_dom_dep,
  ],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build ls', not is_disabler(tools_ls), bool_yn: true)
