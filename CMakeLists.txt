cmake_minimum_required(VERSION 3.31)

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
  message(FATAL_ERROR "Veyr development requires the Ninja presets through scripts/dev.py configure")
endif()
if(NOT IS_ABSOLUTE "$ENV{VEYR_BUILD_DIR}" OR
   NOT IS_ABSOLUTE "$ENV{VEYR_CONAN_TOOLCHAIN}" OR
   NOT EXISTS "$ENV{VEYR_CONAN_TOOLCHAIN}")
  message(FATAL_ERROR "Missing selected build/toolchain paths; run explicit setup, then scripts/dev.py configure")
endif()
file(REAL_PATH "$ENV{VEYR_BUILD_DIR}" _selected_build_dir)
file(REAL_PATH "${CMAKE_BINARY_DIR}" _actual_build_dir)
if(NOT _selected_build_dir STREQUAL _actual_build_dir)
  message(FATAL_ERROR "Build directory differs from the selected developer configuration")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Veyr VERSION 0.1.0 LANGUAGES C CXX)
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
  message(FATAL_ERROR "Select a native Debug or Release development configuration")
endif()
if(CMAKE_CROSSCOMPILING OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR
   NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" OR
   NOT CMAKE_SYSTEM_NAME MATCHES "^(Linux|Windows)$")
  message(FATAL_ERROR "Only native Windows/Linux x86_64 is supported; macOS is deferred")
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "Use the reviewed native Clang Conan profile")
endif()

set(VEYR_PYTHON "$ENV{VEYR_PYTHON}" CACHE FILEPATH "Reviewed developer Python")
if(NOT IS_ABSOLUTE "${VEYR_PYTHON}" OR NOT EXISTS "${VEYR_PYTHON}")
  message(FATAL_ERROR "VEYR_PYTHON must select the reviewed interpreter through scripts/dev.py")
endif()
set(VEYR_LLVM_PROVIDER "managed" CACHE STRING "LLVM provider: managed, system or custom")
set_property(CACHE VEYR_LLVM_PROVIDER PROPERTY STRINGS managed system custom)
set(VEYR_LLVM_DIR "" CACHE PATH "Explicit external native LLVM configuration directory")

include(cmake/VeyrTargets.cmake)
include(cmake/VeyrLLVM.cmake)
veyr_configure_llvm()

add_library(veyrcore STATIC src/core/build_info.cpp)
veyr_target_policy(veyrcore)
target_include_directories(veyrcore PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(veyrcore PRIVATE VEYR_BUILD_VERSION="${PROJECT_VERSION}")

add_executable(veyrc src/driver/main.cpp)
veyr_target_policy(veyrc)
target_link_libraries(veyrc PRIVATE veyrcore)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
