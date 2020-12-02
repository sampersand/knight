# knight
An extremely simple programming language that I've designed to be easy to implement in a variety of languages. It's not actually meant to be used, though it is a fully-functional lang.

# Specs


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

## Syntax
```
expr
 := nullary
  | unary expr
  | binary expr expr
  | ternary expr expr expr

primary
 :=
  | [0-9]+
  | `'` [^']* `'` | `"` [^"]* `"`
  | [a-z_][a-z_0-9]*

nullary 
 := primary
  | 'P' | 'PROMPT'
  | 'T' | 'TRUE' | 'F' | 'FALSE'
  | 'N' | 'NULL'

unary
 := 'B' | 'BLOCK'
  | 'C' | 'CALL'
  | 'O' | 'OUTPUT'
  | 'P' | 'PROMPT'
  | 'Q' | 'QUIT'
  | 'S' | 'SYSTEM'
  | '`'
  | '!'

binary
 := 'R' | 'RAND'
  | 'W' | 'WHILE'
  | ';' | '='
  | '+' | '-' | '*' | '/' | '^'
  | '<' | '>' | '&' | '|'

ternary
 := `IF`
```
