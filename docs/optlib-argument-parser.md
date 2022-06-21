@page Commandline Argument Parser

The commandline argument parser is designed to be as flexible as possible while retaining similar syntax and usage styles to most GNU CLI programs.  

# Basic Concepts

Take note of the terminology used by `optlib` documentation:  
- The word *'argument'* should be taken to mean *any* commandline argument regardless of its type.
- The word *'option'* can mean different things depending on where it is used.
  When it appears as output from the program, *(i.e. from a `--help` command)*, 'option' means either a *long-opt* or a *short-opt*. *(See [Argument Types](#argument-types))*  
  Otherwise, it refers to the `optlib` implementation of a long-opt.

## Argument Types

Arguments are assigned 'types' depending on their prefix.  

| Argument Typename             | Prefix | Description                                                                                                                  | Capturing Rules | Capturable |
|-------------------------------|--------|------------------------------------------------------------------------------------------------------------------------------|-----------------|------------|
| **Option** <br/> *(long-opt)* | `--`   | Arguments of any length that have a valid prefix.                                                                            | When allowed by the implementation. | No |
| **Flag**  <br/> *(short-opt)* | `-`    | These are single-character arguments that may be chained together using a single prefix.                                     | **Unchained:** When allowed by the implementation.<br/>**Chained:** When allowed by the implementation, but only when last in the chain. | No |
| **Parameter**                 |        | Any argument that isn't an **Option** or **Flag**.                                                                           | Never | Yes |

Here's an example of argument type deduction from [ARRCON](https://github.com/radj307/ARRCON):
 ```
            Parameter
                v
 ARRCON -H 192.168.0.53 --port=25555
         ^                    ^
        Flag          Option with capture
 ```

## Argument Capturing

An argument's type determines whether it is *capable* of capturing other arguments, but whether it actually does is determined by a few factors:

- The argument in question was specified as a capturing argument by the developer.  
  This is generally indicated in usage guides by a bracket-enclosed string following the argument's name, i.e. `--arg <required>` or `--arg [optional]`.
- *Capturable* input is available.  
   Capturable input is defined as any of the following:  
  - A string that is directly appended to the argument, seperated by a single equals sign `=`.  
    Strings that include space characters can also be specified as long as they are enclosed with quotation marks, i.e. `="my string"`
  - A **Parameter** immediately following the argument. *(Options and flags can never be captured)*  

Arguments that are not specifically allowed to capture *cannot capture under any circumstances.*  
In the event that a user attempts to force a non-capturing option or flag to capture a string by appending `=...` to it, the string is inserted into the argument list as a Parameter instead.  

#### Note for Developers

Parameters are removed from the argument list if they are captured by an option or flag, and as such will not be present when accessing arguments by index, or when iterating over the list.  

## Argument Order

`optlib` is designed to provide an order-agnostic\* interface for developers, but does provide ways for developers to enforce their own ordering schemes.  
Consequently, whether the order of arguments is important or not should be considered to be *implementation-defined.*

\* The order of arguments doesn't matter.
