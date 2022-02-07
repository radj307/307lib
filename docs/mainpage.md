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
