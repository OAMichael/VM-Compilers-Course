cmake_minimum_required(VERSION 3.20)

set(TEST_EXEC "TestRegisterAllocator")

set(TEST_SOURCES
RegisterAllocator.cpp
)

add_executable(${TEST_EXEC} ${TEST_SOURCES})
target_link_libraries(${TEST_EXEC} PUBLIC "VM-IR-Utils" GTest::gtest_main GTest::gmock_main)

target_include_directories(${TEST_EXEC}
    PUBLIC ${CMAKE_SOURCE_DIR}/Include
)

add_custom_target(run_register_allocator_tests
    DEPENDS ${TEST_EXEC}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXEC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running register allocator tests"
    VERBATIM
)
