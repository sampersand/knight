# RDI, RSI, RDX, RCX, R8, R9
foo: .asciz "[%p]\n"

.quad 0
.global kn_fn_prompt
kn_fn_prompt:
	jmp die

.quad 0
.global kn_fn_rand
kn_fn_rand:
	jmp die

.quad 0
.global kn_fn_true
kn_fn_true:
	jmp die

.quad 0
.global kn_fn_false
kn_fn_false:
	jmp die

.quad 0
.global kn_fn_null
kn_fn_null:
	jmp die


.quad 1
.global kn_fn_block
kn_fn_block:
	jmp die

.quad 1
.global kn_fn_eval
kn_fn_eval:
	jmp die

.quad 1
.global kn_fn_call
kn_fn_call:
	jmp die

.quad 1
.global kn_fn_system
kn_fn_system:
	jmp die

.quad 1
.global kn_fn_quit
kn_fn_quit:
	jmp die

.quad 1
.global kn_fn_not
kn_fn_not:
	jmp die

.quad 1
.global kn_fn_length
kn_fn_length:
	jmp die

.quad 1
.global kn_fn_output
kn_fn_output:
	jmp die


.quad 2
.global kn_fn_add
kn_fn_add:
	jmp die

.quad 2
.global kn_fn_sub
kn_fn_sub:
	jmp die

.quad 2
.global kn_fn_mul
kn_fn_mul:
	jmp die

.quad 2
.global kn_fn_div
kn_fn_div:
	jmp die

.quad 2
.global kn_fn_mod
kn_fn_mod:
	jmp die

.quad 2
.global kn_fn_pow
kn_fn_pow:
	jmp die

.quad 2
.global kn_fn_eql
kn_fn_eql:
	jmp die

.quad 2
.global kn_fn_lth
kn_fn_lth:
	jmp die

.quad 2
.global kn_fn_gth
kn_fn_gth:
	jmp die

.quad 2
.global kn_fn_then
kn_fn_then:
	jmp die

.quad 2
.global kn_fn_assign
kn_fn_assign:
	jmp die

.quad 2
.global kn_fn_while
kn_fn_while:
	jmp die

.quad 2
.global kn_fn_and
kn_fn_and:
	jmp die

.quad 2
.global kn_fn_or
kn_fn_or:
	jmp die


.quad 3
.global kn_fn_if
kn_fn_if:
	jmp die

.quad 3
.global kn_fn_get
kn_fn_get:
	jmp die


.quad 4
.global kn_fn_set
kn_fn_set:
	jmp die
