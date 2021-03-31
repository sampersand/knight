
/*
abuse the fact that `malloc` will allocate things that are 16-aligned.
0...00000 = false
0...XXX10 = 62-bit integer
0...00100 = null
0...01100 = true
X...X0001 = ident
X...X0101 = function
X...X1001 = string
*/

.equ FALSE_BITS, 0b0000
.equ NUM_BIT,    0b0010
.equ NULL_BITS,  0b0100
.equ TRUE_BITS,  0b1100

.equ ALLOC_BIT,  0b0001
.equ TAG_MASK,   0b1111

.equ FUNC_BIT,   0b0100
.equ FUNC_TAG,   FUNC_BIT | ALLOC_BIT
.equ STRING_BIT, 0b1000
.equ STRING_TAG, STRING_BIT | ALLOC_BIT
.equ IDENT_TAG,  0b0000 | ALLOC_BIT

