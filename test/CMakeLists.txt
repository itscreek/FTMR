set(TEST_SRC test_directed_graph.cpp test_multitree_recolorability.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_test ${TEST_SRC})

target_include_directories(unit_test PRIVATE ${FTMR_SRC_DIR})

target_link_libraries(unit_test PRIVATE ftmr gtest_main)

include(GoogleTest)

gtest_discover_tests(unit_test)