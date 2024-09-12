add_executable(
        TestOrderBook
        Test.cpp
)
target_link_libraries(
        TestOrderBook
        GTest::gtest_main
)
gtest_discover_tests(TestOrderBook)

set_target_properties(
        TestOrderBook
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/test"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/test"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)
