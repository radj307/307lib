@mainpage

# 307lib

This is my personal collection of libraries for C++20.  

# Setup
## Pre-Requisities
 - [CMake](https://cmake.org/download/) 3.20 or later.
 - [Ninja](https://github.com/ninja-build/ninja/releases)
 - A supported compiler:
   - MSVC 16.11 or later.
   - gcc/g++ 10 or later.

## Process
 1. First, Clone the repository:  
    `git clone https://github.com/radj307/307lib && cd 307lib`

 2. Initialize & update git submodules:  
    `git submodule update --init --recursive`
    
 2. Configure the project using cmake:  
    `cmake -B ./out -S . -G Ninja`

 3. Build the project using cmake:  
    `cmake --build ./out`
    
 4. Install the project using cmake:  
    `cmake --install ./out`

# Generating Documentation
1. Clone the repository & `cd` to it.  
`git clone https://github.com/radj307/307lib && cd 307lib`
2. Clone the `gh-pages` branch as a subdirectory of the main repository & `cd` to it.  
`git clone --single-branch --branch gh-pages https://github.com/radj307/307lib gh-pages && cd gh-pages`
3. Initialize & clone submodules.  
`git submodule init && git submodule update`
4. Run doxygen.  
`doxygen ./Doxyfile`


To execute all of the above steps, you can use this one-liner:  
`git clone https://github.com/radj307/307lib && cd 307lib && git clone --single-branch --branch gh-pages https://github.com/radj307/307lib gh-pages && cd gh-pages && git submodule init && git submodule update && doxygen ./Doxyfile`  
