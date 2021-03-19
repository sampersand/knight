
/*
abuse the fact that `malloc` will allocate things that are 16-aligned.
0...00000 = false
0...X0001 = 60-bit integer
0...00010 = null
0...00100 = true
X...X0000 = string, x nonzero
X...X0010 = ident, x nonzero
X...X0100 = function, x nonzero
*/

.equ KN_FALSE,   0b0000
.equ KN_NULL,    0b0010
.equ KN_TRUE,    0b0100

.equ KN_NUM_SHIFT, 4
.equ KN_NUM_BIT,   0b0001
.equ KN_STR_BIT,   0b0010
.equ KN_VAR_BIT,   0b0100
.equ KN_FUNC_BIT,  0b1000

.equ KN_TAG_MASK,   0b1111

.macro kn_new_number reg:req
	shl $KN_NUM_SHIFT, \reg
	or $KN_NUM_BIT, \reg
.endm

.macro kn_new_string reg:req, clobber=%r15
    .ifdef KN_DEBUG
	pushq \clobber
	movq \reg, \clobber
	andq $KN_TAG_MASK, \clobber
	assert_eq $0, \clobber
	assert_ne $0, \reg
	popq \clobber
    .endif
    or $KN_STR_BIT, \reg
.endm

.macro kn_value_new_boolean reg:req
	//cmp 
	assert_ne $0, \reg
.endm
