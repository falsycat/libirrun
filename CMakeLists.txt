cmake_minimum_required(VERSION 3.18)

project(irrun)

include(CTest)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(IRRUN_CXX_COMPILER_WARNING_OPTIONS
  "$<$<CXX_COMPILER_ID:Clang>:-Wall -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic>"
  "$<$<CXX_COMPILER_ID:GCC>:-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused>"
  "$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

find_package(LLVM REQUIRED)
llvm_map_components_to_libnames(IRRUN_LLVM_ENGINE_LIBRARIES
  ExecutionEngine
  Support
  irreader
  mcjit
  native
)

add_library(irrun)
target_compile_options(irrun
  PRIVATE "${IRRUN_CXX_COMPILER_WARNING_OPTIONS}"
)
target_sources(irrun
  PRIVATE irrun.cc
  PUBLIC  irrun.h
)
target_link_libraries(irrun
  ${IRRUN_LLVM_ENGINE_LIBRARIES}
)

add_subdirectory(test)
