cmake_minimum_required(VERSION 3.20)

project(quant-backend
    VERSION 0.1.0
    DESCRIPTION "Native code generation backend for the Quant compiler"
    LANGUAGES CXX)

include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(QUANT_BACKEND_TOP_LEVEL ON)
else()
  set(QUANT_BACKEND_TOP_LEVEL OFF)
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(QUANT_BACKEND_INSTALL
       "Generate install rules for quant-backend" ${QUANT_BACKEND_TOP_LEVEL})
option(QUANT_BACKEND_WARNINGS_AS_ERRORS
       "Treat warnings as errors" OFF)

# ---------------------------------------------------------------------------
# Targets compiled into the library
#
# The same spelling is used by the Quant compiler, so a parent project can
# pass its QUANT_BACKENDS list straight through to the backend.
# ---------------------------------------------------------------------------

set(QUANT_BACKEND_KNOWN_TARGETS
    "x86_64-linux;aarch64-linux;x86_64-windows;aarch64-zeropoint")

if(NOT DEFINED QUANT_BACKENDS OR QUANT_BACKENDS STREQUAL "")
  set(QUANT_BACKENDS "${QUANT_BACKEND_KNOWN_TARGETS}")
endif()

foreach(_backend IN LISTS QUANT_BACKENDS)
  if(NOT _backend IN_LIST QUANT_BACKEND_KNOWN_TARGETS)
    message(FATAL_ERROR
            "Unknown backend '${_backend}' in QUANT_BACKENDS. "
            "Known backends: ${QUANT_BACKEND_KNOWN_TARGETS}")
  endif()
endforeach()

# Default target for command lines without --target. Set this for builds whose
# host backend is not compiled in (e.g. a ZeroPoint-only compiler).
set(QUANT_DEFAULT_TARGET "" CACHE STRING
    "Default --target when none is passed (must be one of QUANT_BACKENDS)")

message(STATUS "quant-backend targets: ${QUANT_BACKENDS}")

set(QUANT_HAS_X86_64 OFF)
set(QUANT_HAS_AARCH64 OFF)
set(QUANT_HAS_ELF OFF)
set(QUANT_HAS_PE OFF)

set(QUANT_BACKEND_DEFINES "")

foreach(_backend IN LISTS QUANT_BACKENDS)
  if(_backend MATCHES "^x86_64")
    set(QUANT_HAS_X86_64 ON)
  elseif(_backend MATCHES "^aarch64")
    set(QUANT_HAS_AARCH64 ON)
  endif()

  if(_backend MATCHES "-windows$")
    set(QUANT_HAS_PE ON)
  else()
    set(QUANT_HAS_ELF ON)
  endif()

  string(REPLACE "-" "_" _target_key "${_backend}")
  list(APPEND QUANT_BACKEND_DEFINES "QUANT_TARGET_${_target_key}=1")
endforeach()

foreach(_flag QUANT_HAS_X86_64 QUANT_HAS_AARCH64 QUANT_HAS_ELF QUANT_HAS_PE)
  if(${_flag})
    list(APPEND QUANT_BACKEND_DEFINES ${_flag})
  endif()
endforeach()

if(QUANT_DEFAULT_TARGET)
  if(NOT QUANT_DEFAULT_TARGET IN_LIST QUANT_BACKEND_KNOWN_TARGETS)
    message(FATAL_ERROR
            "Unknown backend '${QUANT_DEFAULT_TARGET}' in QUANT_DEFAULT_TARGET. "
            "Known backends: ${QUANT_BACKEND_KNOWN_TARGETS}")
  endif()
  if(NOT QUANT_DEFAULT_TARGET IN_LIST QUANT_BACKENDS)
    message(FATAL_ERROR
            "Default target '${QUANT_DEFAULT_TARGET}' must also be enabled in "
            "QUANT_BACKENDS (${QUANT_BACKENDS})")
  endif()
  list(APPEND QUANT_BACKEND_DEFINES
         "QUANT_DEFAULT_TARGET=\"${QUANT_DEFAULT_TARGET}\"")
endif()

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

set(QUANT_BACKEND_SOURCES
    src/backend/backend_error.cpp
    src/backend/code_emitter.cpp
    src/backend/native_backend.cpp
    src/backend/registry.cpp
    src/backend/symbol_table.cpp
    src/qu-ir/dump.cpp
)

foreach(_backend IN LISTS QUANT_BACKENDS)
  string(REPLACE "-" "_" _target_source "${_backend}")
  list(APPEND QUANT_BACKEND_SOURCES
         "src/backend/targets/${_target_source}.cpp")
endforeach()

if(QUANT_HAS_X86_64)
  list(APPEND QUANT_BACKEND_SOURCES
         src/backend/isel.cpp
         src/backend/x86_64.cpp)
endif()

if(QUANT_HAS_AARCH64)
  list(APPEND QUANT_BACKEND_SOURCES
         src/backend/aarch64_isel.cpp
         src/backend/aarch_64.cpp)
endif()

if(QUANT_HAS_ELF)
  list(APPEND QUANT_BACKEND_SOURCES src/backend/elf_writer.cpp)
endif()

if(QUANT_HAS_PE)
  list(APPEND QUANT_BACKEND_SOURCES src/backend/pe_writer.cpp)
endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(quant-backend ${QUANT_BACKEND_SOURCES})
add_library(quant::backend ALIAS quant-backend)

set_target_properties(quant-backend PROPERTIES
    EXPORT_NAME backend
    POSITION_INDEPENDENT_CODE ON
    CXX_EXTENSIONS OFF
)

target_compile_features(quant-backend PUBLIC cxx_std_20)

target_include_directories(quant-backend
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
)

target_compile_definitions(quant-backend PRIVATE ${QUANT_BACKEND_DEFINES})

if(MSVC)
  target_compile_options(quant-backend PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
        /FS
    )
  if(QUANT_BACKEND_WARNINGS_AS_ERRORS)
    target_compile_options(quant-backend PRIVATE /WX)
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(quant-backend PRIVATE
        -Wall -Wextra -Wpedantic
        -Wshadow -Wconversion -Wsign-conversion
        -Wnon-virtual-dtor -Woverloaded-virtual
    )
  if(QUANT_BACKEND_WARNINGS_AS_ERRORS)
    target_compile_options(quant-backend PRIVATE -Werror)
  endif()
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------

if(QUANT_BACKEND_INSTALL)
  install(TARGETS quant-backend
        EXPORT quant-backend-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

  install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(EXPORT quant-backend-targets
        FILE quant-backend-targets.cmake
        NAMESPACE quant::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quant-backend
    )

  include(CMakePackageConfigHelpers)

  configure_package_config_file(
        cmake/quant-backend-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/quant-backend-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quant-backend
    )

  write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/quant-backend-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

  install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/quant-backend-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/quant-backend-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quant-backend
    )
endif()
