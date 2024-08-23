tools_cv_capture = executable('webcface-cv-capture',
  sources: [
    'main.cc',
  ],
  dependencies: [
    webcface_dep,
    cli11_dep,
    spdlog_dep,
    opencv_core_dep,
    opencv_videoio_dep,
  ],
  install: true,
)
summary('Build cv-capture', not is_disabler(tools_cv_capture), bool_yn: true)
