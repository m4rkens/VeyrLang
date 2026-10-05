find_package(GTest 1.18.0 EXACT CONFIG REQUIRED)
foreach(_target GTest::gtest_main GTest::gmock)
  if(NOT TARGET ${_target})
    message(FATAL_ERROR "Conan GoogleTest/GoogleMock must export ${_target}")
  endif()
endforeach()
include(GoogleTest)

add_executable(foundation_tests infrastructure/build_info_test.cpp)
veyr_target_policy(foundation_tests)
target_link_libraries(foundation_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(foundation_tests DISCOVERY_MODE PRE_TEST)

add_executable(llvm_link_tests infrastructure/llvm_link_test.cpp)
veyr_target_policy(llvm_link_tests)
target_link_libraries(llvm_link_tests PRIVATE veyr_llvm_support GTest::gtest_main)
gtest_discover_tests(llvm_link_tests DISCOVERY_MODE PRE_TEST)

add_test(NAME Driver.Version COMMAND veyrc --version)
set_tests_properties(Driver.Version PROPERTIES PASS_REGULAR_EXPRESSION "veyrc Veyr ${PROJECT_VERSION}")
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/check_driver_arguments.cmake" CONTENT [=[
execute_process(COMMAND "$<TARGET_FILE:veyrc>" --unsupported
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
if(NOT result EQUAL 2 OR NOT errors MATCHES "Unsupported arguments. Usage: veyrc --version")
  message(FATAL_ERROR "Expected unsupported-argument exit 2; actual=${result}: ${output}${errors}")
endif()
]=])
add_test(NAME Driver.UnsupportedArguments COMMAND "${CMAKE_COMMAND}" -P
  "${CMAKE_CURRENT_BINARY_DIR}/check_driver_arguments.cmake")

# Synthetic export fixtures exercise policy errors, not platform readiness.
# Every negative must fail with its specific diagnostic; no retries/acquisition.
function(veyr_add_policy_negative name expected overrides)
  set(fixture "${CMAKE_CURRENT_BINARY_DIR}/policy/${name}")
  file(MAKE_DIRECTORY "${fixture}/include/llvm/Config")
  file(WRITE "${fixture}/include/llvm/Config/abi-breaking.h"
    "#define LLVM_ENABLE_ABI_BREAKING_CHECKS 0\n")
  set(module "${PROJECT_SOURCE_DIR}/cmake/VeyrLLVM.cmake")
  set(configuration "${CMAKE_BUILD_TYPE}")
  set(runtime "${CMAKE_MSVC_RUNTIME_LIBRARY}")
  if(WIN32)
    set(triple "x86_64-pc-windows-msvc")
    set(windows TRUE)
  else()
    set(triple "x86_64-unknown-linux-gnu")
    set(windows FALSE)
  endif()
  string(CONFIGURE [=[
cmake_minimum_required(VERSION 3.31)
project(VeyrPolicyFixture LANGUAGES NONE)
include("@module@")
add_library(LLVMSupport STATIC IMPORTED)
set(WIN32 @windows@)
set(CMAKE_BUILD_TYPE "@configuration@")
set(CMAKE_MSVC_RUNTIME_LIBRARY "@runtime@")
set(LLVM_PACKAGE_VERSION 23.1.2)
set(LLVM_BUILD_TYPE Release)
set(LLVM_ABI_BREAKING_CHECKS FORCE_OFF)
set(LLVM_ENABLE_SHARED_LIBS OFF)
set(LLVM_TARGETS_TO_BUILD X86)
set(LLVM_HOST_TRIPLE "@triple@")
set(LLVM_TARGET_TRIPLE "@triple@")
set(LLVM_DEFINITIONS "-D_GLIBCXX_USE_CXX11_ABI=1")
set(LLVM_INCLUDE_DIRS "@fixture@/include")
@overrides@
veyr_validate_llvm_exports("@runtime@")
]=] fixture_contents @ONLY)
  file(WRITE "${fixture}/CMakeLists.txt" "${fixture_contents}")
  string(CONFIGURE [=[
execute_process(COMMAND "@CMAKE_COMMAND@" -S "@fixture@" -B "@fixture@/build" -G Ninja
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
if(result EQUAL 0 OR NOT "${output}${errors}" MATCHES "@expected@")
  message(FATAL_ERROR "Expected @name@ rejection '@expected@'; actual=${result}: ${output}${errors}")
endif()
message(STATUS "Expected @name@ rejection observed: ${result}")
]=] runner_contents @ONLY)
  file(WRITE "${fixture}/check.cmake" "${runner_contents}")
  add_test(NAME "LLVMPolicy.${name}" COMMAND "${CMAKE_COMMAND}" -P "${fixture}/check.cmake")
endfunction()

veyr_add_policy_negative(WrongVersion "exact LLVM 23.1.2 required" "set(LLVM_PACKAGE_VERSION 23.1.1)")
veyr_add_policy_negative(WrongConfiguration "static Release LLVM" "set(LLVM_BUILD_TYPE Debug)")
if(WIN32)
  set(_triple_diagnostic "native .* x86_64 LLVM triples required")
else()
  set(_triple_diagnostic "Veyr LLVM policy: native Linux x86_64 GNU LLVM triples required")
endif()
veyr_add_policy_negative(WrongTriple "${_triple_diagnostic}" "set(LLVM_HOST_TRIPLE aarch64-unknown-linux-gnu)")
if(WIN32)
  veyr_add_policy_negative(WrongRuntime "package CRT" "set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)")
else()
  veyr_add_policy_negative(WrongRuntime "package requires the reviewed" "set(LLVM_DEFINITIONS -D_GLIBCXX_USE_CXX11_ABI=0)")
endif()
veyr_add_policy_negative(WrongHeaderABI "package header ABI checks must be disabled"
  "file(WRITE \"\${LLVM_INCLUDE_DIRS}/llvm/Config/abi-breaking.h\" \"#define LLVM_ENABLE_ABI_BREAKING_CHECKS 1\\n\")")

add_test(NAME include-policy
  COMMAND "${VEYR_PYTHON}" -B "${PROJECT_SOURCE_DIR}/scripts/check_includes.py"
    --root "${PROJECT_SOURCE_DIR}")
