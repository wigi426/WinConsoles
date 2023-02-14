

add_executable(test "GenericUseTest.cpp")
target_link_libraries(test PRIVATE WinConsolesLib)

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/src/)
target_link_directories(test PRIVATE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})