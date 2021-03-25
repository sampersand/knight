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

.equ KN_FALSE,      0b0000
.equ KN_NULL,       0b0010
.equ KN_TRUE,       0b0100
.equ KN_UNDEFINED,  0b1000

.equ KN_NUM_SHIFT,  4
.equ KN_NUM_BIT,    0b0001
.equ KN_STR_BIT,    0b0010
.equ KN_VAR_BIT,    0b0100
.equ KN_FUNC_BIT,   0b1000

.equ KN_TAG_MASK,   0b1111

.macro kn_vl_new_number reg:req
	assert cmp, \reg, \reg, jmp
	shl $KN_NUM_SHIFT, \reg
	inc \reg
.endm

.macro kn_vl_new_string reg:req, clobber=%r15
    .ifdef KN_DEBUG
	pushq \clobber
	movq \reg, \clobber
	andq $KN_TAG_MASK, \clobber
	assert cmp, $0, \clobber, je, "clobber isn't zero: %$2lld (\reg).\n"
	assert cmp, $0, \reg, jne, "register is null (\reg).\n"
	popq \clobber
    .endif

    or $KN_STR_BIT, \reg
.endm

.macro kn_vl_new_var reg:req, clobber=%r15
    .ifdef KN_DEBUG
	pushq \clobber
	movq \reg, \clobber
	andq $KN_TAG_MASK, \clobber
	assert cmp, $0, \clobber, je, "clobber isn't zero: %$2lld (\reg).\n"
	assert cmp, $0, \reg, jne, "register is null (\reg).\n"
	popq \clobber
    .endif

    or $KN_VAR_BIT, \reg
.endm

.macro kn_vl_as_number reg:req
	assert test, $KN_NUM_BIT, \reg, jnz, "value %2$d is not a number"
	sar $KN_NUM_SHIFT, \reg
.endm

.macro kn_vl_as_string reg:req
	assert test, $KN_STR_BIT, \reg, jnz, "register '\reg' ('%lld') doesn't match '%lld'."
	sub $KN_STR_BIT, \reg
.endm
