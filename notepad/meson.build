tools_notepad = executable('webcface-notepad',
  sources: [
    'main.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    spdlog_dep,
  ],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build notepad', not is_disabler(tools_notepad), bool_yn: true)
