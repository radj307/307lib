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

A capture argument can be specified explicitly or implicitly. An explicit capture involves appending an equals sign `=` followed by a string. On the other hand, an implicit capture captures a Parameter that is separated by at least one space character.  
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

### Argument Parsing Rules

If you want to change the way that opt3 parses commandline input, you can use the `ArgParsingRules` struct.  
The `ArgManager` constructor accepts an `ArgParsingRules` instance as the third parameter.
```cpp
int main(const int argc, char** argv)
{
  opt3::ArgParsingRules rules;
  rules.allowUnexpectedArgs = false;
  
  opt3::ArgManager args{ argc, argv, rules,
    'h',
    "help",
  };
}
```

The following settings are exposed via `ArgParsingRules`:

- `delimiters`
  - Defines the list of characters that are considered valid Flag/Option prefixes.
  - Default: `{ '-' }`
- `allowNumericFlags`  
  - Determines whether digits are allowed to be considered flags. This fixes a potential bug where Parameters are interpreted as Flags when entering negative numbers followed by alphanumeric characters in the same string, ex: `-200.0F`.
  - This is ignored when `-` is not a delimiter.
  - Default: `false`
- `assumeValidNumberWithDashPrefixIsNegative`
  - A valid negative number *(ex: `-30`)* would normally be treated like a Flag and interpreted as `-3 -0`. When this is true, valid negative numbers are interpreted as Parameters instead of flags.
  - This is ignored when `-` is not a delimiter.
  - Default: `true`
- `defaultCaptureStyle`
  - Determines the default capture style for arguments/templates in the capture list that did not have a capture style explicitly defined.
  - Default: `Optional`
- `allowUnexpectedArgs`
  - Determines what happens when the user enters an Option or Flag that wasn't explicitly defined in the capture list.
  - When `true`, syntactically correct Options & Flags are always allowed regardless of whether or not they were defined in the capture list. When `false`, only Options & Flags that were explicitly defined in the capture list are allowed.
  - The action taken for disallowed arguments depends on the value of `convertUnexpectedArgsToParameters`.
  - Default: `true`
- `convertUnexpectedArgsToParameters`
  - Determines what happens when unexpected arguments aren't allowed and an unexpected argument was entered by the user.
  - When `true`, unexpected Options & Flags are converted to Parameters instead of triggering an exception. When `false`, unexpected arguments will cause an exception to be thrown by the parser.
  - Disallowed arguments that were converted to Parameters will include prefix delimiters.  
    For example, in a project that expects `-h` but does **not** expect `-F`, the argument `-hF` would be interpreted as a Parameter with the value `-hF`.
  - This is ignored when `allowUnexpectedArgs` is `true`.
  - Default: `false`

# *WIP*
