/*
string layout:
[1 byte] 0 allocated, 1 for embedded
emebdded:
	[1 byte] length
	[2 bytes] padding
	[4 bytes] refcount
	[23 bytes] embedded
	[1 byte] '\0'
allocated:
	[3 bytes] padding
	[4 bytes] refcount (note same position as embedded)
	[8 bytes] pointer to string
	[8 bytes] length
	[8 bytes] padding
*/

.equ KN_STR_ALLOC, 0
.equ KN_STR_EMBED, 1
.equ KN_STR_EMBED_LENGTH, 23
.equ KN_STR_SIZE, 32
.equ KN_STR_OFFSET_RC, 4
.equ KN_STR_E_OFFSET_DATA, 8
.equ KN_STR_E_OFFSET_LEN, 1
.equ KN_STR_A_OFFSET_PTR, 8
.equ KN_STR_A_OFFSET_LEN, 16

/* create a new string */
.globl kn_str_alloc
kn_str_alloc:
	push %rbx
	mov %rdi, %rbx // preserve the length

	/* Allocate the struct. */
	mov $KN_STR_SIZE, %edi
	call xmalloc
	movw $1, KN_STR_OFFSET_RC(%rax)

	/* Check to see if we are embedded. */
	cmp $KN_STR_EMBED_LENGTH, %rbx
	jg 0f

	/* Setup the length and flag for embedded strings. */
	movb $KN_STR_EMBED, (%rax)
	movb %bl, KN_STR_E_OFFSET_LEN(%rax)
	pop %rbx
	ret

0:	/* Setup the length, flag, and refcount for allocated strings. */
	movb $KN_STR_ALLOC, (%rax)
	mov %ebx, KN_STR_A_OFFSET_LEN(%rax)

	/* Allocate the pointer and assign it. */
	mov %rbx, %rdi
	inc %rdi /* for trailing `\0` */
	mov %rax, %rbx /* preserve the struct return value */
	call xmalloc
	movq %rax, KN_STR_A_OFFSET_PTR(%rbx)

	/* Cleanup */
	mov %rbx, %rax
	pop %rbx
	ret

/* Gets the length of a string */
.globl kn_str_length
kn_str_length:
	cmpb $KN_STR_ALLOC, (%rdi)
	je 0f
	movzb KN_STR_E_OFFSET_LEN(%rdi), %eax
	ret
0:
	movq KN_STR_A_OFFSET_LEN(%rip), %rax
	ret

/* Gets a pointer to the data of a string. */
.globl kn_str_deref
kn_str_deref:
        cmpb $KN_STR_ALLOC, (%rdi)
        je 0f
        lea KN_STR_E_OFFSET_DATA(%rdi), %rax
        ret
0:
	mov KN_STR_A_OFFSET_PTR(%rdi), %rax
	ret

/* free the memory associated with a string. */
.globl kn_str_free
kn_str_free:
	decw KN_STR_OFFSET_RC(%rdi)
	jz 0f
	ret
0: /* we dont have any more pointers to it */
	cmpb $KN_STR_EMBED, (%rdi)
	je _free /* if we don't have a `ptr` to get rid of, just free this struct. */

	/* Free the allocated pointer */
	push %rbx
	mov %rdi, %rbx
	mov KN_STR_A_OFFSET_PTR(%rdi), %rdi
	call _free
	mov %rbx, %rdi
	pop %rbx
	jmp _free

/* clones a string. */
.globl kn_str_clone
kn_str_clone:
	incb KN_STR_OFFSET_RC(%rdi)
	mov %rdi, %rax
	ret

.data

.balign 16
.globl kn_string_empty
kn_string_empty:
	.quad string_intern_empty
	.quad 0

.balign 16
.globl kn_string_true
kn_string_true:
	.quad string_intern_true
	.quad 0

.balign 16
.globl kn_string_false
kn_string_false:
	.quad string_intern_false
	.quad 0

.balign 16
.globl kn_string_null
kn_string_null:
	.quad string_intern_null
	.quad 0

.balign 16
.globl kn_string_zero
kn_string_zero:
	.quad string_intern_zero
	.quad 0

string_intern_empty: .asciz ""
string_intern_true:  .asciz "true"
string_intern_false: .asciz "false"
string_intern_null:  .asciz "null"
string_intern_zero:  .asciz "0"
