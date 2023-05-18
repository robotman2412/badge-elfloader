cmake_minimum_required(VERSION 3.10)

# Compile in release mode by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Define the project.
project(kernel LANGUAGES C CXX ASM)
add_compile_options(-Wno-unused-function)

# Add ABI files into include path.
include_directories(
	src
)

# Create output file and add sources.
add_library(kernel
	src/abicall.S
	src/abiwrapper.S
	src/trap.S
	src/interrupt.S
	src/kernel.cpp
)
