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

if(BUILD_TESTING)
  add_custom_target(irrun-test-cases ALL
    SOURCES "$<TARGET_PROPERTY:irrun-test-cases,IRS>"
    VERBATIM
  )

  set(IRRUN_TEST_CASES "")

  function(add_irrun_test_case name output)
    set(in  "${CMAKE_CURRENT_SOURCE_DIR}/${name}.c")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${name}.ll")
    add_custom_command(
      OUTPUT  "${out}"
      COMMAND clang -S -emit-llvm -o "${out}" "${in}"
      DEPENDS "${in}"
      COMMENT "generating IR for test case '${name}'"
      VERBATIM
    )
    set_property(TARGET irrun-test-cases APPEND PROPERTY IRS "${out}")

    add_test(
      NAME    irrun-test-${name}
      COMMAND $<TARGET_FILE:irrun-test> "${out}" ${ARGN}
    )
    set_property(
      TEST irrun-test-${name}
      PROPERTY
        PASS_REGULAR_EXPRESSION "${output}"
    )
  endfunction()

  add_irrun_test_case(strlen "12" "hello world!")

  add_irrun_test_case(prime  "not prime" "57")
endif()
