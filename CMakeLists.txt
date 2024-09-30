cmake_minimum_required(VERSION 3.20)

project(VM-Compliers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_OPTIONS
	-Wall
	-Werror
	-Wextra
	-Wconversion
	-Wwrite-strings
	-Wcast-qual
	-Wundef
	-Wlogical-op
)

add_compile_options(${COMPILE_OPTIONS})

add_subdirectory(Src)
add_subdirectory(SampleIR)
