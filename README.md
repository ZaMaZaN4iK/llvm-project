# Container usage efficiency static analyzer based on Clang Static Analyzer

This directory and its subdirectories contain source code for LLVM,
a toolkit for the construction of highly optimized compilers,
optimizers, and runtime environments.

# Required dependencies
* CMake (>= 3.4.3)
* C++ compiler with C++14 support

# How to build
* Checkout this repository
* Go to the checkouted repository
* Make build directory and change current directory: `mkdir build && cd build`
* Run CMake: `cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ../llvm`
* Build `clang-tidy` target

# How to use
Test one file: `clang-tidy -checks='clang-analyzer-cplusplus.InefficientContainer' main.cpp`
