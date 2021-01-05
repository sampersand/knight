# RDI, RSI, RDX, RCX, R8, R9

.equ KN_VL_INT_TAG, 0
.equ KN_VL_INT_SIZE, 16
.equ KN_VL_INT_VAL_OFFSET, 8

.equ KN_VL_FUNC_TAG, 1
.equ KN_VL_FUNC_BASE_SIZE, 16
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

.globl kn_value_new_func
kn_value_new_func:
	push %rbx
	mov %rdi, %rbx
	mov -8(%rdi), %rdi # load the arity
	add $2, %rdi # add for function pointer and tag.
	#(this shouldnt be commented out) mul $8, %rdi # we allocate in quadwords.
	call _malloc
	movb $KN_VL_FUNC_TAG, (%rax)
	mov %rbx, 8(%rax)
	jmp die
	mov -8(%rbx), %rcx
0:
	cmp $0, %rcx
	je 1f

1:
	mov %rbx, 8(%rax)
	pop %rbx
	ret


.global kn_value_dump
kn_value_dump:
	push %rbx
	movzbl (%rdi), %eax
	cmp $KN_VL_INT_TAG, %eax
	je Ldump_number
	cmp $KN_VL_STR_TAG, %eax
	je Ldump_string
	cmp $KN_VL_IDENT_TAG, %eax
	je Ldump_ident
	cmp $KN_VL_TRUE_TAG, %eax
	je Ldump_true
	cmp $KN_VL_FALSE_TAG, %eax
	je Ldump_false
	cmp $KN_VL_NULL_TAG, %eax
	je Ldump_null

invalid_fmt: .asciz "unknown value type %d\n"
	mov %rax, %rsi
	lea invalid_fmt(%rip), %rdi
	call _printf
	jmp die

num_fmt: .asciz "Number(%ld)\n"
Ldump_number:
	mov 8(%rdi), %rsi
	lea num_fmt(%rip), %rdi
	call _printf
	pop %rbx
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
