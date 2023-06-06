@page opt3 Commandline Argument Manager

# opt3

The `opt3` namespace provides everything you need to manage and utilize GNU-style commandline arguments independently of platform or compiler.  

opt3 makes extensive use of modern C++ features like [concepts](https://en.cppreference.com/w/cpp/language/constraints), [variadic functions & templates](https://en.cppreference.com/w/cpp/utility/variadic), [variants](https://en.cppreference.com/w/cpp/utility/variant), and [optionals](https://en.cppreference.com/w/cpp/utility/optional).  
It is not the most efficient library out there by any means, but it makes up for it by being easy to set up & use in a wide variety of applications.

## Getting Started

The opt3 library revolves around the `ArgManager` *(`arg_manager`)* class. `ArgManager` inherits from the `arg_container` class, which provides a comprehensive set of functions specifically designed to handle command-line arguments effectively. Documentation on these functions can be found [here](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html).  
Creating a new `ArgManager` instance is simple, and only *requires* the argument count (`argc`) and argument array (`argv`) from your program's `main()` function.  
```cpp
#include <opt3.hpp>

int main(const int argc, char** argv)
{
  opt3::ArgManager args{ argc, argv }; //< ready to use!
}
```

## Basic Concepts

See [here](https://www.learncpp.com/cpp-tutorial/command-line-arguments/) for an introduction to commandline arguments in C++.  

The ArgManager class constructor parses the `argc` and `argv` parameters from your `main()` function into a vector of `std::variant` type, preserving argument precedence. Each `std::variant` can represent an `opt3::Option`, `opt3::Flag`, or `opt3::Parameter` (refer to [Argument Types](#argument-types)). The entire parsing process happens during the instantiation of `ArgManager`.

Options and Flags can capture additional input, which is stored as an optional member within the corresponding `opt3::Option` or `opt3::Flag` object. Captured input is denoted by appending an equals sign (`=`) to the option/flag name. For example, `--opt=someCapturedString` is an `opt3::Option` named `opt` that captures the string `someCapturedString`. This style of capturing is called *explicit* capturing.  
Alternatively, Options/Flags can be configured to capture Parameters that immediately follow them by providing the option/flag name to the constructor of `ArgManager`. This style of capturing is called *implicit* capturing. If a parameter is captured by an option/flag, it is removed from the direct argument list. Therefore, in the example `--opt someCapturedParam`, the `opt3::Option` named `opt` captures an `opt3::Parameter` named `someCapturedParam`, which is no longer present directly in the argument list. Instead, it can be accessed through the `opt` option. See [Defining Capturing Arguments](#defining-capturing-arguments) for more information.

When referencing an argument in your code, only the argument's *name* is used. An argument's name does not include prefix characters like dashes (`-`). For instance, to refer to the flag "-h" you would use the char 'h' or string "h", **not** "-h". This allows your code to work independently of whatever prefix delimiters are being used.

### Argument Types

One of three *argument types* is assigned to each parsed argument, which corresponds to its semantics and how it is interpreted.  

- Option  
  - Name is preceded by two delimiters `--`.
  - Names can be any length greater than 0.
  - Is able to capture parameters.
- Flag  
  - Name is preceded by one delimiter `-`.
  - Names are always exactly 1 character in length.
  - Is able to capture parameters.
  - Multiple flags can be chained together. Only the last flag in the chain can capture parameters.  
    *Ex: `-qv`* is equivalent to *`-q -v`*
- Parameter  
  - Any argument that is not an option or a flag.
  - Is able to be captured by Options/Flags. When a Parameter is captured, it is removed from the argument list.
  - Note that negative numbers *(ex: `-20`)* may result in unexpected behaviour because they may be interpreted as Flags when the `assumeValidNumberWithDashPrefixIsNegative` parsing rule is set to `false` (see [Argument Parsing Rules](#argument-parsing-rules)).

In the code, these arguments are templated instances of [`basic_arg_t`](https://radj307.github.io/307lib/html/structopt3_1_1basic__arg__t.html)

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

## Examples

### Check if an argument was specified

You can check if an argument was specified on the commandline by using the [`check`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#a686d6571318aae77d59524aef42e218e), [`check_any`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#ac7060604d1a29465ce34723f5dcf4216), & [`check_all`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#a52716edbccb679dca5149f60930e9b51) methods.
```cpp
opt3::ArgManager args{ argc, argv };

if (args.check('v')) {
  // parameter "v" or flag "-v" was specified
}
if (args.check<opt3::Option>("help")) {
  // option "--help" was specified
}
if (args.check_any<opt3::Flag, opt3::Option>('h', "help")) {
  // flag "-h" or option "--help" was specified
}
if (args.check_all<opt3::Flag, opt3::Option>('h', "help")) {
  // flag "-h" and option "--help" were specified
}
```

### Check if an argument has a capture

You can check if an argument has captured input by using the [`checkv`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#a48eadc0234a6c37abe5156b42632f100), [`checkv_any`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#a8e018c8dabeef85544cdacb688ed9d94), & [`checkv_all`](https://radj307.github.io/307lib/html/structopt3_1_1arg__container.html#ad4ae2185b17551ab032074953786bb0c) methods.
```cpp
opt3::ArgManager args{ argc, argv,
  'f',
  "opt"
};

if (args.checkv('f')) {
  // flag "-f" has a capture argument
}
if (args.checkv_any('f', "opt")) {
  // flag "-f" or option "--opt" has a capture argument
}
if (args.checkv_all('f', "opt")) {
  // flag "-f" and option "--opt" have a capture argument
}
```
