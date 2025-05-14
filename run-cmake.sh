#!/bin/bash

set -euo pipefail

# Change to the script's directory
cd "$(dirname "$0")"

# Use Homebrew LLVM for the latest C++ compiler
llvm="$(brew --prefix llvm)"
export CC="${llvm}/bin/clang"
export CXX="${llvm}/bin/clang++"
export LDFLAGS="-L${llvm}/lib"
export CPPFLAGS="-I${llvm}/include"

# Enable test builds
BUILD_TESTS=On

# Create and enter build directory
mkdir -p build
cd build

# Generate the build system using Ninja
cmake .. -G Ninja -Wno-dev -DBUILD_TESTING="${BUILD_TESTS}"

# Build the project
cmake --build .

# Run tests if the build succeeds and testing is enabled
if [[ "${BUILD_TESTS}" == *On* ]]; then
    ctest --rerun-failed --output-on-failure
fi
