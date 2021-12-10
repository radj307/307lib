@page page1 TermAPI

A C++20 Library designed for using ANSI escape sequences inline with std iostreams, or independently.


# Usage
Include the \<TermAPI.hpp\> header in the file you want to use it.  
All functions are contained within the sys::term namespace(s).

__A Note on Windows Escape Sequence Support__ 
The Windows 10 Anniversary Update added (more) support for "Virtual Sequences"; _(also known as ANSI escape sequences)_ 
this feature is disabled by default, and must be enabled by the program before using them.

TermAPI provides a convenient method for accomplishing this in the form of the __EnableANSI__ function.  
This function is available for all operating systems, however it does nothing on all operating systems other than Windows.  
Using preprocessor statements, the underlying code to enable virtual sequence support is automatic and does not require input from the programmer.  
```cpp
#include <TermAPI.hpp>
int main()
{
	std::cout << sys::term::EnableANSI;
	return 0;
}
```


## Version 2.0.0 Sequences
As of TermAPI 2.0.0, the old functions have been completely replaced with a new Sequence object.  
By default, these functions are used instead of the version 1.0.0 functions.  
The Sequence object is returned from most of the functions found in \<SequenceDefinitions.hpp\> and is called by using one of its operators:  
- operator<<  
  Inserts the sequence into the given output stream.
  ```cpp
  std::cout << sys::term::HideCursor << std::endl;
  ```
- operator>>
  This is a non-standard usage of the input stream operator, to allow using escape sequences inline with input streams.  
  The sequence will be printed to STDOUT by calling operator().
  ```cpp
  std::cin >> sys::term::setCursorPosition(5, 5);
  ```
- operator()  
  Can be called anywhere. The sequence will be printed to STDOUT using printf.
  ```cpp
  sys::term::HideCursor()();
  ```
Not all functions can be used inline, specifically functions that require multiple parameters for input or functions from TermAPIQuery.hpp that require reading responses from STDIN, such as getCursorPosition.  
  
## Using Version 1.0.0 Functions
To use the functions from v1:
```cpp
#define TERMAPI_ENABLE_OLD_FUNCTIONS
#include <TermAPI.hpp>
```

These functions cannot be used inline for the most part, except for a few specific functions.

## A Note on Cursor Positioning
By default, the screen buffer origin point is (1,1); _not (0,0) as one might expect._
TermAPI provides a function that allows you to change the origin point from (1,1) to (0,0) by automatically translating coordinates in all functions that accept or return cursor coordinates.
```cpp
// Set the cursor origin point to (0,0):
sys::term::SetOriginPos(0);
// Set the cursor origin point to (1,1):
sys::term::SetOriginPos(1);
```

# Color Library
TermAPI uses the xterm-256color palette by default.  

The color library uses the color namespace, and is split between multiple different headers which provide different levels of functionality.  
To quickly set the color of text, you can use something similar to this example:
```cpp
std::cout << color::setcolor(color::red) << "This is red!" << color::reset() << '\n';
```
You can set the color of the background by including a Layer when calling setcolor: _(By default, Layer::FOREGROUND is used.)_
```cpp
std::cout << color::setcolor(color::red, color::Layer::BACKGROUND) << "My background is red!" << color::reset() << '\n';
```
You can also make bold, inverted, or underlined text by using the FormatFlags object.  
To make bold text with an underline, you could use:
```cpp
std::cout << color::setcolor(color::green, FormatFlag::BOLD | FormatFlag::UNDERLINE) << "This is bold!" << color::reset() << '\n';
```

Since setcolor is an object, you can also instantiate it and use it whenever you want:
```cpp
int main()
{
	color::setcolor myColor{ color::red, Layer::BACKGROUND, FormatFlag::BOLD };
	std::cout << myColor << "This text is bold, with a red background!" << color::reset() << '\n'
			  << "This is the default text color\n"
			  << myColor << "And this is bold with a red background again!" << color::reset() << '\n';
}
```
This is especially useful as a member of another object, when you want it to be printed with the same color all the time.  

## ColorPalette
When using the color library, you may come across a situation where you want to be able to quickly set the same color for similar characters, and be able to disable all color sequences from being printed out in certain operation modes.  
To accomplish this, you can use the ColorPalette object from the ColorPalette.hpp header to quickly & easily set consistent colors, as well as disable/enable all sequences without changing your code.
All functions from the ColorPalette object will only insert characters into STDOUT when the color palette is set as active, otherwise they simply return right away.

```cpp
#include <TermAPI.hpp>
#include <ColorPalette.hpp>

// Create something to use as a key: (enum of type char uses only 1 byte, so is very useful)
enum class UIElement : char {
	MESSAGE,
	ERROR_CODE,
	BRACKETS,
	SOME_OTHER_ELEMENT,
}

// using UIElement as the key type:
using PaletteType = color::ColorPalette<UIElement>;

// define which colors to use for each UIElement:
PaletteType palette{
	std::make_pair(UIElement::MESSAGE, 				color::setcolor(color::green)),
	std::make_pair(UIElement::ERROR_CODE, 			color::setcolor(color::intense_red)),
	std::make_pair(UIElement::BRACKETS, 			color::setcolor(color::white, color::Layer::FOREGROUND, color::FormatFlag::BOLD | color::FormatFlag::INVERT)),
	std::make_pair(UIElement::SOME_OTHER_ELEMENT, 	color::setcolor(color::orange)),
};

void print_message()
{
	std::cout 
		<< palette.set(UIElement::MESSAGE)
		<< "This message is green!"
		<< palette.reset() 
		<< std::endl;
}

int main()
{
#ifdef OS_WIN
	std::cout << sys::term::EnableANSI;
#endif
	// Print a message with color:
	print_message();
	// Disable the palette & print a message without color:
	palette.setActive(false);
	print_message();
	return 0;
}
```


## DEC Line Drawings
```cpp
// Enable Line Drawing Mode:
sys::term::setCharacterSet(sys::term::CharacterSet::DEC_LINE_DRAWING)();
// Disable Line Drawing Mode:
sys::term::setCharacterSet(sys::term::CharacterSet::ASCII)();
```
TermAPI provides a helper object for drawing lines in the LineCharacter.hpp header.  
