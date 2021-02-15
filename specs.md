# Overview

Knight is meant to be easily implementable in virtually every language imaginable. As such, the language itself is not very complicated, and the specs leave a lot of things undefined and/or up to the implementation---this allows each langauge to implement it in the most idiomatic way possible.

# Syntax
## Overview
The language itself is inspired by Polish Notation (PN): Instead of `output(1 + 2 * 4)`, Knight has `OUTPUT + 1 * 2 4`.

Knight does not have a distinction between statements and expressions: Every single function in Knight returns a value, which can be used by other functions. So, instead of `if(x < 3) { output("hi") } else { output("bye") }`, Knight does `OUTPUT IF (< x 3) "hi" "bye"`.

All characters other than those mentioned in this document are considered invalid within Knight: Both within source code, and strings. Notably, the NUL character (`\0`) is not permissible within Knight strings, and can be used as a deliminator within implementations.

## Whitespace
The following characters are the only recognized whitespace characters:
- Tab (`0x09`, ie `\t`)
- Newline (`0x0a`, ie `\n`)
- Carriage return (`0x0d`, ie `\r`)
- Space (`0x20`, ie a space---` `)
- All parentheses (`(`, `)`, `[`, `]`, `{`, `}`).
(Because all functions have a fixed arity (see `Built-in Functions`), all forms of parentheses in Knight are considered whitespace.) Implementations may define other characters as whitespace if they wish---notably, this means that you may use regex's `\s` to strip away whitespace.

Additionally, the `:` function is a no op, and as such may safely be considered whitespace as well.

## Comments
Comments in Knight start with `#` and go until a newline character (`\n`) is encountered, or the end of the file; everything after the `#` is ignored.

There are no multiline or embedded comments in Knight.

## Literals
In Knight, there are two literals: Numbers and Strings.

Number literals are simply a sequence of ASCII digits (ie `0` (`0x30`) through `9` (`0x39`)). Leading `0`s do not indicate octal numbers (eg, `011` is the number eleven, not nine). No other bases are supported, and only integral numbers are allowed.

String literals in Knight begin with with either a single quote (`'`) or a double quote (`"`). All characters are taken literally until the opening close is encountered again. This means that there are no escape sequences within string literals; if you want a newline character, you will have to do:
```text
OUTPUT "this is a newline:
cool, right?"
```
Due to the lack of escape sequences, each string may only contain one of the two types of quotes (as the other quote will denote the end of the string.)

There are also boolean and null values within Knight. See `Functions` for more details on them.

## Variables
In Knight, all variables are lower case---upper case letters are reserved for functions. Variable names must start with an ASCII lower case letter (ie `a` (`0x61`) through `z` (`0x7a`)) or an underscore (`_` (`0x5f`)). After the initial letter, variable names may also include ASCII digits (ie `0` (`0x30`) through `9` (`0x39`)).

## Functions
In Knight, there are two different styles of functions: symbolic and word-based functions. In both cases, the function is uniquely 

Word-based functions start with a single uppercase letter, such as `I` for `if` or `R` for `random`, and may contain any amount of upper case letters afterwards. (Notably, this does _not_ include `_`). 
Symbolic functions are functions that are composed of a single symbol

# Details
## Types
### Number
In Knight, only integral numbers exist.
