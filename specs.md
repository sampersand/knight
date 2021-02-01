# Overview

Knight is meant to be easily implementable in virtually every language imaginable. As such, the language itself is not very complicated, and the specs leave a lot of things undefined and/or up to the implementation---this allows each langauge to implement it in the most idiomatic way possible.

# Syntax
## Overview
The language itself is inspired by Polish Notation (PN): Instead of `output(1 + 2 * 4)`, Knight has `OUTPUT + 1 * 2 4`.

Knight does not have a distinction between statements and expressions: Every single function in Knight returns a value, which can be used by other functions. So, instead of `if(x < 3) { output("hi") } else { output("bye") }`, Knight does `OUTPUT IF (< x 3) "hi" "bye"`.

All characters other than those mentioned in this document are considered invalid within Knight: Both within source code, and strings. Notably, the NUL character (`\0`) is not permissible within Knight strings, and can be used as a deliminator within implementations.

# Whitespace
The following characters are the only recognized whitespace characters. (Because all functions have a fixed arity (see `Functions`), all forms of parenthesis in Knight are considered whitespace.):
- Tab (`0x09`, ie `\t`)
- Newline (`0x0a`, ie `\n`)
- Carriage return (`0x0d`, ie `\r`)
- Space (`0x20`, ie a space---` `)
- All parenthesis (`(`, `)`, `[`, `]`,
- Other whitespace characters (`(`, `)`, `[`, `]`, `{`, `}`)
- All digits (ie `0x30` through `0x39`)
- All upper-case letters (ie `0x41` through `0x5a`)
- All lower-case letters (ie `0x61` through `0x7a`)
- Any symbols defined in the specs, ie `` + - * / % ^ & | < > ? = ; : ! `
- `#` (ie `0x23`)

## Whitespace
In knight, whitespace is mostly irrelevant---it's used as an aid to the programmer to make their programs easier to read. Instead of writing `OUTPUT+'-x='-0x`, you can write `OUTPUT + '-x=' - 0 x`. In Knight, the following "normal whitespace characters" characters are considered whitespace:
- Space (0x20, ` `)
- Newline (0x0a, `\n`)
- Tab (0x09, `\t`)
Implementations may define additional
As all functions have a fixed arity (see the `Functions` section for more details), all forms of

As all functions have a fixed arity (more on user-defined functions later), all forms of parenthesis (ie `(`, `)`, `[`, `]`, `{`, `}`) are considered whitespace in addition to "normal" whitespace.


## Comments
## Literals
## Functions
