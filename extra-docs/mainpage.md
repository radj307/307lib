# 307lib
This is my personal collection of libraries and utilities for C++  


Here are some brief descriptions of some important files:

	- sysarch.h
		Used as a basic wrapper for certain preprocessor macros that can determine which C/C++ version the compiler is using, which compiler is being used, which operating system the compiler is running on, which architecture the compiler is set to, and more.  
	
	- var.hpp
		Contains some generic functions & concepts for working with variadic functions and tuples.  
		
	- make_exception.hpp
		This is a wrapper based on Microsoft's non-standard implementation of std::exception to allow throwing generic exceptions with a message.  
		It accepts any number of arguments so long as they can be inserted into a std::ostream, and returns an _except_ struct, which is a child of std::exception.  
		To catch exceptions thrown with this method, any `catch(const std::exception& ex)` block will work.  

	- Windows.hpp
		Somewhat obviously, this is only relevant for Windows.  
		Experimental wrapper for the Windows.h header that includes it within a namespace to avoid polluting global. Unfortunately, since most/all other libraries just include \<Windows.h\> in the global namespace this often will not work correctly.  
		