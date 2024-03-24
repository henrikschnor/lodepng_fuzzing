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
