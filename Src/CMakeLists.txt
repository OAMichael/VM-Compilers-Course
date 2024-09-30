cmake_minimum_required(VERSION 3.20)

set(VMIR_UTILS_NAME VM-IR-Utils)

set(VMIR_UTILS_SOURCES
    Instruction.cpp
    Function.cpp
    IRBuilder.cpp
    ControlFlowGraph.cpp
)

add_library(${VMIR_UTILS_NAME} STATIC ${VMIR_UTILS_SOURCES})
target_include_directories(${VMIR_UTILS_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
