project(
  'polecat',
  'c',
  default_options : [
    'buildtype=debugoptimized',
  ],
  license : 'MIT',
  version : '0.1.5',
)

libarchive_dep = dependency('libarchive')
libcurl_dep = dependency('libcurl')
libjson_c_dep = dependency('json-c')

conf_data = configuration_data()
conf_data.set_quoted('NAME', meson.project_name())
conf_data.set_quoted('VERSION', meson.project_version())
if get_option('release')
  conf_data.set('NDEBUG', true)
else
  conf_data.set('DEBUG', true)
endif
configure_file(output : 'polecat-config.h', configuration : conf_data)

conf_inc = include_directories('.')

run_target(
  'loc',
  command : [
    'sh',
    '-c',
    'find src'
      + ' -type f'
      + ' -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp"'
      + ' | xargs wc -l',
  ],
)

subdir('src')
