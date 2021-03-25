.data
source:
	/* .asciz "; = a 3 : O + 'a*4=' * a 4" */
	.asciz "i"
# 	.asciz "123'abdqw\"
# '"#\'

.globl _main
.text
_main:
	push %rbx

	call process_arguments    // process command line arguments
	mov %rax, %rbx
	call kn_startup        // Start up knight

	xor %eax, %eax
	pop %rbx; ret

	lea source(%rip), %rdi
	call kn_parse
	mov %rax, %rdi
	call kn_vl_dump
/*
	# mov %rbx, %rdi
	# call _strlen
	# mov %rax, %rdi
	mov $4, %rdi
	call kn_str_alloc

	mov %rax, %rdi
	mov %rax, %rbx

	call kn_str_deref
	mov %rax, %rdx
	lea source(%rip), %rsi
	call _strcpy
	mov %rax, %rsi
	lea printf1(%rip), %rdi
	call _printf

	mov %rbx, %rax
	call kn_str_free


	call kn_parse

	mov %rax, %rsi
	lea printf1(%rip), %rdi

	call _printf

	#call kn_vl_dump

	call kn_run
*/
	xor %eax, %eax
	pop %rbx
	ret

process_arguments:
	; mov 16(%rsi), %rdi
	lea source(%rip), %rdi
	jmp _strdup
