// RDI, RSI, RDX, RCX, R8, R9

.include "debug.s"

.equ FALSE_BITS, 0b0000
.equ NULL_BITS,  0b0010
.equ TRUE_BITS,  0b0100
.equ FUNC_TAG,   0b1000
.equ STRING_TAG, 0b1100
.equ IDENT_TAG,  0b1110
.equ ALLOC_BIT,  0b1000
.equ TAG_MASK,   0b1111
.equ ALLOC_MASK, 0b1001

// abuse the fact that `malloc` will allocate things that are 16-aligned.
// 0...00000 = false
// 0...XXXX1 = 63-bit
// 0...00010 = null
// 0...00100 = true
// X...X1000 = function
// X...X1100 = string
// X...X1110 = ident

.globl kn_value_new_integer
kn_value_new_integer:
	mov %rdi, %rax
	shl %rax
	or $1, %al
	ret

// Create a new boolean.
// 
// if rdi is zero, false is returned. Otherwise, true is returned.
.globl kn_value_new_boolean
kn_value_new_boolean:
	cmp $0, %rdi
	je kn_value_new_false // If the value given is zero, return 'false'.
	// otherwise, fall through and reutnr 'true'.

// Create a true value
.globl kn_value_new_true
kn_value_new_true:
	mov $TRUE_BITS, %eax
	ret

// Create a false value.
.globl kn_value_new_false
kn_value_new_false:
	xor %eax, %eax
	ret

// Create a new null value.
.globl kn_value_new_null
kn_value_new_null:
	mov $NULL_BITS, %eax
	ret

// Creates a new string value from the given string.
//
// The string in `rdi` must have been created via functions in the kn_string file.
.globl kn_value_new_string
kn_value_new_string:
.if DEBUG // ensure that the lower bits are not set.
	mov %edi, %ecx
	and $TAG_MASK, %ecx
	assert_eq $0, %rcx
.endif // DEBUG
	lea STRING_TAG(%rdi), %rax
	ret

// Creates a new identifier.
.globl kn_value_new_identifier
kn_value_new_identifier:
.if DEBUG // ensure that the lower bits are not set.
	mov %edi, %ecx
	and $TAG_MASK, %ecx
	assert_eq $0, %rcx
.endif // DEBUG
	lea IDENT_TAG(%rdi), %rax
	ret

// NOTE: This function should only ever be called from `kn_parse`.
//
// A prereq is that `r12` contains the stream.
.globl kn_value_new_function
kn_value_new_function:
.if DEBUG // ensure that the lower bits are not set.
	mov %edi, %ecx
	and $TAG_MASK, %ecx
	assert_eq $0, %rcx
.endif // DEBUG
	push %rbx
	push %r13
	push %r14

	mov %rdi, %rbx       // Save the function pointer
	movzb -1(%rdi), %r13 // load the arity

	mov %r13, %rdi
	inc %rdi             // add one for the function pointer itself
	imul $8, %rdi        // find the amount of bytes we need to allocate
	call xmalloc         // allocate the memory for the struct

	mov %rbx, (%rax)     // load the instruction pointer over.
	mov %rax, %rbx       // save our malloc pointer.
	mov %rax, %r14       // and load the "next position" pointer
0:
	cmp $0, %r13         // if we have no arguments left...
	jz 1f                // ...then we have nothing left to parse, and jump to the end.
	dec %r13             // otherwise, subtract one from argc,
	add $8, %r14         // and add one to the next pointer register.
	mov %r12, %rdi
	call kn_parse        // fetch the next stream
	mov %rax, (%r14)     // store the ast we just read
	mov %rdi, %r12       // the handle_stream function returns the stream in rdi; a bit of a hack...
	jmp 0b               // and
1:
	lea FUNC_TAG(%rbx), %rax // load the return value back
	pop %r14             // and restore the registers
	pop %r13
	pop %rbx
	ret
// 
// // note that this expects the stream to be in register r12.
// .globl kn_value_new_function
// kn_value_new_function:
// 	push %rbx
// 	push %r13
// 	push %r14
// 
// 	mov %rdi, %rbx // set rbx to the current function.
// 	mov -8(%rdi), %r13 // load the arity
// 	lea 2(%r13), %rdi // // add for function pointer and tag.
// 	imul $8, %rdi // we allocate in quadwords.
// 	call _malloc
// 
// 	movb $KN_VL_FUNC_TAG, (%rax) // set the tag
// 	mov %rbx, 8(%rax) // set the function pointer
// 	mov %rax, %rbx // rbx is now our return value.
// 	lea 8(%rax), %r14 // and r14 is our current arg pos.
// 
// // set each argument
// 0:
// 	cmp $0, %r13 			// check to see if we're done.
// 	jz 1f
// 	add $8, %r14			// add 8 to the destination location.
// 	dec %r13 			// subtract one from arity
// 	mov %r12, %rdi			
// 	call kn_parse		
// 	mov %rax, (%r14)		// set the value's pointer
// 	mov %rdi, %r12 			// update the stream.
// 	jmp 0b 	// go again
// 1:
// 	mov %rbx, %rax // update the pointer
// 	pop %r14
// 	pop %r13
// 	pop %rbx
// 	ret
// 

.globl kn_value_run
kn_value_run:
	mov %edi, %eax
	and $ALLOC_MASK, %eax
	cmp $ALLOC_BIT, %eax    // check to see if we are an allocated type
	je 0f                   // if so, continue onwards
	mov %rdi, %rax          // otherwise, just copy the immediate value.
	ret
0: // string, ident, function
	mov %edi, %eax
	and $~TAG_MASK, %rdi
	and $TAG_MASK, %eax
	cmp $STRING_TAG, %eax
	jne 0f                   // if we are not a string, continue onwards
	sub $8, %rsp
	call kn_string_clone     // duplicate the string
	add $8, %rsp
	mov %rax, %rdi
	jmp kn_value_new_string   // create a new string with the return value.
0: // ident, function
	cmp $IDENT_TAG, %eax      // check to see if we are an ident
	cmove (%rdi), %rdi        // if we are, fetch the ident, and run it
	je kn_env_get
	assert_eq $FUNC_TAG, %rax // sanity check


.globl value_to_boolean
value_to_boolean:
	// Short circuit: If the value is `0` (ie false), `1` (ie the number zero), or `2` (ie
	// `null`), then we return false. otherwise, return true.
	mov %dil, %al
	and $ALLOC_MASK, %al // TODO: remove me ?
	cmp $ALLOC_BIT, %al
	je 0f
	// otherwise, literal values are true if theyre > 2
	xor %eax, %eax
	cmp $2, %cl
	seta %al
	ret
0: // not a literal, ie either a string or ident/func
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	jne 1f
	mov (%rdi), %rdi // deref the string struct ptr
	movzb (%rdi), %eax // deref the first byte, return if its zero or not.
	ret
1: // must run the value to get the result.
	sub $8, %rsp
	call value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp value_to_boolean // do it over again.

.globl value_to_integer
value_to_integer:
	// Short circuit: If the value is `0` (ie false), `1` (ie the number zero), or `2` (ie
	// `null`), then we return false. otherwise, return true.
	mov %dil, %al
	and $1, %al
	cmp $1, %al
	je 0f
	mov %dil, %cl
	cmp $ALLOC_BIT, %cl // if the allocated bit is set, jmp to not a literal.
	jae 1f
	and $1, %cl
	jne 0f // optimize for the path of being an integer
	shr %rdi
	mov %rdi, %rax
	ret
0: // a literal, but not an integer
	shr %dil
	or $1, %dil
	movzb %dil, %ecx
	ret
1: // either a string or a value that needs to be run.
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	jne 2f // if we are not a string, we must evaluate it.
	and $~TAG_MASK, %rdi // remove the tag so we can deref it.
	mov (%rdi), %rdi // deref the string struct ptr
	jmp _strtoll
2: // must run the value to get the result.
	sub $8, %rsp
	call value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp value_to_integer // do it over again.

.globl value_to_string
value_to_string:
	mov %dil, %al
	and $1, %al
	cmp $1, %al
	jne 0f
	call die // todo: int to string
0: // check for allocataed
	mov %dil, %cl
	cmp $ALLOC_BIT, %cl // if the allocated bit is set, jmp to not a literal.
	jae 1f
0: // check for false
	cmp $FALSE_BITS, %dil
	jne 0f
	mov kn_string_false(%rip), %rax
	ret
0: // check for true
	cmp $TRUE_BITS, %dil
	jne 0f
	mov kn_string_true(%rip), %rax
	ret
0: // we must be null here.
	mov kn_string_null(%rip), %rax
	ret
1: // either a string or a value that needs to be run.
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	jne 2f // if weare not a string, we must evaluate it.
	sub $STRING_TAG, %rdi // remove tag so we can pass it to clone correctly.
	jmp kn_string_clone
2: // must run the value to get the result.
	sub $8, %rsp
	call value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp value_to_string // do it over again.

.globl value_run
value_run:
	mov %dil, %al
	and $1, %al
	cmp $1, %al
	je 0f
	mov %dil, %cl
	cmp $ALLOC_BIT, %cl // if the allocated bit is set, jmp to not a literal.
	jae 1f
0:
	mov %rdi, %rax
	ret // do not need to run literal values.
1: // string or ident or function
	and $~TAG_MASK, %rdi // remove the tag
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	jne 2f // if it is not a string, go onwards
	sub $8, %rsp
	call kn_string_clone // duplicate the string
	add $8, %rsp
	jmp kn_value_new_string // then return the new string
2: // ident or function
	cmp $IDENT_TAG, %cl
	je kn_env_get // if we are an ident, get the variable
	// otherwise, execute the function.
	push 8(%rdi)
	add $16, %rdi
	ret

.globl value_clone
value_clone:
	mov %dil, %al
	and $1, %al
	cmp $1, %al
	je 0f
	mov %dil, %cl
	cmp $ALLOC_BIT, %cl // if the allocated bit is set, jmp to not a literal.
	jae 1f
0:
	mov %rdi, %rax
	ret // do not need to clone literal values.
1: // string or ident or function
	sub $8, %rsp
	and $~TAG_MASK, %rdi // remove the tag
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	jne 2f // if it is not a string, go onwards
	call kn_string_clone // duplicate the string
	add $8, %rsp
	jmp kn_value_new_string // then return the new string
2: // ident or function
	cmp $IDENT_TAG, %cl
	jne 3f
	call _strdup
	add $8, %rsp
	jmp kn_value_new_identifier
3:
	call die

.globl kn_value_free
kn_value_free:
	mov %dil, %cl
	cmp $ALLOC_BIT, %cl // if the allocated bit is set, jmp to not a literal.
	jae 0f
	ret // do not need to free literal values.
0: // string or ident or function
	and $~TAG_MASK, %rdi // remove the tag
	and $TAG_MASK, %cl
	cmp $STRING_TAG, %cl
	je kn_string_free
	cmp $IDENT_TAG, %cl
	je _free
	call die // we are a function
	
// 
// .globl kn_value_free
// kn_value_free:
// 	push %rbx
// 	mov %rdi, %rbx
// 	mov (%rdi), %rax
// 	cmp $KN_VL_FUNC_TAG, %bl
// 	je free_function
// 	cmp $KN_VL_IDENT_TAG, %bl
// 	je free_string
// 	cmp $KN_VL_STR_TAG, %bl
// 	jne finish_freeing
// free_string:
// 	mov 8(%rbx), %rdi
// 	call kn_string_free
// finish_freeing:
// 	mov %rbx, %rdi
// 	call _free
// 	pop %rbx
// 	ret
// free_function:
// 	push %r13
// 	push %r14
// 	mov %rbx, %r14
// 	add $16, %r14
// 	mov 8(%rbx), %r13
// 0:
// 	cmp $0, %r13
// 	jz 1f
// 	dec %r13
// 	call kn_value_free
// 	jmp 0b
// 1:
// 	pop %r14
// 	pop %r13
// 	jmp finish_freeing
// 



.globl kn_value_dump
kn_value_dump:
	mov %rdi, %rsi
	test $1, %sil

	jz 0f
	shr %rsi
	lea num_fmt(%rip), %rdi
	jmp _printf
0: // true, false, null, string, ident, function
	cmp $TRUE_BITS, %rsi
	jne 1f
	lea true_fmt(%rip), %rdi
	jmp 2f
1:
	cmp $FALSE_BITS, %rsi
	jne 1f
	lea false_fmt(%rip), %rdi
	jmp 2f
1:
	cmp $NULL_BITS, %rsi
	jne 1f
	lea null_fmt(%rip), %rdi
2:
	jmp _printf
1:
0: // string, ident, function
	mov %sil, %al
	and $TAG_MASK, %al
	cmp $IDENT_TAG, %al
	jne 0f
	and $~TAG_MASK, %rsi
	lea ident_fmt(%rip), %rdi
	jmp _printf
0: // string, function
	cmp $STRING_TAG, %al
	jne 0f
	and $~TAG_MASK, %rsi
	lea string_fmt(%rip), %rdi
	mov (%rsi), %rsi
	jmp _printf
0: // function
	cmp $FUNC_TAG, %al
	jne 0f
	push %rbx

	// first, print the start
	and $~TAG_MASK, %rsi
	mov %rsi, %rbx
	lea kn_func_start(%rip), %rdi
	call _printf

	push %r12
	push %r13

	mov (%rbx), %rsi
	movzb -1(%rsi), %r12
	lea 8(%rbx), %r13
1:
	cmp $0, %r12
	je 2f
	dec %r12
	mov (%r13), %rdi
	call kn_value_dump
	add $8, %r13
	jmp 1b
2:
	// then print the end
	pop %r13
	pop %r12
	mov %rbx, %rsi
	pop %rbx
	lea kn_func_stop(%rip), %rdi
	jmp _printf
0: // unknown
	lea invalid_fmt(%rip), %rdi
	call abort

.data:
invalid_fmt: .asciz "unknown value type: '%d'\n"
kn_func_start: .asciz "Func(%p):\n"
kn_func_stop: .asciz "Func(%p)/\n"
num_fmt: .asciz "Number(%ld)\n"
string_fmt: .asciz "String(%s)\n"
ident_fmt: .asciz "Ident(%s)\n"
true_fmt: .asciz "True\n"
false_fmt: .asciz "False\n"
null_fmt: .asciz "Null\n"
// invalid_fmt:
// 	.asciz "unknown value type: '%d'\n"
// .globl kn_value_dump
// kn_value_dump:
// 	push %rbx
// 	mov %rdi, %rbx
// 	movzbl (%rdi), %eax
// 	cmp $KN_VL_INT_TAG, %al
// 	je Ldump_number
// 	cmp $KN_VL_FUNC_TAG, %al
// 	je Ldump_func
// 	cmp $KN_VL_STR_TAG, %al
// 	je Ldump_string
// 	cmp $KN_VL_IDENT_TAG, %al
// 	je Ldump_ident
// 	cmp $KN_VL_TRUE_TAG, %al
// 	je Ldump_true
// 	cmp $KN_VL_FALSE_TAG, %al
// 	je Ldump_false
// 	cmp $KN_VL_NULL_TAG, %al
// 	je Ldump_null
// 	mov %rax, %rsi
// 	lea invalid_fmt(%rip), %rdi
// 	call _printf
// 	jmp die
// 
// num_fmt: .asciz "Number(%ld)\n"
// Ldump_number:
// 	mov 8(%rdi), %rsi
// 	lea num_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// string_fmt: .asciz "String(%s)\n"
// Ldump_string:
// 	mov 8(%rdi), %rsi
// 	mov (%rsi), %rsi
// 	lea string_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// ident_fmt: .asciz "Ident(%s)\n"
// Ldump_ident:
// 	mov 8(%rdi), %rsi
// 	lea ident_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// true_fmt: .asciz "True\n"
// Ldump_true:
// 	lea true_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// false_fmt: .asciz "False\n"
// Ldump_false:
// 	lea false_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// null_fmt: .asciz "Null\n"
// Ldump_null:
// 	lea null_fmt(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
// 
// // RDI, RSI, RDX, RCX, R8, R9
// kn_func_start: .asciz "Func(%p):\n"
// kn_func_stop: .asciz "Func(%p)/\n"
// Ldump_func:
// 	mov 8(%rbx), %rsi
// 	lea kn_func_start(%rip), %rdi
// 	call _printf
// 
// 	push %r12
// 	push %r13
// 	mov 8(%rbx), %rsi
// 	mov %rbx, %r13
// 	add $16, %r13
// 	mov -8(%rsi), %r12
// 0:
// 	cmp $0, %r12
// 	jz 1f
// 	dec %r12
// 	mov (%r13), %rdi
// 	add $8, %r13
// 	call kn_value_dump
// 	jmp 0b
// 1:
// 	pop %r13
// 	pop %r12
// 	mov 8(%rbx), %rsi
// 	lea kn_func_stop(%rip), %rdi
// 	call _printf
// 	pop %rbx
// 	ret
