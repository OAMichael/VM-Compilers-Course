cmake_minimum_required(VERSION 3.20)


function(add_sample_ir_executable filename)
    set(SAMPLE_IR_EXEC IRGen${filename})
    set(SAMPLE_IR_SOURCES IRGen${filename}.cpp)
    add_executable(${SAMPLE_IR_EXEC} ${SAMPLE_IR_SOURCES})
    target_include_directories(${SAMPLE_IR_EXEC} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${SAMPLE_IR_EXEC} PUBLIC "VM-IR-Utils")
endfunction()


add_subdirectory(FactRecursive)
add_subdirectory(FactLoop)
