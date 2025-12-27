cmake_minimum_required(VERSION 3.24)

project(hybrid_search VERSION 0.1.0 LANGUAGES CXX)

# ---- Options ----
option(HYBRID_SEARCH_BUILD_TESTS "Build hybrid_search tests" ON)
option(HYBRID_SEARCH_BUILD_BENCH "Build hybrid_search benchmarks" OFF)

# ---- Library target (header-only) ----
add_library(hybrid_search INTERFACE)
add_library(hybrid_search::hybrid_search ALIAS hybrid_search)

target_include_directories(hybrid_search
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(hybrid_search INTERFACE cxx_std_20)

if (MSVC)
  target_compile_options(hybrid_search INTERFACE /W4)
else()
  target_compile_options(hybrid_search INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ---- Tests ----
if (HYBRID_SEARCH_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ---- Benchmarks ----
if (HYBRID_SEARCH_BUILD_BENCH)
  add_subdirectory(bench)
endif()

# ---- Install / export ----
include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS hybrid_search EXPORT hybrid_searchTargets)

install(EXPORT hybrid_searchTargets
  FILE hybrid_searchTargets.cmake
  NAMESPACE hybrid_search::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hybrid_search
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/hybrid_searchConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hybrid_searchConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hybrid_searchConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hybrid_search
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/hybrid_searchConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/hybrid_searchConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hybrid_search
)
