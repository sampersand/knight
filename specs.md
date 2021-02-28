# Overview

Knight is meant to be easily implementable in virtually every language imaginable. As such, the language itself is not very complicated, and the specs leave a lot of things undefined and/or up to the implementation---this allows each language to implement it in the most idiomatic way possible.

## Notation
In this document, some notation is used to describe what is required of implementations:
- The word **required** indicates directions implementations if they want to be valid.
- The word **optional** indicates directions that probably should be implemented, but aren't required
- The word **undefined** is used to indicate that behaviour is undefined: Programs that contain undefined behaviour are invalid, and the interpreter does not have to provide any guarantees. (However, if possible, implementations should gracefully exit.)

# Syntax
## Overview
The language itself is inspired by Polish Notation (PN): Instead of `output(1 + 2 * 4)`, Knight has `OUTPUT + 1 * 2 4`.

Knight does not have a distinction between statements and expressions: Every single function in Knight returns a value, which can be used by other functions. So, instead of `if(x < 3) { output("hi") } else { output("bye") }`, Knight does `OUTPUT IF (< x 3) "hi" "bye"`.

All characters other than those mentioned in this document are considered invalid within Knight: Both within source code, and strings. Notably, the NUL character (`\0`) is not permissible within Knight strings, and can be used as a deliminator within implementations.

## Whitespace
Implementations are **required** to recognize the following characters as whitespace:
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
In Knight, all variables are lower case---upper case letters are reserved for functions. Variable names must start with an ASCII lower case letter (ie `a` (`0x61`) through `z` (`0x7a`)) or an underscore (`_` (`0x5f`)). After the initial letter, variable names may also include ASCII digits (ie `0` (`0x30`) through `9` (`0x39`)). The maximum length of variables should only be constrained by available memory.

## Functions
In Knight, there are two different styles of functions: symbolic and word-based functions. In both cases, the function is uniquely identified by its first character. 

Word-based functions start with a single uppercase letter, such as `I` for `IF` or `R` for `RANDOM`, and may contain any amount of upper case letters afterwards. (Note that they may _not_ include `_`---that is considered the start of an identifier.) This means that `R`, `RAND`, `RANDOM`, `RANDINT`, `RANDOMNUMBER`, etc. are all the same function---the `R` function.

In contrast, Symbolic functions are functions that are composed of a single symbol, such as `;` or `%`. Unlike word-based functions, they should not consume additional characters following them, word-based or not. The characters `+++` should be interpreted identically to `+ + +`---three separate addition functions.

Each function has a predetermined arity---no variable argument functions are allowed. After parsing a function's name, an amount of expressions corresponding to that function's arity should be parsed: For example, after parsing a `+`, two expressions must be parsed, such as `+ 1 2`. Programs that contain functions with fewer than the required amount of arguments are considered undefined. While not necessary, it's recommended to provide some form of error message (if it's easy to implement), such as `missing argument 2 for ';`, or even `missing an argument for ';'`.

The list of required functions are as follows. Implementations may define additional symbolic or keyword-based functions as desired. (For details on what individual functions mean, see `# Semantics`.)

- Arity `0`: `PROMPT`, `RANDOM`, `TRUE`, `FALSE`, `NULL`.
- Arity `1`: `BLOCK`, `EVAL`, `CALL`, `QUIT`, `LENGTH`, `DUMP`, `OUTPUT`, `` ` ``, `!`, `:`
- Arity `2`: `WHILE`, `+`, `-`, `*`, `/`, `%`, `^`, `?`, `<`, `>`, `&`, `|`, `;`, `=`
- Arity `3`: `IF`, `GET`
- Arity `4`: `SET`

Short note on `TRUE`/`FALSE`/`NULL`: As they are functions that take no arguments, and should simply return a true, false, or null value, they can be instead interpreted as literals. That is, there's no functional difference between parsing `TRUE` as a function, and then executing that function and parsing `TRUE` as a boolean literal.

Note that `:` is the "no-op" function, and can safely be considered a piece of whitespace.

### Implementation-Defined Functions
Implementations may define their own functions, as long as they start with an upper-case letter, or a symbol. 

--# Optional features.--
# Details
## Functions
### Adding new functions.

## Types
Knight itself only has a handful of builtin types---Numbers, Strings, Booleans, and Null. Knight has four different contexts: numeric, string, boolean, and "unchanged". Barring the "unchanged" context, values shall be able to be coerced to the correct type automatically.

### Null
`NULL` is used to indicate the absence of a value within Knight, and some functions may return it (For example, `WHILE` when no expression is executed). There is only one null type in Knight.

#### Conversions
- `Number`: Null is required to evaluate to `0` in a numeric context.
- `String`: Null is required to evaluate to either `"null"` in a string context.
- `Boolean`: Null is required to evaluate to evaluate to `FALSE` in a boolean context.

#### Functions
- `? NULL rhs`: The the only function defined on `NULL`--This shall return a truthy value when the 

### Booleans
The boolean type in Knight is how truthiness is defined. Booleans have exactly two valid values: `TRUE` and `FALSE`. In certain contexts (such as the condition of `IF`), expressions will be converted to their boolean value.

The conversions for booleans themselves is defined as follows:
- `Number`: In numeric contexts, `TRUE` shall become `1` and `FALSE` shall become `0`.
- `String`: In string contexts, `TRUE` shall become `"true"` and `FALSE` shall become `"false"`.
- `Boolean`: In boolean contexts, `TRUE` and `FALSE` do not change.

### Integers
In Knight, only integral numbers exist.
