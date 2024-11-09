tools_launcher = executable('webcface-launcher',
  sources: [
    'main.cc',
    'launcher.cc',
    'command.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    spdlog_dep,
    tiny_process_library_dep,
    tomlplusplus_dep,
  ],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build launcher', not is_disabler(tools_launcher), bool_yn: true)
