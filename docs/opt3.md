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
Capturing arguments must be specified in the constructor of `opt3::ArgManager`, and can be specified in a number of ways.

#### *Explicit* vs. *Implicit* Capture

A capture can be classified as either explicit or implicit. An explicit capture involves appending an equals sign `=` followed by a string. On the other hand, an implicit capture captures a Parameter that is separated by at least one space character.  
- Explicit: `--arg="Hello World!"`  
- Implicit: `--arg "Hello World!"`  

#### `CaptureStyle`
- Disabled  
  Argument cannot capture under any circumstances. If a capture is forced by appending an equals sign `=`, the value is converted to a Parameter and included in the argument list.
- EqualsOnly  
  Argument cannot implicitly capture, can only capture by appending an equals sign `=` followed by a value.
- Optional  
- Required  

You can specify characters or strings individually. Characters correspond to Flags, while strings correspond to Options.
```cpp
opt3::ArgManager args{ argc, argv, 'h', "help" };
```

```cpp
opt3::ArgManager args{ argc, argv,
  opt3::make_template(opt3::CaptureStyle::Optional, 'h', "help")
};
```

