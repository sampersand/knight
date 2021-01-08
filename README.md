# knight
An extremely simple programming language that I've designed to be easy to implement in a variety of languages. It's not actually meant to be used, though it is a fully-functional lang.

# Examples
Here's some examples of the syntax to give you a feel for it:

## Guessing Game
```
; = max 100                                   # max = 100
; = secret (RAND 1 max)                       # secret = rand(1, max)
; = nguess 0                                  # nguess = 0
; = guess 0                                   # guess = 0
; OUTPUT (+ 'guess 1-' max)                   # print('pick from 1-' + m)
; WHILE (| (< guess secret) (> guess secret)) # while guess != s:
  ; = guess (+ 0 (PROMPT '> '))               #   guess = int(prompt('> '))
  ; = nguess (+ nguess 1)                     #   nguess += 1
    OUTPUT (                                  #   print(
     IF (< guess secret) 'too low'            #     if guess < secret: 'too low'
     IF (> guess secret) 'too high'           #     if guess > secret: 'too high'
                         'correct')           #     else: 'correct')
OUTPUT (+ 'tries: ' nguess)                   # print('tries: ' + n)
```

## Fibonacci
```
; = fib BLOCK                           # function fib:
    ; = a 0                             #    a = 0
    ; = b 1                             #    b = 1
    ; WHILE n                           #    while n != 0:
        ; = b + a = tmp b               #       b = a + (tmp = b)
        ; = a tmp                       #       a = tmp
        : = n - n 1                     #       n -= 1
    : a                                 #    return a
; = n 10                                # n = 10
: OUTPUT +++ 'fib(' n ')=' CALL fib     # print "fib(" + n + ")=" + fib()
# => fib(10)=55
```
# Specs

## Syntax
Every Knight program is a single expression. (The `;` function can be used to write more than one expression, sequentially.) Because of this, parsing is extremely simple: Parse a token, then parse as many arguments as that expression dictates.

Non-symbol functions are defined by their first character: additional uppercase characters following it are ignored. Because of this, `OUTPUT` is the same as `OUT`, which is the same as `OFOOBARBAZ`.

All whitespace (including `(`, `)`, `[`, `],` `{,` `}`, and `:`) outside of strings is completely ignored except in four cases:
1. Between two "word" keywords, such as `IF PROMPT`
2. Between two numbers, such as `+ 1 2`
3. Between two identifiers, such as `* a b`
4. Between an identifier and a number, such as `+ a 3`.

As such, expressions such as `OUTPUT * a (IF b 3 b)` can be written as `O*aIb 3b`.
```ebnf
expr
 := nullary
  | unary expr
  | binary expr expr
  | ternary expr expr expr
  | quaternary expr expr expr expr

nullary 
 := [0-9]+
  | `'` [^']* `'` | `"` [^"]* `"`
  | [a-z_][a-z_0-9]*
  | ('T' | 'F' | 'N' | 'P' | 'R') {UPPER}
  ;

unary
 := ('B' | 'C' | 'O' | 'P' | 'Q' | 'S' | 'L') {UPPER}
  | '`'
  | '!'
  ;

binary
 := 'W' {UPPER}
  | '-' | '+' | '*' | '/' | '^'
  | '<' | '>' | '&' | '|' | '?'
  | ';' | '='
  ;

ternary := 'I' | 'G' ;
quaternary := 'S' ;
```

## Functions
```
A
BLOCK (body) - Anonymous function.
CALL (block) - Calls `block`.
D
EVAL (string) - Evaluates `string`.
FALSE () - `FALSE` literal.
GET (string, index, length) - Gets a substring of length `length` from `string` starting at `index`.
H
IF (cond, if_t, if_f) - Evaluates and returns `if_t` if `cond` is truthy. Otherwise, evaluates and returns `if_f`.
J
K
LENGTH (string) - Length of `string`.
M
NULL () - `NULL` literal.
OUTPUT (thing) - Prints `thing`. If `thing` ends with `\`, it omits the last character, otherwise appends a newline.
PROMPT () - Reads a line from stdin.
QUIT (status) - Quits with `status`.
RAND () - Returns a random integer.
SET (string, start, len, repl) - Returns a new string with the substring of length `len`, starting at `start`, replaced with `repl`.
TRUE () - `TRUE` literal.
U
V
W WHILE (cond, body) - Evaluates the `body` while the `cond`s true. Returns `body`s last value, or `NULL` if it never ran.
X
Y
Z

# - comment until EOL
+ (x, y) - If `x` is a string, converts `y` to a string and concats. Otherwise, convert both to a number and add them.
- (x, y) - Converts both to an integer returns `x - y`.
* (x, y) - Converts both to an integer returns `x * y`.
/ (x, y) - Converts both to an integer returns `x / y`.
% (x, y) - Converts both to an integer returns `x % y`. Optionally supports printf for a single argument if the language
has easy support for it.
^ (x, y) - Converts both to an integer returns `x ^ y`.
& (x, y) - Evaluates both, returns `x` if `x` is truthy, otherwise returns `y`.
| (x, y) - Evaluates both, returns `y` if `x` is truthy, otherwise returns `x`.
< (x, y) - If `x` is a string, converts `y` to a string and checks to see if `x` is less than `y` in the current local. Otherwise, converts both to an integer and sees if `x` is less than `y`.
> (x, y) - If `x` is a string, converts `y` to a string and checks to see if `x` is greater than `y` in the current local. Otherwise, converts both to an integer and sees if `x` is greater than `y`.
? (x, y) - If `x` is a string, converts `y` to a string and checks to see if both are equal. Otherwise, converts both to an integer and sees if theyre both equal.
! (x) - Boolean negation of `x`.

` (x) - Runs `x` as a shell command, returns `x`'s stdout.
; (x, y) - Evaluates `x`, then `y`, and returns `y`.
= (x, y) - Sets `x` in the global scope to `y`, crashing if `x` isnt an identifier.
,
@
~
$
.
\
```



## Details
The exact details of the language are not nailed down: This is intentional, as it's meant to be fairly easy to be implemented in each language. Thus, the maximum and minimum of integer types is unspecified, some functions are not included (such as the "system" function `` ` ``, `O`, `C`)
