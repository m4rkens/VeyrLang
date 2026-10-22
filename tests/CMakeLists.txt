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

add_executable(source_tests source/source_manager_test.cpp)
veyr_target_policy(source_tests)
target_link_libraries(source_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(source_tests DISCOVERY_MODE PRE_TEST)

add_executable(diagnostic_tests
  diagnostics/diagnostic_test.cpp
  diagnostics/plain_text_renderer_test.cpp)
veyr_target_policy(diagnostic_tests)
target_link_libraries(diagnostic_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(diagnostic_tests DISCOVERY_MODE PRE_TEST)

add_executable(lexer_tests
  lexing/token_test.cpp
  lexing/lexer_test.cpp)
veyr_target_policy(lexer_tests)
target_link_libraries(lexer_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(lexer_tests DISCOVERY_MODE PRE_TEST)

add_executable(parser_tests
    parsing/parser_test.cpp
    parsing/parser_integration_test.cpp)
veyr_target_policy(parser_tests)
target_link_libraries(parser_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(parser_tests DISCOVERY_MODE PRE_TEST)

add_executable(ast_tests ast/smoke_entry_test.cpp)
veyr_target_policy(ast_tests)
target_link_libraries(ast_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(ast_tests DISCOVERY_MODE PRE_TEST)

add_executable(semantic_tests
    semantics/smoke_validator_test.cpp
    semantics/smoke_validator_integration_test.cpp)
veyr_target_policy(semantic_tests)
target_link_libraries(semantic_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(semantic_tests DISCOVERY_MODE PRE_TEST)

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

# Synthetic Windows consumer fixtures run on either host without a compiler,
# package operations or native acceptance. Every rejection checks the CRT diagnostic.
function(veyr_add_consumer_crt_fixture name configuration runtime accepted)
  set(fixture "${CMAKE_CURRENT_BINARY_DIR}/policy/ConsumerCRT/${name}")
  file(MAKE_DIRECTORY "${fixture}")
  set(module "${PROJECT_SOURCE_DIR}/cmake/VeyrTargets.cmake")
  if(configuration STREQUAL "Debug")
    set(expected MultiThreadedDebugDLL)
  else()
    set(expected MultiThreadedDLL)
  endif()
  string(CONFIGURE [=[
cmake_minimum_required(VERSION 3.31)
set(WIN32 TRUE)
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT MSVC)
set(CMAKE_BUILD_TYPE "@configuration@")
set(CMAKE_MSVC_RUNTIME_LIBRARY [==[@runtime@]==])
include("@module@")
if(NOT VEYR_MSVC_RUNTIME STREQUAL "@expected@" OR
   NOT CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "@expected@")
  message(FATAL_ERROR "Selected consumer CRT was not retained")
endif()
]=] fixture_contents @ONLY)
  file(WRITE "${fixture}/input.cmake" "${fixture_contents}")
  string(CONFIGURE [=[
cmake_minimum_required(VERSION 3.31)
cmake_policy(GET CMP0012 boolean_policy)
if(NOT boolean_policy STREQUAL "NEW")
  message(FATAL_ERROR "Consumer CRT fixture requires deterministic TRUE/FALSE handling")
endif()
execute_process(COMMAND "@CMAKE_COMMAND@" -P "@fixture@/input.cmake"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
if(@accepted@)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Expected @name@ acceptance; actual=${result}: ${output}${errors}")
  endif()
elseif(result EQUAL 0 OR NOT "${output}${errors}" MATCHES "Consumer CRT" OR
       NOT "${output}${errors}" MATCHES "differs from")
  message(FATAL_ERROR "Expected @name@ CRT rejection; actual=${result}: ${output}${errors}")
endif()
message(STATUS "Expected @name@ CRT policy result observed: ${result}")
]=] runner_contents @ONLY)
  file(WRITE "${fixture}/check.cmake" "${runner_contents}")
  add_test(NAME "ConsumerCRT.${name}" COMMAND "${CMAKE_COMMAND}" -P "${fixture}/check.cmake")
endfunction()

veyr_add_consumer_crt_fixture(DebugLiteral Debug MultiThreadedDebugDLL TRUE)
veyr_add_consumer_crt_fixture(ReleaseLiteral Release MultiThreadedDLL TRUE)
veyr_add_consumer_crt_fixture(DebugConan Debug
  "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>" TRUE)
veyr_add_consumer_crt_fixture(ReleaseConan Release
  "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL>" TRUE)
veyr_add_consumer_crt_fixture(DebugCombinedConan Debug
  "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL>" TRUE)
foreach(configuration Debug Release)
  veyr_add_consumer_crt_fixture(${configuration}Canonical "${configuration}"
    "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" TRUE)
endforeach()

veyr_add_consumer_crt_fixture(DebugStatic Debug MultiThreadedDebug FALSE)
veyr_add_consumer_crt_fixture(ReleaseStatic Release MultiThreaded FALSE)
veyr_add_consumer_crt_fixture(DebugOppositeLiteral Debug MultiThreadedDLL FALSE)
veyr_add_consumer_crt_fixture(ReleaseOppositeLiteral Release MultiThreadedDebugDLL FALSE)
veyr_add_consumer_crt_fixture(ReleaseDebugOnlyExpression Release
  "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>" FALSE)
foreach(configuration Debug Release)
  veyr_add_consumer_crt_fixture(${configuration}OppositeExpression "${configuration}"
    "$<$<CONFIG:Release>:MultiThreadedDebugDLL>$<$<CONFIG:Debug>:MultiThreadedDLL>" FALSE)
  veyr_add_consumer_crt_fixture(${configuration}UnsupportedRuntime "${configuration}"
    UnsupportedCRT FALSE)
  veyr_add_consumer_crt_fixture(${configuration}UnsupportedExpression "${configuration}"
    "$<IF:$<CONFIG:Debug>,MultiThreadedDebugDLL,MultiThreadedDLL>" FALSE)
  veyr_add_consumer_crt_fixture(${configuration}MalformedExpression "${configuration}"
    "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL" FALSE)
  veyr_add_consumer_crt_fixture(${configuration}AmbiguousExpression "${configuration}"
    "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Debug>:MultiThreadedDLL>" FALSE)
endforeach()

add_test(NAME include-policy
  COMMAND "${VEYR_PYTHON}" -B "${PROJECT_SOURCE_DIR}/scripts/check_includes.py"
    --root "${PROJECT_SOURCE_DIR}")
