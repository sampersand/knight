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

All characters other than those mentioned in this document are considered invalid within Knight, both within source code, and strings. Notably, the NUL character (`\0`) is not permissible within Knight strings, and can be used as a deliminator within implementations.

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

# Types
Knight itself only has a handful of builtin types---Numbers, Strings, Booleans, and Null. Knight has four different contexts: numeric, string, boolean, and "unchanged". Barring the "unchanged" context, values shall be able to be coerced to the correct type automatically. As such, all types define infallible conversions to each of these contexts.

Note that _all_ types within Knight are immutable. This means that it's a perfectly valid (and probably a good) idea to use reference counting in non-garbage-collected languages.

In addition to these types types, two additional types do exist: Identifier and Function. However, these types are only accessible via a `BLOCK`, and the only valid operation on them is to `CALL` them. As such, they do not have conversions defined on them (as doing so would be performing an operation other than `CALL`) and are not described here.

----------------------

## Number
In Knight, only integral numbers exist---all functions which might return non-integral numbers are simply truncated (look at the the functions' respective definitions for details on what exactly truncation means in each case).

All implementations must be able to represent a minimum integral value of `-2147483648`, and a maximal integral value of `2147483647` (ie, the minimum and maximum values for a 2's complement 32-bit integer). Implementations are allowed to represent numbers outside this range---this is simply the bare minimum that's required.

### Conversions
- numeric: In numeric contexts, the number itself is simply returned.
- string: In string contexts, numbers are converted to their base-10 representation. Negative numbers shall have a `-` prepended to the beginning of the string. (e.g. `0` -> `"0"`, `123` -> `"123"`, `- 0 12` => `"-12"`)
- boolean: In boolean contexts, nonzero numbers shall become `TRUE`, whereas zero shall become `FALSE`.

----------------------

## String
Strings in Knight are like strings in other languages, albeit a bit simpler: They're immutable (like all types within Knight), and can only represent a specific subset of the ASCII character set. 

Implementations are _only_ required to support the following characters within strings, although they may support additional characters if they want:
- Whitespace (see [Whitespace](#whitespace) for details)
- ASCII characters `0x21` (`!`) through `0x7e` (`~`)

### Conversions
- numeric: In numeric contexts, all leading whitespace (see [Whitespace](#whitespace) for details) shall be stripped. An optional `-` may then appear to force the number to be negative. Then, as many consecutive digits as possible are read, and then interpreted as if it were a number literal. In regex terms, It would be capture group of `^\s*(-?\d*)`. Note that if no valid digits are found after stripping whitespace and the optional `-`, the number `0` shall be used.
- string: In string contexts, the string itself is returned.
- boolean: In boolean contexts, nonempty strings shall become `TRUE`, whereas empty strings shall become `FALSE`.

----------------------

## Boolean
The Boolean type has two variants: `TRUE` and `FALSE`. These two values are used to indicate truthiness within Knight, and is the type that's should be converted to within boolean contexts.

### Contexts
- numeric: In numeric contexts, `TRUE` becomes `1` and `FALSE` becomes `0`.
- string: In string contexts, `TRUE` becomes `"true"` and `FALSE` becomes `"false"`.
- boolean: In boolean contexts, the boolean itself is simply returned.

----------------------

## Null
The `NULL` type is used to indicate the absence of a value within Knight, and is the return value of some function (such as `OUTPUT` and `WHILE`). 

### Contexts
- numeric: Null must become `0` in numeric contexts.
- string: Null must become `"null"` in string contexts.
- boolean: Null must become `FALSE` in boolean contexts.

----------------------

Functions:
`O` and `W` both return null always


# Details
## Functions
### Adding new functions.
