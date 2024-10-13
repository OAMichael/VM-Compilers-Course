cmake_minimum_required(VERSION 3.20)

set(TEST_EXEC "TestDominatorTree")

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/Src/Instruction.cpp
    ${CMAKE_SOURCE_DIR}/Src/Function.cpp
    ${CMAKE_SOURCE_DIR}/Src/ControlFlowGraph.cpp
    ${CMAKE_SOURCE_DIR}/Src/IRBuilder.cpp
    DominatorTree.cpp
)

add_executable(${TEST_EXEC} ${TEST_SOURCES})
target_link_libraries(${TEST_EXEC} GTest::gtest_main)

target_include_directories(${TEST_EXEC}
    PUBLIC ${CMAKE_SOURCE_DIR}/Include
)

add_custom_target(run_dominator_tree_tests
    DEPENDS ${TEST_EXEC}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXEC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running dominator tree tests"
    VERBATIM
)
