include(FetchContent)

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(hybrid_search_tests
    test.cpp
)

target_link_libraries(hybrid_search_tests
  PRIVATE
    hybrid_search::hybrid_search
    GTest::gtest_main
)


target_compile_features(hybrid_search_tests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(hybrid_search_tests)
