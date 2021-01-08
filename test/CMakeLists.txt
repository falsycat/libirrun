add_executable(irrun-test)
target_compile_options(irrun-test
  PRIVATE "${IRRUN_CXX_COMPILER_WARNING_OPTIONS}"
)
target_include_directories(irrun-test
  PRIVATE .
)
target_sources(irrun-test
  PRIVATE main.c
)
target_link_libraries(irrun-test irrun)
