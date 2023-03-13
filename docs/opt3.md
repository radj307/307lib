@page opt3 Commandline Argument Manager

# opt3

The `opt3` namespace provides everything you need to manage and utilize GNU-style commandline arguments independently of platform or compiler.  
It is intended to replace the deprecated `optlib` library, and consists of a single header file (`opt3.hpp`).
Migrating an existing project from the now-deprecated optlib will likely require some code changes.

## Basic Concepts

### Argument Types

One of three *argument types* is assigned to each parsed argument, which corresponds to its semantics and how it is interpreted.

- Option  
  - Always preceded by two delimiters `--`.
  - Can be of any length.
  - Is able to capture parameters.
- Flag  
  - Always preceded by one delimiter `-`.
  - Always a single character in length.
  - Multiple flags can be chained together.  
    *Ex: `-qv`* is equivalent to *`-q -v`*
  - Is able to capture parameters as long as it is not part of a chain, or is the last element in the chain.
- Parameter  
  - Any argument that is not an option or a flag.
  - Note that negative numbers *(ex: `-20`)* may result in unexpected behaviour because they may be interpreted as Flags.

### Capturing Arguments

Option and Flag arguments can be made to *capture* Parameters. Captured parameters are not included directly in the list of arguments; instead they are stored within the argument that captured them.
Capturing arguments must be specified in the constructor of `opt3::ArgManager`, but they can be specified in a number of ways.

#### Defining Capturing Arguments
All capturing arguments must be specified in the constructor of `opt3::ArgManager`.  
If a Flag/Option is *not* specified, it will never be able to capture under any circumstances.

The simplest way is to pass all of the Flags and Options in your program that *can* capture to the `opt3::ArgManager` constructor:  
```cpp
int main(const int argc, char** argv)
{
  opt3::ArgManager args{ argc, argv, 'h', "help" };
}
```
You can force a specific Flag or Option to *require* capture input by using custom string literals:
```cpp
int main(const int argc, char** argv)
{
  using namespace opt3_literals;
  opt3::ArgManager args{ argc, argv, 'h'_requireCapture, "help"_reqcap };
  // ""_requireCapture & ""_reqcap can be used interchangeably.
}
```
Other custom string literals include:
- `""_requireCapture` / `""_reqcap`
- `""_optionalCapture` / `""_optcap`
- `""_noCapture` / `""_nocap`

Alternatively, you can use *template arguments* to define a set of arguments that have similar rules.
```cpp
int main(const int argc, char** argv)
{
  opt3::ArgManager args{ argc, argv,
    opt3::make_template(opt3::CaptureStyle::Optional, opt3::ConflictStyle::, 'h', "help")
  };
}
```

#### *Explicit* vs. *Implicit* Capture

A capture can be classified as either explicit or implicit. An explicit capture involves appending an equals sign `=` followed by a string. On the other hand, an implicit capture captures a Parameter that is separated by at least one space character.  
- Explicit: `--arg="Hello World!"`  
- Implicit: `--arg "Hello World!"`  

#### Capture Style
The `CaptureStyle` bitfield enum defines capturing rules for a specific argument or templated argument.

- Disabled  
  - Disables implicit and explicit captures for this argument/template.
  - If a user attempts to force an argument to capture by using an explicit capture, the value that would have been captured is treated like a Parameter and is added directly to the argument list.
- EqualsOnly  
  - Disables implicit captures for this argument/template.
- Optional *(Default)*
- Required  
  - An exception is thrown if this argument does not specify a capture value.

# *WIP*
