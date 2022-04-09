@mainpage

<p align="center"><a href="https://github.com/radj307/307lib"><img alt="307lib Banner" src="https://i.imgur.com/ygAstMw.png"></a></p>
<h1 align="center">C++ 20 Common Libraries</h1>
<p align="center"><a href="https://github.com/radj307/307lib/releases"><img alt="Latest Github Tag" src="https://img.shields.io/github/v/tag/radj307/307lib?color=e8e8e7&label=Latest%20Version&logo=github&logoColor=e8e8e7&style=for-the-badge"></a>
 <a href="https://github.com/radj307/307lib/actions/workflows/update-documentation.yml"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/workflow/status/radj307/307lib/Update%20Documentation?color=e8e8e7&label=Auto-Doxygen&logo=github&logoColor=e8e8e7&style=for-the-badge"></a>
</p>
 
This is a collection of common libraries that I've created over the years.  
It aims to provide a universal set of cross-platform C++ extensions to accomplish pretty much anything with ease.  

# Platform Support

Currently supported operating systems:
 - Windows
 - Linux
 - macOS

This library was designed for x64 and x86 systems, but may work on other architectures.

# Building from Source

Building from source is easy and platform-independent.

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
 3. Configure the project using cmake:  
    `cmake -B ./out -S . -G Ninja`
 4. Build the project using cmake:  
    `cmake --build ./out`    
 5. Install the project using cmake:  
    `cmake --install ./out`

# Generating Documentation

The github pages site is automatically generated using Github Actions & Doxygen using the following process:

## Pre-Requisites

 - git  <i>(obviously)</i>
 - [Doxygen](https://github.com/doxygen/doxygen)
   - This should be on your PATH, unless you're using the GUI version.
 - [Graphviz](https://graphviz.org/download/)
   - This must be on your PATH.

## Process

 1. Clone the repository & `cd` to it.  
    `git clone https://github.com/radj307/307lib && cd 307lib`    
 2. Clone the `gh-pages` branch as a subdirectory of the main repository & `cd` to it.  
    `git clone --single-branch --branch gh-pages https://github.com/radj307/307lib gh-pages && cd gh-pages`    
 3. Initialize & clone submodules.  
    `git submodule init && git submodule update`    
 4. Run doxygen.  
    `doxygen ./Doxyfile`
