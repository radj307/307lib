@mainpage

# 307Lib
This is my personal collection of libraries for C++.  

These were all converted to build with CMake using the "Visual Studio 17 2022" generator, and changing CMake options is not yet guaranteed to work without issue.  
_(Except for the ENABLE_TESTING option, this is guaranteed to work as nothing else depends on it.)_    
Most of the options are dependent options, but I can't guarantee that everything will work as expected when disabling libraries as I am still new to using CMake.  

## Index
| Library | Version | Dependencies    | Purpose                                                                    | Minimum Version |
| ------- | ------- | --------------- | -------------------------------------------------------------------------- | --------------- |
| shared  | \<all\> |                 | Functions used by all other libraries.                                     | C++17 / C++20   |
| str-lib | \<all\> | shared          | String-related functions.                                                  | C++17 / C++20   |
| TermAPI | 3       | shared, str-lib | ANSI Escape Sequences, & various utilities for controlling the terminal.   | C++20           |
| TermAPI | 4       | shared		  | ANSI Escape Sequences, & various utilities for controlling the terminal.   | C++20           |
| file-lib| \<all\> | shared, str-lib | File I/O & Utility functions.                                              | C++14 / C++17   |
| opt-lib | \<all\> | shared, str-lib | Commandline-argument objects & functions, as well as environment variables | C++20           |


## Shared Headers
These are some of the shared lib's headers:

- sysarch.h  
	Used as a basic wrapper for certain preprocessor macros that can determine which C/C++ version the compiler is using, which compiler is being used, which operating system the compiler is running on, which architecture the compiler is set to, and more.  

- var.hpp  
	Contains some generic functions & concepts for working with variadic functions and tuples.  
	
- iter.hpp  
	Contains some generic functions related to iterators.
	
- math.hpp  
	Contains generic math & algorithm functions.
	
- make_exception.hpp  
	This is a wrapper based on Microsoft's non-standard implementation of std::exception to allow throwing generic exceptions with a message.  
	It accepts any number of arguments so long as they can be inserted into a std::ostream, and returns an _except_ struct, which is a child of std::exception.  
	To catch exceptions thrown with this method, any `catch(const std::exception& ex)` block will work.  

- Windows.hpp  
	Somewhat obviously, this is only relevant for Windows.  
	Experimental wrapper for the \<Windows.h\> header that includes it within a namespace to avoid polluting global.  
	Unfortunately, since most/all other libraries just include \<Windows.h\> in the global namespace this often will not work correctly.  
	It also undefines many of the \<Windows.h\> macros.
