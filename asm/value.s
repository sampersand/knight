# RDI, RSI, RDX, RCX, R8, R9

.equ KN_VL_INT_TAG, 0
.equ KN_VL_FUNC_TAG, 1
.equ KN_VL_IDENT_TAG, 2
.equ KN_VL_STR_TAG, 3
.equ KN_VL_FALSE_TAG, 4
.equ KN_VL_TRUE_TAG, 5
.equ KN_VL_NULL_TAG, 6

# The first byte of every value is a tag, and the following 7 are garbage.

.macro new_simple label:req, tag:req
.globl \label
\label:
	push %rbx
	mov %rdi, %rbx
	mov $16, %rdi
	call _malloc
	movb $\tag, (%rax)
	mov %rbx, 8(%rax)
	pop %rbx
	ret
.endm

new_simple new_integer, KN_VL_INT_TAG
new_simple new_string, KN_VL_STR_TAG
new_simple new_identifier, KN_VL_IDENT_TAG

.globl new_boolean
new_boolean:
	push %rbx
	mov %rdi, %rbx
	mov $8, %rdi
	call _malloc
	movb $KN_VL_FALSE_TAG, (%rax)
	cmp $0, %rbx
	je 0f
	movb $KN_VL_TRUE_TAG, (%rax)
0:
	pop %rbx
	ret

.globl new_null
new_null:
	sub $8, %rsp
	mov %rdi, %rbx
	mov $16, %rdi
	call _malloc
	movb $KN_VL_NULL_TAG, (%rax)
	add $8, %rsp
	ret

# note that this expects the stream to be in register r12.
.globl new_function
new_function:
	push %rbx
	push %r13
	push %r14

	mov %rdi, %rbx # set rbx to the current function.
	mov -8(%rdi), %r13 # load the arity
	lea 2(%r13), %rdi # # add for function pointer and tag.
	imul $8, %rdi # we allocate in quadwords.
	call _malloc

	movb $KN_VL_FUNC_TAG, (%rax) # set the tag
	mov %rbx, 8(%rax) # set the function pointer
	mov %rax, %rbx # rbx is now our return value.
	lea 8(%rax), %r14 # and r14 is our current arg pos.

# set each argument
0:
	cmp $0, %r13 			# check to see if we're done.
	jz 1f
	add $8, %r14			# add 8 to the destination location.
	dec %r13 			# subtract one from arity
	mov %r12, %rdi			
	call parse_ast		
	mov %rax, (%r14)		# set the value's pointer
	mov %rdi, %r12 			# update the stream.
	jmp 0b 	# go again
1:
	mov %rbx, %rax # update the pointer
	pop %r14
	pop %r13
	pop %rbx
	ret

.globl to_boolean
to_boolean:
	xor %eax, %eax
	movzbl (%rdi), %ecx
	cmp $KN_VL_INT_TAG, %rcx
	je int_to_boolean
	cmp $KN_VL_STR_TAG, %ecx
	je str_to_boolean
	cmp $KN_VL_TRUE_TAG, %ecx
	jne to_boolean_false
to_boolean_true:
	inc %eax
to_boolean_false:
	ret
str_to_boolean:
	mov 8(%rdi), %rcx
	movzbl (%rcx), %ecx
	jmp to_boolean_compare
int_to_boolean:
	mov 8(%rdi), %rcx
to_boolean_compare:
	jrcxz to_boolean_true
	ret



.globl run_value
run_value:
	movzbl (%rdi), %eax
	cmp $KN_VL_FUNC_TAG, %rax
	je 0f
	cmp $KN_VL_IDENT_TAG, %rax
	jne clone_value
	mov 8(%rdi), %rdi # get the identifier's value.
	jmp get_variable # 
0:
	push 8(%rdi) # push the function's return addr.
	add $16, %rdi # set the arguments to the function's arguments.
	ret # go to the function's return addr.


.globl clone_value
clone_value:
	sub $24, %rsp
	mov %rbx, (%rsp)
	mov %rdi, %rbx
	movzbl (%rdi), %eax
	cmp $KN_VL_FUNC_TAG, %eax
	je clone_function
	cmp $KN_VL_IDENT_TAG, %eax
	je clone_string
	cmp $KN_VL_STR_TAG, %eax
	je clone_string
# clone normal value
	mov $16, %rdi
	call _malloc
	movzbl (%rbx), %ecx
	movb %cl, (%rax)
	mov 8(%rbx), %ecx
	mov %ecx, 8(%rax)
clone_value_finalize:
	mov (%rsp), %rbx
	add $24, %rsp
	ret
clone_string:
	mov %r12, 8(%rsp)
	mov $16, %rdi
	call _malloc
	mov %rax, %r12
	movb $KN_VL_STR_TAG, (%r12)
	mov 8(%rbx), %rdi
	sub $8, %rsp
	call _strdup
	add $8, %rsp
	mov %rax, 8(%r12)
	mov %r12, %rax
	mov 8(%rsp), %r12
	jmp clone_value_finalize
clone_function:
	# setup stack
	mov %r12, 8(%rsp)
	mov %r13, 16(%rsp)
	mov 8(%rbx), %r13 # move function ptr over
	mov -8(%r13), %r12 # load arity

	mov %r12, %rax
	add $2, %rax
	imul $8, %rax
	call _malloc # allocate enough memory

	movq $KN_VL_FUNC_TAG, (%rax) # move tag over
	mov %r13, 8(%rax) # move function ptr over
	mov %rax, %r13 # set r13 to ret ptr
0:
	cmp $0, %r12
	je 1f
	call ddebug # TODO: cloning functions
1:
	mov %r13, %rax # restore ret ptr
	mov 8(%rsp), %r12 # restore registers
	mov 16(%rsp), %r13
	jmp clone_value_finalize

.globl free_value
free_value:
	push %rbx
	mov %rdi, %rbx
	mov (%rbx), %rax
	cmp $KN_VL_FUNC_TAG, %bl
	je free_function
	cmp $KN_VL_IDENT_TAG, %bl
	je free_string
	cmp $KN_VL_STR_TAG, %bl
	jne finish_freeing
free_string:
	mov 8(%rbx), %rdi
	call _free
finish_freeing:
	mov %rbx, %rdi
	call _free
	pop %rbx
	ret
free_function:
	push %r13
	push %r14
	mov %rbx, %r14
	add $16, %r14
	mov 8(%rbx), %r13
0:
	cmp $0, %r13
	jz 1f
	dec %r13
	call free_value
	jmp 0b
1:
	pop %r14
	pop %r13
	jmp finish_freeing

invalid_fmt:
	.asciz "unknown value type: '%d'\n"
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
	jz 1f
	dec %r12
	mov (%r13), %rdi
	add $8, %r13
	call kn_value_dump
	jmp 0b
1:
	pop %r13
	pop %r12
	mov 8(%rbx), %rsi
	lea func_stop(%rip), %rdi
	call _printf
	pop %rbx
	ret
