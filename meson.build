project('glsandbox', ['c', 'cpp'],
  meson_version: '>=1.0.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2'
  ]
)

libglfw = dependency('glfw3', required: true)
libOpenGL = dependency('opengl')
libglm = dependency('glm', required: true)

glad_proj = subproject('glad')
glad = glad_proj.get_variable('glad_dep')

if not libOpenGL.found()
  libOpenGL = dependency('gl', required: true)
endif

common_srcs = [
  files('utility.cpp')
]
common_include_dirs = [
  include_directories('include')
]
common_deps = [libOpenGL, libglfw, libglm, glad]

executable('hello_world',
  'hello_world/main.cpp',
  common_srcs,
  dependencies: common_deps,
  include_directories: common_include_dirs
)


executable('postprocessing',
  'postprocessing/main.cpp',
  common_srcs,
  dependencies: common_deps,
  include_directories: common_include_dirs,
  build_by_default: false
)

