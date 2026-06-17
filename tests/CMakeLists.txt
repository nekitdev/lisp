add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/googletest ${CMAKE_CURRENT_BINARY_DIR}/gtest)

add_executable(test_lisp src/test_lisp.cpp)

target_link_libraries(test_lisp gtest gtest_main lisp_library)

target_include_directories(test_lisp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_test(NAME test_lisp COMMAND test_lisp)
