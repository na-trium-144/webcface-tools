project('webcface-tools', 'cpp',
  version: '2.1.0',
  license: 'MIT',
  meson_version: '>=1.1',
  default_options: [
    'buildtype=release',
    'warning_level=3',
    # 'werror=true',
    'cpp_std=gnu++17,c++17', # spdlogがgnu++必要、webcfaceは17以上必須
  ],
)
cxx = meson.get_compiler('cpp')

tools_version_str = meson.project_version()
if get_option('version_suffix') == 'git'
  git = find_program('git', required: false)
  if git.found()
    git_describe = run_command(git, 'describe', '--tags', check: false)
    if git_describe.returncode() == 0
      tools_version_str = git_describe.stdout().strip('v').strip()
    else
      warning('Failed to get version from git describe:\n' + git_describe.stderr())
    endif
  endif
elif get_option('version_suffix') != ''
  tools_version_str = meson.project_version() + '-' + get_option('version_suffix')
endif
summary('Version', tools_version_str)
add_project_arguments(
  '-DTOOLS_VERSION="' + tools_version_str + '"',
  language: ['cpp'],
)

relative_rpath = ''
if host_machine.system() == 'linux'
  relative_rpath = '$ORIGIN' / '..' / get_option('libdir')
elif host_machine.system() == 'darwin'
  relative_rpath = '@loader_path' / '..' / get_option('libdir')
endif

if cxx.get_argument_syntax() == 'msvc'
  add_project_arguments(
    '/utf-8',
    language: ['cpp'],
  )
endif

webcface_dep = dependency('webcface', version: '>=2.4.0')
spdlog_dep = dependency('spdlog')
tiny_process_library_dep = dependency('tiny-process-library')
tomlplusplus_dep = dependency('tomlplusplus',
  default_options: [
    'compile_library=false',
  ],
)
cli11_dep = dependency('cli11', 'CLI11')
ftxui_screen_dep = dependency('ftxui',
  method: 'cmake',
  modules: ['ftxui::screen'],
  fallback: ['ftxui', 'screen_dep'],
)
ftxui_dom_dep = dependency('ftxui',
  method: 'cmake',
  modules: ['ftxui::dom'],
  fallback: ['ftxui', 'dom_dep'],
)
ftxui_component_dep = dependency('ftxui',
  method: 'cmake',
  modules: ['ftxui::component'],
  fallback: ['ftxui', 'component_dep'],
)

subdir('launcher')
subdir('send')
subdir('tui')
subdir('ls')

if host_machine.system() == 'linux'
  conf_data = configuration_data({
    'WEBCFACE_TOOLS_LAUNCHER_CONFIG_PATH': get_option('launcher_config_path'),
  })
  configure_file(
    input: 'scripts' / 'webcface-launcher.service.in',
    output: 'webcface-launcher.service',
    configuration: conf_data,
    install: true,
    install_dir: 'lib' / 'systemd' / 'system', # lib/x86_64-linux-gnu/ ではない
  )
endif
install_data(['README.md', 'LICENSE'],
  install_dir: get_option('datadir') / 'doc' / 'webcface-tools',
)
