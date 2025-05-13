
add_executable(unit_tests unit_tests.cpp)
target_include_directories(unit_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(unit_tests PUBLIC Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)

add_executable(integration_tests integration_tests.cpp)
target_include_directories(integration_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(integration_tests PUBLIC Catch2::Catch2WithMain)
catch_discover_tests(integration_tests)

add_executable(mock_file_io_tests mock_file_io_tests.cpp)
target_include_directories(mock_file_io_tests PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(mock_file_io_tests PUBLIC Catch2::Catch2WithMain)
catch_discover_tests(mock_file_io_tests)

