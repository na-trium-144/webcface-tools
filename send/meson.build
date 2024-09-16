tools_send = executable('webcface-send',
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
summary('Build send', not is_disabler(tools_send), bool_yn: true)
