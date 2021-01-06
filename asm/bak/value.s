# RDI, RSI, RDX, RCX, R8, R9

.equ KN_VL_INT_TAG, 0
.equ KN_VL_INT_SIZE, 16
.equ KN_VL_INT_VAL_OFFSET, 8

.equ KN_VL_FUNC_TAG, 1
.equ KN_VL_FUNC_BASE_SIZE_DIV_TWO, 16
.equ KN_VL_FUNC_PTR_OFFSET, 8
.equ KN_VL_FUNC_ARG0_OFFSET, 16

.equ KN_VL_IDENT_TAG, 2
.equ KN_VL_IDENT_SIZE, 16
.equ KN_VL_IDENT_PTR_OFFSET, 8

.equ KN_VL_STR_TAG, 3
.equ KN_VL_STR_SIZE, 16
.equ KN_VL_STR_PTR_OFFSET, 8

.equ KN_VL_CONST_SIZE, 8
.equ KN_VL_FALSE_TAG, 4
.equ KN_VL_TRUE_TAG, 5
.equ KN_VL_NULL_TAG, 6

.globl kn_value_new_int
kn_value_new_int:
	push %rbx
	mov %rdi, %rbx
	mov $KN_VL_INT_SIZE, %rdi
	call _malloc
	movb $KN_VL_INT_TAG, (%rax)
	mov %rbx, KN_VL_INT_VAL_OFFSET(%rax)
	pop %rbx
	ret

.globl kn_value_new_str
kn_value_new_str:
	push %rbx
	mov %rdi, %rbx
	mov $KN_VL_STR_SIZE, %rdi
	call _malloc
	movb $KN_VL_STR_TAG, (%rax)
	mov %rbx, KN_VL_STR_PTR_OFFSET(%rax)
	pop %rbx
	ret


# Creates a new identifier value.
# ARGUMENTS:
# 	rdi: A pointer to an identifier string.
.globl kn_value_new_ident
kn_value_new_ident:
	push %rbx
	mov %rdi, %rbx
	mov $KN_VL_IDENT_SIZE, %rdi
	call _malloc
	movb $KN_VL_IDENT_TAG, (%rax)
	mov %rbx, KN_VL_IDENT_PTR_OFFSET(%rax)
	pop %rbx
	ret

.globl kn_value_new_bool
kn_value_new_bool:
	push %rbx
	mov %rdi, %rbx
	mov $KN_VL_CONST_SIZE, %rdi
	call _malloc
	movb $KN_VL_FALSE_TAG, (%rax)
	cmp $0, %rbx
	je 0f
	movb $KN_VL_TRUE_TAG, (%rax)
0:
	pop %rbx
	ret

.globl kn_value_new_null
kn_value_new_null:
	sub $8, %rsp
	mov %rdi, %rbx
	mov $KN_VL_CONST_SIZE, %rdi
	call _malloc
	movb $KN_VL_NULL_TAG, (%rax)
	add $8, %rsp
	ret

# note that this expects the stream to be in register r12.
.globl kn_value_new_func
kn_value_new_func:
	push %rbx
	push %r13
	push %r14
	mov %rdi, %rbx # set rbx to the current function.

	mov -8(%rdi), %r13 # load the arity
	mov %r13, %rdi
	add $KN_VL_FUNC_BASE_SIZE_DIV_TWO, %rdi # add for function pointer and tag.
	imul $8, %rdi # we allocate in quadwords.
	call _malloc

	movb $KN_VL_FUNC_TAG, (%rax) # set the tag
	mov %rbx, KN_VL_FUNC_PTR_OFFSET(%rax) # set the function pointer
	mov %rax, %rbx # rbx is now our return value.
	mov %rax, %r14 # and r14 is our current arg pos.
	add $KN_VL_FUNC_PTR_OFFSET, %r14 # we add 8 within the cmp loop

# set each argument
parse_next_argument:
	cmp $0, %r13 			# check to see if we're done.
	jz done_creating_arguments
	add $8, %r14			# add 8 to the destination location.
	dec %r13 			# subtract one from arity
	mov %r12, %rdi			
	call parse_ast			
	mov %rax, (%r14)		# set the value's pointer
	mov %rdi, %r12 			# update the stream.
	jmp parse_next_argument 	# go again

done_creating_arguments:
	mov %rbx, %rax # update the pointer
	pop %r14
	pop %r13
	pop %rbx
	ret

invalid_fmt: .asciz "unknown value type: '%d'\n"

.global kn_value_dump
kn_value_dump:
	push %rbx
	mov %rdi, %rbx
	movzbl (%rdi), %eax
	cmp $KN_VL_INT_TAG, %al
	je Ldump_number
	cmp $KN_VL_FUNC_TAG, %al
	je Ldump_func
	cmp $KN_VL_STR_TAG, %al
	je Ldump_string
	cmp $KN_VL_IDENT_TAG, %al
	je Ldump_ident
	cmp $KN_VL_TRUE_TAG, %al
	je Ldump_true
	cmp $KN_VL_FALSE_TAG, %al
	je Ldump_false
	cmp $KN_VL_NULL_TAG, %al
	je Ldump_null
	mov %rax, %rsi
	lea invalid_fmt(%rip), %rdi
	call _printf
	jmp die

num_fmt: .asciz "Number(%ld)\n"
Ldump_number:
	mov 8(%rdi), %rsi
	lea num_fmt(%rip), %rdi
	pop %rbx
	call _printf
	ret

string_fmt: .asciz "String(%s)\n"
Ldump_string:
	mov 8(%rdi), %rsi
	lea string_fmt(%rip), %rdi
	call _printf
	pop %rbx
	ret

ident_fmt: .asciz "Ident(%s)\n"
Ldump_ident:
	mov 8(%rdi), %rsi
	lea ident_fmt(%rip), %rdi
	call _printf
	pop %rbx
	ret

true_fmt: .asciz "True\n"
Ldump_true:
	lea true_fmt(%rip), %rdi
	call _printf
	pop %rbx
	ret

false_fmt: .asciz "False\n"
Ldump_false:
	lea false_fmt(%rip), %rdi
	call _printf
	pop %rbx
	ret

null_fmt: .asciz "Null\n"
Ldump_null:
	lea null_fmt(%rip), %rdi
	call _printf
	pop %rbx
	ret

# RDI, RSI, RDX, RCX, R8, R9
func_start: .asciz "Func(%p):\n"
func_stop: .asciz "Func(%p)/\n"
Ldump_func:
	mov 8(%rbx), %rsi
	lea func_start(%rip), %rdi
	call _printf

	push %r12
	push %r13
	mov 8(%rbx), %rsi
	mov %rbx, %r13
	add $16, %r13
	mov -8(%rsi), %r12
0:
	cmp $0, %r12
	jz done_with_func_args
	dec %r12
	mov (%r13), %rdi
	add $8, %r13
	call kn_value_dump
	jmp 0b


done_with_func_args:
	pop %r13
	pop %r12
	mov 8(%rbx), %rsi
	lea func_stop(%rip), %rdi
	call _printf
	pop %rbx
	ret
