# RDI, RSI, RDX, RCX, R8, R9

.quad 0
.global kn_fn_prompt
kn_fn_prompt:
	call die

.quad 0
.global kn_fn_random
kn_fn_random:
	call die

.quad 0
.global kn_fn_true
kn_fn_true:
	call ddebug

.quad 0
.global kn_fn_false
kn_fn_false:
	call die

.quad 0
.global kn_fn_null
kn_fn_null:
	call die


.quad 1
.global kn_fn_block
kn_fn_block:
	call die

.quad 1
.global kn_fn_eval
kn_fn_eval:
	call die

.quad 1
.global kn_fn_call
kn_fn_call:
	call die

.quad 1
.global kn_fn_system
kn_fn_system:
	call die

.quad 1
.global kn_fn_quit
kn_fn_quit:
	call die

.quad 1
.global kn_fn_not
kn_fn_not:
	mov (%rdi), %rdi
	call run_value
	mov %rax, %rdi
	call to_boolean
	mov %rax, %rdi
	jmp new_boolean

.quad 1
.global kn_fn_length
kn_fn_length:
	call die

.quad 1
.global kn_fn_output
kn_fn_output:
	call die


.quad 2
.global kn_fn_add
kn_fn_add:
	call die

.quad 2
.global kn_fn_sub
kn_fn_sub:
	call die

.quad 2
.global kn_fn_mul
kn_fn_mul:
	call die

.quad 2
.global kn_fn_div
kn_fn_div:
	call die

.quad 2
.global kn_fn_mod
kn_fn_mod:
	call die

.quad 2
.global kn_fn_pow
kn_fn_pow:
	call die

.quad 2
.global kn_fn_eql
kn_fn_eql:
	call die

.quad 2
.global kn_fn_lth
kn_fn_lth:
	call die

.quad 2
.global kn_fn_gth
kn_fn_gth:
	call die

.quad 2
.global kn_fn_then
kn_fn_then:
	call die

.quad 2
.global kn_fn_assign
kn_fn_assign:
	call die

.quad 2
.global kn_fn_while
kn_fn_while:
	call die

.quad 2
.global kn_fn_and
kn_fn_and:
	call die

.quad 2
.global kn_fn_or
kn_fn_or:
	call die


.quad 3
.global kn_fn_if
kn_fn_if:
	call die

.quad 3
.global kn_fn_get
kn_fn_get:
	call die


.quad 4
.global kn_fn_set
kn_fn_set:
	call die
