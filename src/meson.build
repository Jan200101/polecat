polecat_deps = [libarchive_dep, libcurl_dep, libjson_c_dep]

polecat_sources = files(
  'common.c',
  'common.h',
  'config.c',
  'config.h',
  'dxvk.c',
  'dxvk.h',
  'lutris.c',
  'lutris.h',
  'main.c',
  'main.h',
  'net.c',
  'net.h',
  'tar.c',
  'tar.h',
  'wine.c',
  'wine.h',
)

executable(
  'polecat',
  polecat_sources,
  dependencies : polecat_deps,
  include_directories : conf_inc,
  install : true,
)
