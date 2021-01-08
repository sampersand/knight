# RDI, RSI, RDX, RCX, R8, R9

.quad 0
.global func_prompt
func_prompt:
	call die

.quad 0
.global func_random
func_random:
	sub $8, %rsp
	call _random
	add $8, %rsp
	mov %rax, %rdi
	jmp value_new_integer

.quad 0
.global func_true
func_true:
	mov $1, %edi
	jmp value_new_boolean

.quad 0
.global func_false
func_false:
	mov $0, %edi
	jmp value_new_boolean

.quad 0
.global func_null
func_null:
	jmp value_new_null

.quad 1
.global func_block
func_block:
	call die

.quad 1
.global func_eval
func_eval:
	call die

.quad 1
.global func_call
func_call:
	call die

.quad 1
.global func_system
func_system:
	call die

.quad 1
.global func_quit
func_quit:
	call die

.quad 1
.global func_not
func_not:
	push %rbx
	mov (%rdi), %rbx
	mov (%rdi), %rdi
	call value_run
	mov %rax, %rdi
	call value_to_boolean

	mov %rbx, %rdi
	test %rax, %rax
	setne %al
	movzbl %al, %ebx
	call value_free # with the `edi` up above

	mov %rbx, %rdi
	pop %rbx
	jmp value_new_boolean

.quad 1
.global func_length
func_length:
	#call die
	mov (%rdi), %rdi
	call value_run
	mov %rax, %rdi
	call value_to_integer
	mov %rax, %rdi
	jmp value_new_integer


.quad 1
.global func_output
func_output:
	call die


.quad 2
.global func_add
func_add:
	call die

.quad 2
.global func_sub
func_sub:
	call die

.quad 2
.global func_mul
func_mul:
	call die

.quad 2
.global func_div
func_div:
	call die

.quad 2
.global func_mod
func_mod:
	call die

.quad 2
.global func_pow
func_pow:
	call die

.quad 2
.global func_eql
func_eql:
	call die

.quad 2
.global func_lth
func_lth:
	call die

.quad 2
.global func_gth
func_gth:
	call die

.quad 2
.global func_then
func_then:
	call die

.quad 2
.global func_assign
func_assign:
	call die

.quad 2
.global func_while
func_while:
	call die

.quad 2
.global func_and
func_and:
	call die

.quad 2
.global func_or
func_or:
	call die


.quad 3
.global func_if
func_if:
	call die

.quad 3
.global func_get
func_get:
	call die


.quad 4
.global func_set
func_set:
	call die
