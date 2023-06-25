project('glad', 'c',
  version: '2.0'
)

inc = include_directories('include')

glad = static_library('glad',
  'src/gl.c',
  include_directories: inc
)

glad_dep = declare_dependency(
  include_directories: inc,
  link_with: glad
)
