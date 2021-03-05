# Overview
Knight is meant to be easily implementable in virtually every language imaginable. As such, the language itself is not very complicated, and the specs leave a lot of things unspecified and/or up to the implementation---this allows each language to implement it in the most idiomatic way possible.

## Notation
In this document, some notation is used to describe what is required of implementations:
- The words **required**/**must**/**shall** indicates directions implementations if they want to be valid.
- The word **optional**/**may** indicates directions that probably should be implemented, but aren't required
- The word **undefined** is used to indicate that behaviour is undefined: Programs that contain undefined behaviour are invalid, and the interpreter does not have to provide any guarantees. (However, if possible, implementations should gracefully exit.)

# Table of Contents

1. [Syntax](#syntax)  
	1.1 [Whitespace](#whitespace)  
	1.2 [Comments](#comments)  
	1.3 [Literals](#literals)  
	1.4 [Variables](#variables)  
	1.5 [Functions](#functions)  
2. [Types](#types)  
	2.1 [Number](#number)  
	2.2 [String](#string)  
	2.3 [Boolean](#boolean)  
	2.4 [Null](#null)  
3. [Variables](#variables-1)  
4. [Functions](#functions-1)  
	4.1.1 [`TRUE`](#true)  
	4.1.2 [`FALSE`](#false)  
	4.1.3 [`NULL`](#null-1)  
	4.1.4 [`PROMPT`](#prompt)  
	4.1.5 [`RANDOM`](#random)  

	4.2.1 [`EVAL`](#)  
	4.2.2 [`BLOCK`](#)  
	4.2.3 [`CALL`](#)  
	4.2.4 [`` ` ``](#)  
	4.2.5 [`QUIT`](#)  
	4.2.6 [`!`](#)  
	4.2.7 [`LENGTH`](#)  
	4.2.8 [`DUMP`](#)  
	4.2.9 [`OUTPUT`](#)  

	4.3.1 [`+`](#)  
	4.3.2 [`-`](#)  
	4.3.3 [`*`](#)  
	4.3.4 [`/`](#)  
	4.3.5 [`%`](#)  
	4.3.6 [`^`](#)  
	4.3.7 [`<`](#)  
	4.3.8 [`>`](#)  
	4.3.9 [`?`](#)  
	4.3.10 [`|`](#)  
	4.3.11 [`&`](#)  
	4.3.12 [`;`](#)  
	4.3.13 [`=`](#)  
	4.3.14 [`WHILE`](#)  

	4.4.1 [`IF`](#)  
	4.4.2 [`GET`](#)  

	4.5.1 [`SUBSTITUTE`](#)  

# 1 Syntax
The language itself is inspired by Polish Notation (PN): Instead of `output(1 + 2 * 4)`, Knight has `OUTPUT + 1 * 2 4`.

Knight does not have a distinction between statements and expressions: Every single function in Knight returns a value, which can be used by other functions. So, instead of `if(x < 3) { output("hi") } else { output("bye") }`, Knight does `OUTPUT IF (< x 3) "hi" "bye"`.

All characters other than those mentioned in this document are considered invalid within Knight, both within source code, and strings. Notably, the NUL character (`\0`) is not permissible within Knight strings, and can be used as a deliminator within implementations.

## 1.1 Whitespace
Implementations are **required** to recognize the following characters as whitespace:
- Tab (`0x09`, ie `\t`)
- Newline (`0x0a`, ie `\n`)
- Carriage return (`0x0d`, ie `\r`)
- Space (`0x20`, ie a space---` `)
- All parentheses (`(`, `)`, `[`, `]`, `{`, `}`).
(Because all functions have a fixed arity (see `Built-in Functions`), all forms of parentheses in Knight are considered whitespace.) Implementations may define other characters as whitespace if they wish---notably, this means that you may use regex's `\s` to strip away whitespace.

Additionally, the `:` function is a no op, and as such may safely be considered whitespace as well.

## 1.2 Comments
Comments in Knight start with `#` and go until a newline character (`\n`) is encountered, or the end of the file; everything after the `#` is ignored.

There are no multiline or embedded comments in Knight.

## 1.3 Literals
In Knight, there are two literals: Numbers and Strings.

Number literals are simply a sequence of ASCII digits (ie `0` (`0x30`) through `9` (`0x39`)). Leading `0`s do not indicate octal numbers (eg, `011` is the number eleven, not nine). No other bases are supported, and only integral numbers are allowed.

String literals in Knight begin with with either a single quote (`'`) or a double quote (`"`). All characters are taken literally until the opening close is encountered again. This means that there are no escape sequences within string literals; if you want a newline character, you will have to do:
```text
OUTPUT "this is a newline:
cool, right?"
```
Due to the lack of escape sequences, each string may only contain one of the two types of quotes (as the other quote will denote the end of the string.)

There are also boolean and null values within Knight. See `Functions` for more details on them.

## 1.4 Variables
In Knight, all variables are lower case---upper case letters are reserved for functions. Variable names must start with an ASCII lower case letter (ie `a` (`0x61`) through `z` (`0x7a`)) or an underscore (`_` (`0x5f`)). After the initial letter, variable names may also include ASCII digits (ie `0` (`0x30`) through `9` (`0x39`)). The maximum length of variables should only be constrained by available memory.

## 1.5 Functions
In Knight, there are two different styles of functions: symbolic and word-based functions. In both cases, the function is uniquely identified by its first character. 

Word-based functions start with a single uppercase letter, such as `I` for `IF` or `R` for `RANDOM`, and may contain any amount of upper case letters and `_` afterwards. This means that `R`, `RAND`, `RANDOM`, `RAND_INT`, `RAND_OM_NUMBER`, etc. are all the same function---the `R` function.
_(Note: This is a change from a previous version of Knight where `_` was _ not _ a valid part of an identifier. some implementations may need to be updated)_

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

### 1.5.1 Implementation-Defined Functions
Implementations may define their own functions, as long as they start with an upper-case letter, or a symbol. 

# 2 Types
Knight itself only has a handful of builtin types---Numbers, Strings, Booleans, and Null. Knight has a few different contexts (see [Functions](#Functions) for more details), of which `numeric`, `string`, and `boolean` coerce their types to the correct type. As such, all types define infallible conversions to each of these contexts.

Note that _all_ types within Knight are immutable. This means that it's a perfectly valid (and probably a good) idea to use reference counting in non-garbage-collected languages.

In addition to these types types, two additional types do exist: Identifier and Function. However, these types are only accessible via a `BLOCK`, and the only valid operation on them is to `CALL` them. As such, they do not have conversions defined on them (as doing so would be performing an operation other than `CALL`) and are not described here.

### 2.0.1 Evaluation
All builtin types in Knight (ie Number, String, Boolean, and Null) when evaluated, should return themselves. This is in contrast to identifiers and functions, which may return different values at different points during execution. 

## 2.1 Number
In Knight, only integral numbers exist---all functions which might return non-integral numbers are simply truncated (look at the the functions' respective definitions for details on what exactly truncation means in each case).

All implementations must be able to represent a minimum integral value of `-2147483648`, and a maximal integral value of `2147483647` (ie, the minimum and maximum values for a 2's complement 32-bit integer). Implementations are allowed to represent numbers outside this range---this is simply the bare minimum that's required.

### 2.1.1 Contexts
- **numeric**: In numeric contexts, the number itself is simply returned.
- **string**: In string contexts, numbers are converted to their base-10 representation. Negative numbers shall have a `-` prepended to the beginning of the string. (e.g. `0` -> `"0"`, `123` -> `"123"`, `- 0 12` => `"-12"`)
- **boolean**: In boolean contexts, nonzero numbers shall become `TRUE`, whereas zero shall become `FALSE`.


## 2.2 String
Strings in Knight are like strings in other languages, albeit a bit simpler: They're immutable (like all types within Knight), and can only represent a specific subset of the ASCII character set. 

Implementations are _only_ required to support the following characters within strings, although they may support additional characters if they want:
- Whitespace (see [Whitespace](#whitespace) for details)
- ASCII characters `0x21` (`!`) through `0x7e` (`~`)

### 2.2.1 Contexts
- **numeric**: In numeric contexts, all leading whitespace (see [Whitespace](#whitespace) for details) shall be stripped. An optional `-` may then appear to force the number to be negative. Then, as many consecutive digits as possible are read, and then interpreted as if it were a number literal. In regex terms, It would be capture group of `^\s*(-?\d*)`. Note that if no valid digits are found after stripping whitespace and the optional `-`, the number `0` shall be used.
- **string**: In string contexts, the string itself is returned.
- **boolean**: In boolean contexts, nonempty strings shall become `TRUE`, whereas empty strings shall become `FALSE`.


## 2.3 Boolean
The Boolean type has two variants: `TRUE` and `FALSE`. These two values are used to indicate truthiness within Knight, and is the type that's should be converted to within boolean contexts.

### 2.3.1 Contexts
- **numeric**: In numeric contexts, `TRUE` becomes `1` and `FALSE` becomes `0`.
- **string**: In string contexts, `TRUE` becomes `"true"` and `FALSE` becomes `"false"`.
- **boolean**: In boolean contexts, the boolean itself is simply returned.


## 2.4 Null
The `NULL` type is used to indicate the absence of a value within Knight, and is the return value of some function (such as `OUTPUT` and `WHILE`). 

### 2.4.1 Contexts
- **numeric**: Null must become `0` in numeric contexts.
- **string**: Null must become `"null"` in string contexts.
- **boolean**: Null must become `FALSE` in boolean contexts.


# 3 Variables
Variables in Knight must be able to hold all the builtin types, including other variable names and functions (both of which are returned by the `BLOCK` function).

All variables in Knight are global and last for the duration of the program. (There are no function-local variables, and all `EVAL`s are done at the global scope too.). That is, once a value is assigned to a variable name, that variable name will then never be "deallocated"---value associated with it may change, but the variable will never become undefined. 

Implementations must be able to support variables between 1 and 65535 characters long, however arbitrary-length variable names are encouraged. As is described in the parsing section, variable names must start with a lower-case letter or `_`, and may be followed by any amount of digits, lower-case letters, or `_`.

## 3.1 Evaluation
When evaluated, the variable must return the value previously assigned to it, unevaluated. (That is, if you say had `= foo BLOCK { QUIT 1 }` beforehand and later on evaluated `foo`, it should return the block, and _not_ quit the program.) Note that it's possible for multiple variables to be associated with the same object within Knight (eg `= foo (= bar ...)`).

It's considered undefined behaviour to attempt to evaluate a variable when it hasn't been assigned a value yet. Implementations are encouraged to, at the minimum, abort with a message such as `uninitialized variable accessed`, however this is not a requirement.

## 3.2 Contexts
It's undefined behaviour for a variable to be converted to _any_ type. Note that this is only an issue if a `BLOCK` is used to get a reference to a literal identifier---expressions such as `+ 1 a` should evaluate their arguments first, which will then (most likely) evaluate to a well-defined value. 

Expressions such as `+ (BLOCK foo) 34`, `/ 12 (BLOCK FOO)` and even `? (BLOCK foo) (BLOCK foo)` are all considered undefined. 

# 4 Functions
Every function in Knight has a predetermined arity---there are no varidict functions.

Unless otherwise noted, all functions will _evaluate_ their arguments beforehand. This means that `+ a b` should fetch the value of `a`, the value of `b`, and then add them together, and should _not_ attempt to add a literal identifier to another literal identifier (which is undefined behaviour.)

All arguments _must_ be evaluated in order (from the first argument to the last)---functions such as `;` rely on this.

### 4.0.1 Contexts
Some functions impose certain contexts on arguments passed to them. (See the `Conversion` section of the basic types for exact semantics.) The following are the contexts used within this document:
- `string`: The argument must be evaluated, and then converted to a [String](#String).
- `boolean`: The argument must be evaluated, and then converted to a [Boolean](#Boolean).
- `number`: The argument must be evaluated, and then converted to a [Number](#Number).
- `coerced`: The argument must be evaluated, and then converted to the type of the first argument. (This only appears in binary functions).
- `unchanged`: The argument must be evaluated, and is passed unchanged.
- `unevaluated`: The argument must not be evaluated at all.
If multiple contexts are possible, they're separated with `|`s.

## 4.1 Nullary (Arity 0)

### 4.1.1 `TRUE()`
As discussed in the [Boolean](#Boolean) section, `TRUE` may either be interpreted as a function of arity 0, or a literal value---they're equivalent. See the section for more details.

### 4.1.2 `FALSE()`
As discussed in the [Boolean](#Boolean) section, `FALSE` may either be interpreted as a function of arity 0, or a literal value---they're equivalent. See the section for more details.

### 4.1.3 `NULL()`
As discussed in the [Null](#Null) section, `NULL` may either be interpreted as a function of arity 0, or a literal value---they're equivalent. See the section for more details.

### 4.1.4 `PROMPT()`
This must read a line from stdin until the `\n` character is encountered, of an EOF occurs, whatever happens first. If the line ended with `\r\n` or `\n`, those character must be stripped out as well, regardless of the operating system. The resulting string (with trailing `\r\n`/`\n`) must be returned.

If stdin is closed, this function's behaviour is undefined.
If the line that's read contains any characters that are not allowed to be in Knight strings (see [String](#String)), this function's behaviour is undefined.

### 4.1.5 `RANDOM()`
This function must return a (pseudo) random number between 0 and, at a minimum, 2147483647 (`0x7fff_ffff`). Implementations are free to return a larger random number if they desire; however, all random numbers must be zero or positive.

Note that `RANDOM` _should_ return different numbers across subsequent calls and program executions (although this isn't strictly enforceable, by virtue of how random numbers work..). However, programs should use a somewhat unique seed for every program run (eg a simple `srand(time(NULL)))` is sufficient.)

## 4.2 Unary (Arity 1)

### 4.2.1 `EVAL(string)`
This function takes a single string argument, which should be executed as if it where Knight source code.

This function should act _as if_ its invocation were replaced by the contents of the string, eg:
```
; = a 3
; = bar "* a 4"
: OUTPUT + "a*4=" (EVAL bar)
```
should be equivalent to
```
; = a 3
; = bar "* a 4"
: OUTPUT + "a*4=" (* a 4)
```

### 4.2.2 `BLOCK(unevaluated)`
Unlike nearly every other function in Knight, the `BLOCK` function does _not_ execute its argument---instead, it returns the argument, unevaluated. This is the only way for knight programs to get unevaluated blocks of code, which can be used for delayed execution.

The `BLOCK` function is intended to be used to create user-defined functions (which can be run via `CALL`.) However, as it simply returns its argument, there's no way to provide an arity to user-defined functions: you must simply use global variables:
```
; = max BLOCK { IF (< a b) a b }
; = a 3
; = b 4
: OUTPUT + "max of a and b is: " (CALL max)
```

Regardless of the input, the only valid uses for the return value of this function are as the right-hand-side argument to an `=` function, or as the sole argument to `CALL`. All other uses constitute undefined behaviour.

### 4.2.3 `CALL(<special>)`
The only valid parameter to give to `CALL` is the return value of a `BLOCK`---any other value is considered undefined behaviour. 

`CALL` will simply evaluate its argument, returning whatever the result of evaluating its argument is.

### 4.2.4 `` `(string) ``
Runs the string as a shell command, returning the stdout of the subshell.

If the subshell returns a nonzero status code, this function's behaviour is undefined.
If the subshell's stdout does not contain characters that can appear in a string (see [String](#String)), this function's behaviour is undefined.

Everything else is left up to the implementation---what to do about stderr and stdin, whether to abort execution on failure or continue, how environment variables are propagated, etc.

### 4.2.5 `QUIT(number)`
Aborts the entire knight interpreter with the given status code.

Implementations must accept exit codes between 0 to 127, although they can permit higher status codes if desired.

It is undefined behaviour if the given status code is negative, or is above the highest possible status code.

### 4.2.6 `!(boolean)`
Returns the logical negation of its argument---truthy values become `FALSE`, and falsey values beocme `TRUE`.

### 4.2.7 `LENGTH(string)`
Returns the length of the string, in bytes.

Note that since Knight strings are a strict subset of ASCII, this is both the length of the string in bytes _and_ the length in unicode points.

### 4.2.8 `DUMP(unchanged)`
Dumps a debugging representation of its argument to stdout, without a trailing newline.

Note that this is intended to be used for debugging (and unit testing) purposes, and as such it does not have a strict requirement for what a "debugging representation" means. However, if you wish to use the Knight unit tests, then the output must be in the following format:
- `Null()`
- `Number(<number>)` - `<number>` should be base-10, with a leading `-` if negative.
- `Boolean(<bool>)` - `<bool>` must be either `true` or `false`.
- `String(<string>)` - The literal contents of the string---no escaping whatsoever should be performed. (e.g. `DUMP "foo'b)ar\"` should write `String(foo'b)ar\)`).
- `Identifier(<ident>)` - the name of an identifier.
- `Function(...)` - the contents do not matter, as long as it starts with `Function` and has parens.

### 4.2.9 `OUTPUT(string)`
Writes the string to stdout, flushes stdout, and then returns `NULL`.

Normally, a newline should be written after `string` (which should also flush stdout on most systems.) However, if `string` ends with a backslash (`\`), the backslash is **not written to stdout**, and trailing newline is suppressed. 

For example:
```
# normal string
; OUTPUT "foo"
; OUTPUT "bar"
foo
bar
# no trailing newline
; OUTPUT "foo\"
; OUTPUT "bar"
foobar
# With a string ending in `\n`
; OUTPUT "foo
"
; OUTPUT "bar"
foo

bar
```

## 4.3 Binary (Arity 2)
### 4.3.1 `+(string|number, coerce)`
### 4.3.2 `-(number, number)`
### 4.3.3 `*(string|number, coerce)`
### 4.3.4 `/(number, number)`
### 4.3.5 `%(number, number)`
### 4.3.6 `^(number, number)`
(`^` is undefined for negative exponents (even though it could be defined...))

### 4.3.7 `<(string|number|boolean, coerce)`
### 4.3.8 `>(string|number|boolean, coerce)`
### 4.3.9 `?(unchanged, unchanged)`
### 4.3.10 `|(unchanged, unevaluated)`
### 4.3.11 `&(unchanged, unevaluated)`
### 4.3.12 `;(unchanged, unchanged)`
### 4.3.13 `=(unevaluated, unchanged)`
Note: The first argument must be an identifier. Some implementations may convert the first argument to a string if it's not an identifier (in which case it's a `string` context), but that's not required.
### 4.3.14 `WHILE(unevaluated, unevaluated)`
(returns null)

## 4.4 Ternary (Arity 3)
### 4.4.1 `IF(boolean, unevaluated, unevaluated)`
### 4.4.2 `GET(string, number, number)`

## 4.5 Quaternary (Arity 4)
### 4.5.1 `SUBSTITUTE(string, number, number, string)`

# 5 Extensions
