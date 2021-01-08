# Create a new string
foo1:
	.asciz "<'%s'>\n"

.globl string_new
string_new:
	push %rbx
	mov %rdi, %rbx
	mov %rdi, %rsi
	lea foo1(%rip), %rdi
	call _printf

	mov $12, %edi		# 8 bytes for string ptr, 4 for refcount
	call _malloc
	mov %rbx, (%rax)	# move string ptr over
	movl $1, 8(%rax)	# set refc to 1.
	pop %rbx
	ret

# free a string's memory
.globl string_free
string_free:
	mov 8(%rdi), %ecx
	jrcxz 0f		# if it is an interned string, dont free it.
	dec %ecx		# otherwise, subtract one from the rc
	jrcxz 1f		# and if we are at zero, free the string
	mov %ecx, 8(%rdi)	# otherwise, put the updated rc back and return.
0:
	ret
1:
	push %rbx
	mov %rdi, %rbx
	mov (%rdi), %rdi
	call _free		# first, free the associated string.
	mov %rbx, %rdi
	call _free		# now free the entire string struct.
	pop %rbx
	ret

# .globl string_clone
# string_clone:
# 	mov 8(%rdi), %ecx	# get the ref count
# 	jrcxz 0f		# if it is zero, ie interned, we are done.
# 	incl 8(%rdi)		# increased the ref count.
# 0:
# 	ret

.globl string_clone
string_clone:
	mov 8(%rdi), %ecx	# get the ref count
	cmp $0, %ecx
	sete %al
	add %eax, 8(%rdi)
	ret

.data
.globl string_empty
string_empty:
	.quad string_intern_empty
	.quad 0
.globl string_true
string_true:
	.quad string_intern_true
	.quad 0
.globl string_false
string_false:
	.quad string_intern_false
	.quad 0
.globl string_null
string_null:
	.quad string_intern_null
	.quad 0
.globl string_zero
string_zero:
	.quad string_intern_zero
	.quad 0

string_intern_empty:
	.asciz ""
string_intern_true:
	.asciz "true"
string_intern_false:
	.asciz "false"
string_intern_null:
	.asciz "null"
string_intern_zero:
	.asciz "0"
