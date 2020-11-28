# knight

## Example — Guessing Game
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
 := `F` | `FNDEF`
  | `C` | `CALL`
  | `O` | `OUTPUT`
  | `P` | `PROMPT`
  | `Q` | `QUIT`
  | `E` | `EVAL`
  | `S` | `SYSTEM`
  | `!`

binary
 := `R` | `RAND`
  | `W` | `WHILE`
  | `;` | `=`
  | `+` | `-` | `*` | `/` | `^`
  | `<` | `>` | `&` | `|`

ternary
 := `IF`
```
