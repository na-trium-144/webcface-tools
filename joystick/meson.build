tools_joystick = executable('webcface-joystick',
  sources: [
    'main.cc',
    'list.cc',
    'select.cc',
    'joystick.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    spdlog_dep,
    sdl2_dep,
    # sdl2main_dep,
  ],
  cpp_args: ['-DSDL_MAIN_HANDLED'],
  install: true,
  install_rpath: relative_rpath,
)
summary('Build joystick', not is_disabler(tools_joystick), bool_yn: true)
