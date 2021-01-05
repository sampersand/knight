# RDI, RSI, RDX, RCX, R8, R9
.data
fmt:
	.asciz "arg = %s (%d bytes)\n"
fmt2:
	.asciz "arg = %c\n"

.text
.globl _main, parse_ast
_main:

	sub $8, %rsp
	mov 16(%rsi), %rdi
	mov %rdi, (%rsp)
	call parse_ast
	call parse_ast
	call parse_ast
	call parse_ast

	add $8, %rsp
	xor %eax, %eax
	ret

.globl die
die:
	mov $192, %rdi
	call _exit
# # RDI, RSI, RDX, RCX, R8, R9
# 	.data
# fmt:
# 	.asciz "arg = %s (%d bytes)\n"
# 
# .macro foo
# .endm
# 
# 	.text
# 	.globl _main
# _main:
# 	push %rsi
# 	mov 16(%rsi), %rdi
# 	call _strlen
# 	mov %rax, %rdx
# 	mov (%rsp), %rsi
# 	mov 16(%rsi), %rsi
# 	lea fmt(%rip), %rdi
# 	call _printf
# 	add $8, %rsp
# 	xor %eax, %eax
# 	ret
