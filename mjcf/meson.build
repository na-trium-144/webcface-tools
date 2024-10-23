tools_mjcf = executable('webcface-mjcf',
  sources: [
    'main.cc',
    'body.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    pugixml_dep,
    spdlog_dep,
  ],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build mjcf', not is_disabler(tools_mjcf), bool_yn: true)
