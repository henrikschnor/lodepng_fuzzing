cmake_minimum_required(VERSION 3.19)
project(lodepng_fuzztest)

# Option to add coverage instrumentation
option(COVERAGE_INSTRUMENTATION "Add coverage instrumentation to generate coverage reports." OFF)
set(COVERAGE_FLAGS -fprofile-instr-generate -fcoverage-mapping)

# Export build flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load google/fuzztest
add_subdirectory(fuzztest)
enable_testing()
fuzztest_setup_fuzzing_flags()

# Build lodepng
add_library(lodepng STATIC lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC lodepng)

# Build the fuzztest
add_executable(fuzz_lodepng_decode fuzz_lodepng_decode.cpp)
target_link_libraries(fuzz_lodepng_decode PRIVATE lodepng)
link_fuzztest(fuzz_lodepng_decode)

# Optionally add coverage instrumentation
if(COVERAGE_INSTRUMENTATION)
    target_compile_options(lodepng PRIVATE ${COVERAGE_FLAGS})
    target_compile_options(fuzz_lodepng_decode PRIVATE ${COVERAGE_FLAGS})
    target_link_options(fuzz_lodepng_decode PRIVATE ${COVERAGE_FLAGS})
endif()

