#!/bin/bash
cd "$(dirname "$0")" || exit

# Set to latest C++ compiler (Hombrew)
llvm=$(brew --prefix llvm)
export CC="${llvm}/bin/clang"
export CXX="${llvm}/bin/clang++"
export LDFLAGS="-L${llvm}/lib"
export CPPFLAGS="-I${llvm}/include"

# Set testing
BUILD_TESTS=On

# generate the build system with Nija
cmake -H. -B build -G Ninja -Wno-dev -DBUILD_TESTING=${BUILD_TESTS}

# run the build 
cd build || exit
cmake --build .

# run test when the build passes.
if [[ $? -eq 0 && ${BUILD_TESTS} == *On* ]]; then
    # run all tests in build directory
    ctest --rerun-failed --output-on-failure
fi
