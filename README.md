# Lodepng fuzz test with google/fuzztest

This is an example fuzz test using the google/fuzztest framework to test the lodepng library. It shows how input domains can be used to describe valid test inputs for the code under test.

**Building and running the test**

```sh
# Clone the repository, including submodules
git clone --recurse-submodules https://github.com/henrikschnor/lodepng_fuzztest.git
cd lodepng_fuzztest

# Create a build directory
mkdir -p build
cd build

# Build the fuzz test with the google/fuzztest engine
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DFUZZTEST_FUZZING_MODE=on ..
cmake --build .

# Alternatively: Build the fuzz test in compatibility mode with libfuzzer
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DFUZZTEST_COMPATIBILITY_MODE=libfuzzer ..
cmake --build .

# Run the fuzz test
./fuzz_lodepng_decode --fuzz=FuzztestSuite.FuzzLodepngDecode
```

**Measuring code coverage**

```sh
# The fuzz test and the system under test need to be built with coverage instrumentation.
# Look at the CMakeLists.txt to see which flags are used with the `COVERAGE_INSTRUMENTATION` option.
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DFUZZTEST_FUZZING_MODE=on -DCOVERAGE_INSTRUMENTATION=on ..

# Run the fuzz test
./fuzz_lodepng_decode --fuzz=FuzztestSuite.FuzzLodepngDecode

# Alternatively: Run the fuzz test on the corpus inputs of a previous run.
# First: Save the corpus of a normal fuzzing run (doesn't need coverage instrumentation)
FUZZTEST_TESTSUITE_OUT_DIR=corpus/fuzz_lodepng_decode/FuzztestSuite.FuzzLodepngDecode/coverage ./fuzz_lodepng_decode --fuzz=FuzztestSuite.FuzzLodepngDecode
# Then: Replay the corpus with a coverage instrumented build of the fuzz test
./fuzz_lodepng_decode --corpus_database=corpus --replay_coverage_inputs=true

# Running the instrumented fuzz test should have produced a `default.profraw` file in the current directory.
# This file needs to be converted to an indexed profile data file
llvm-profdata merge -sparse default.profraw -o default.profdata

# Create a coverage summary
llvm-cov report -show-functions -Xdemangler=c++filt -instr-profile=default.profdata ./CMakeFiles/lodepng.dir/lodepng/lodepng.cpp.o ../lodepng/lodepng.cpp

# Alternatively: Create an html report
llvm-cov show -instr-profile=default.profdata --format=html ./CMakeFiles/lodepng.dir/lodepng/lodepng.cpp.o > coverage.html
```
