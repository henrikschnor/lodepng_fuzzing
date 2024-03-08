cmake_minimum_required(VERSION 3.19)
project(lodepng_fuzztest)

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

