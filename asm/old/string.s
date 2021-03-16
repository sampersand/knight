/* Create a new string */
.globl kn_string_new
kn_string_new:
	push %rbx
	mov %rdi, %rbx   // preserve the string passed to us.

	mov $12, %edi    // 8 bytes for string ptr, 4 for refcount
	call xmalloc     // create a new string struct
	mov %rbx, (%rax) // move string ptr over
	movl $1, 8(%rax) // set refc to 1.

	pop %rbx
	ret

// free the memory associated with a string.
.globl kn_string_free
kn_string_free:
	cmpl $0, 8(%rdi)
	jz 0f             // if it is an interned string, dont free it.
	decl 8(%rdi)      // otherwise, subtract one from the rc
	jz 1f             // and if we are at zero, free the string
0:
	ret
1:
	push %rbx
	mov %rdi, %rbx
	mov (%rdi), %rdi
	call _free        // first, free the associated string.
	mov %rbx, %rdi
	pop %rbx
	jmp _free         // now free the entire string struct.


// .globl kn_string_free
// kn_string_free:
// 	movl 8(%rdi), %ecx
// 	jecxz 0f		// if it is an interned string, dont free it.
// 	retn
// 	dec %ecx		// otherwise, subtract one from the rc
// 	jecxz 1f		// and if we are at zero, free the string
// 	mov %ecx, 8(%rdi)	// otherwise, put the updated rc back and return.
// 0:
// 	ret
// 1:
// 	push %rbx
// 	mov %rdi, %rbx
// 	mov (%rdi), %rdi
// 	call _free		// first, free the associated string.
// 	mov %rbx, %rdi
// 	pop %rbx
// 	jmp _free               // now free the entire string struct.

// Duplicate the string
.globl kn_string_clone
kn_string_clone:
	mov %rdi, %rax
	cmpl $0, 8(%rdi) // check to see if we are interned
	jne 0f
	incq 8(%rdi)      // if we are not, add one to the refcount.
0:
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
