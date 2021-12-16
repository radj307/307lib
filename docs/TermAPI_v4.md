@page page2 TermAPI v4

A C++20 wrapper library for implementing ANSI escape sequences.

_v4 Documentation is a Work-In-Progress_

# How to use Version 3
You can still use TermAPI v3 by setting `TERMAPI_VERSION_3` to `ON` before adding 307lib as a subdirectory in CMake.  
Example:  
```cmake
set(TERMAPI_VERSION 3)
add_subdirectory("307lib")
```

# Changes in Version 4
- Removed `str-lib` dependency.
- Renamed the `sys::term` namespace to `term`
- Major overhaul of library structure. Headers have been split into the following object libraries, which are linked to the TermAPI library.
  - __ANSI__  
    Contains some helper objects & functions for working with ANSI escape sequences.  
  - __color__  
    Contains the color library.  
  - __term__  
    Contains the `sys::term` namespace from v3, now renamed to just `term`.  
	Much of the problematic modularity from v3 is now gone.
  - __extensions__  
    Extra headers that require libraries other than `shared`.  
	To enable extensions, `ENABLE_FILELIB` must be `ON`, then set `TERMAPI_ENABLE_EXTENSIONS` to `ON` in CMakeLists.txt before you add the "307lib" subdirectory.  
- Object libraries are automatically linked to the `TermAPI` library, don't include the object libraries directly unless you need specific features from them.  
- Replaced the `Sequence` object with a using statement.  
- Added wstring support to most/all functions with the `wSequence` type.  
- Sequences can no longer be called on their own, you're responsible for inserting sequence strings into STDOUT.  
- Optimizations & improvements across the board.
- Added more color definitions in `color-values.h`

# color

The color library received an extensive overhaul in v4, with the expansion of the `color-transform.h`