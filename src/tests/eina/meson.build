eina_test_src = files(
'eina_suite.c',
'eina_suite.h',
'eina_test_win32_synchronization',
)


eina_test_exe = executable('eina_suite',
  include_directories : config_dir,
  sources : eina_test_src,
  dependencies: [m, check, eina],
  c_args : [
  '-DTESTS_WD="`pwd`"',
  '-DTESTS_BUILD_DIR="'+meson.current_build_dir()+'"',
  '-DTESTS_SRC_DIR="'+meson.current_source_dir()+'"']
)

test('eina', eina_test_exe,
  env : test_env,
)
