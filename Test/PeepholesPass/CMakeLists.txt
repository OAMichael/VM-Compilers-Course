cmake_minimum_required(VERSION 3.20)

set(TEST_EXEC "TestPeepholesPass")

set(TEST_SOURCES
    PeepholesPass.cpp
)

add_executable(${TEST_EXEC} ${TEST_SOURCES})
target_link_libraries(${TEST_EXEC} PUBLIC "VM-IR-Utils" GTest::gtest_main GTest::gmock_main)

target_include_directories(${TEST_EXEC}
    PUBLIC ${CMAKE_SOURCE_DIR}/Include
)

add_custom_target(run_peepholes_pass_tests
    DEPENDS ${TEST_EXEC}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXEC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running peepholes pass tests"
    VERBATIM
)

add_dependencies(run_all_tests run_peepholes_pass_tests)