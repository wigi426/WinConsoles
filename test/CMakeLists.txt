

add_executable(test "test.cpp")

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/src/)
target_link_directories(test PRIVATE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})
target_link_libraries(test WinConsoles)