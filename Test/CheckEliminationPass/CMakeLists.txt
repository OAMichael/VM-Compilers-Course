cmake_minimum_required(VERSION 3.20)

set(TEST_EXEC "TestCheckEliminationPass")

set(TEST_SOURCES
    CheckEliminationPass.cpp
)

add_executable(${TEST_EXEC} ${TEST_SOURCES})
target_link_libraries(${TEST_EXEC} PUBLIC "VM-IR-Utils" GTest::gtest_main GTest::gmock_main)

target_include_directories(${TEST_EXEC}
    PUBLIC ${CMAKE_SOURCE_DIR}/Include
)

add_custom_target(run_check_elimination_pass_tests
    DEPENDS ${TEST_EXEC}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXEC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running check elimination pass tests"
    VERBATIM
)

add_dependencies(run_all_tests run_check_elimination_pass_tests)