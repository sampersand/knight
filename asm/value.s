// RDI, RSI, RDX, RCX, R8, R9

.include "debug.s"
.include "value_header.s"

/* Creates a new number. */
.globl kn_value_new_number
kn_value_new_number:
.ifdef KN_DEBUG
.if 0
	mov %rdi, %rax
	shl %rax
	shr %rax
	assert_eq %rax, %rdi /* ensure we are not massive. */
.endif
.endif /* KN_DEBUG */
	shl $2, %rdi
	lea 2(%rdi), %rax
	ret

/* Create a new boolean. 
 *
 * If rdi is zero, `False` is returned. Otherwise, `True` is returned.
 */
.globl kn_value_new_boolean
kn_value_new_boolean:
	cmp $0, %rdi
	je kn_value_new_false /* If the value given is zero, return 'false'. */
	/* otherwise, fall through and return 'true'. */

/* Create a true value. */
.globl kn_value_new_true
kn_value_new_true:
	mov $TRUE_BITS, %eax
	ret

/* Create a false value. */
.globl kn_value_new_false
kn_value_new_false:
	xor %eax, %eax
	assert_eq $FALSE_BITS, %rax
	ret

/* Create a null value. */
.globl kn_value_new_null
kn_value_new_null:
	mov $NULL_BITS, %eax
	ret

.macro ensure_lower_bits_not_set src=%rdi, clobber=%rcx
.ifdef KN_DEBUG
	mov \src, \clobber
	and $TAG_MASK, \clobber
	assert_z \clobber
.endif /* KN_DEBUG */
.endm /* ensure_lower_bits_not_set */

/* Creates a new string value from the given string.
 *
 * This doesn't accept c-style strings; it only accepts strings created from
 * functions (or the interned strings) within `functions.s`.
 */
.globl kn_value_new_string
kn_value_new_string:
	ensure_lower_bits_not_set
	lea STRING_TAG(%rdi), %rax
	ret

/* Creates a new identifier.
 *
 * This function accepts a single c-style string, which it will
 * not modify, but will retain ownership of.
 */
.globl kn_value_new_identifier
kn_value_new_identifier:
	ensure_lower_bits_not_set
	lea IDENT_TAG(%rdi), %rax
	ret

/* Creates a new function
 *
 * NOTE: This function relies on the fact that the `r12` register
 * is where the current stream is held.
 */
.globl kn_value_new_function
kn_value_new_function:
	ensure_lower_bits_not_set
	push %rbx
	push %r13
	push %r14

	mov %rdi, %rbx       // Save the function pointer
	movzb -1(%rdi), %r13 // load the arity

	lea 8(, %rdi, 8), %rdi // fetch the amount of memory to allocate
	call xmalloc           // allocate the memory for the struct

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

/* Frees the memory associated with a value.
 *
 * After calling this function, the value should not be used again.
 */
.globl kn_value_free
kn_value_free:
	mov %rdi, %rax
	test $ALLOC_BIT, %rax /* optimize for the case where it's a literal */
	jnz 0f
	ret                   /* No need to free literal values. */
0: /* string or ident or function */
	and $~TAG_MASK, %rdi  /* remove the tag */
	test $STRING_BIT, %rax
	jnz kn_string_free    /* if we are a string, free it */
	test $FUNC_BIT, %rax
	jz _free              /* if we are an ident, use `_free` */
/* At this point, we are a function */
	assert_test $FUNC_BIT, %rax
	push %rbx
	push %r12
	sub $8, %rsp /* we only need two registers, but we need alignment */

	mov %rdi, %rbx /* Store the base address. */

	mov (%rdi), %rax
	movzb -1(%rax), %r12
	lea (%rdi, %r12, 8), %r12 /* Load the last index into r12 */
0: /* Because order does not matter when freeing, we can count backwards */
	cmp %rbx, %r12
	je 1f                 /* If our ending argument is equal to the starting one, we are done. */
	mov (%r12), %rdi
	call kn_value_free    /* Free the current argument */
	sub $8, %r12          /* go one argument back */
	jmp 0b
1:
	mov %rbx, %rdi /* prepare to free the function struct itself */
	add $8, %rsp   /* restore previous values */
	pop %r12
	pop %rbx
	jmp _free      /* free the entire struct, as it was malloced. */

/* Runs a value
 *
 * This will not free the value that is passed, and the return value must
 * be freed independently of the passed value.
 */
.globl kn_value_run
kn_value_run:
	mov %rdi, %rax        /* both prep for immediate return and the start of allocated runs */
	test $ALLOC_BIT, %rdi /* check to see if we are an allocated type */
	jnz 0f                /* if so, continue onwards */
	ret                   /* otherwise, just return the immediate value. */
0: /*string, ident, function */
	and $~TAG_MASK, %rdi  /* remove the tag from the pointer */
	test $FUNC_BIT, %rax
	jnz 0f                /* if we are a function, jump to the end */
	test $STRING_BIT, %rax
	cmovz (%rdi), %rdi    /* if we are an identifier, fetch it... */
	jz kn_env_get         /* ...and then run it */
/* here we should be running a string */
	assert_test $STRING_BIT, %rax
	sub $8, %rsp
	call kn_string_clone    /* running a string duplicates it */
	add $8, %rsp
	mov %rax, %rdi
	jmp kn_value_new_string /* create a new string with the return value. */
0: /* run a function */
	assert_test $FUNC_BIT, %rax /* sanity check */
	mov (%rdi), %rax          /* load the function pointer */
	lea 8(%rdi), %rdi         /* load the arguments start */
	jmp *%rax                 /* run the function */

/* Converts a value to a string
 *
 * The returned string is completely distinct from the passed value---each
 * must be individually freed.
 */
.globl kn_value_to_string
kn_value_to_string:
	test $ALLOC_BIT, %rdi   /* check to see if we are an allocated type */
	jnz 1f                  /* if so, continue onwards */
/* Here we are a literal */
	test $NUM_BIT, %rdi     /* restore the tag bit */
	jnz integer_to_string   /* if we are a number, go to that section */
/* Either true, false, or null */
	lea kn_string_true(%rip), %rax
	cmp $TRUE_BITS, %rdi
	je 0f                           /* if we are `true`, go to return. */
	lea kn_string_false(%rip), %rax
	cmp $FALSE_BITS, %rdi           /* if we are `false`, go to return. */
	je 0f
	assert_eq $NULL_BITS, %rdi      /* at this point, we should be null. */
	lea kn_string_null(%rip), %rax
0:
	ret
1: /* here, we are allocated */
	test $STRING_BIT, %rdi
	jz 0f
	and $~TAG_MASK, %rdi
	jmp kn_string_clone     /* if we are a string, simply clone it */
0: /* otherwise, we execute the value, then convert that to a string. */
	sub $8, %rsp
	call kn_value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp kn_value_to_string /* after running, try to parse the string again */
integer_to_string:
	sar $2, %rdi
	sub $40, %rsp  /* allocate space for the string on the stack; it will be duped later */
	mov %rsp, %rsi /* get a pointer to the base */

	/* short circuit for if we are zero */
	cmp $0, %rdi
	jne 0f
	lea kn_string_zero(%rip), %rax
	add $40, %rsp
	ret
0: /* here we're a nonzero value */
	/* add a leading `-` if necessary */
	cmp $0, %rdi
	jge 0f            /* if positive, skip this */
	movb $'-', (%rsi) /* add the `-` */
	inc %rsi          /* increase the pointer */
	neg %rdi          /* remove the negative value */
0: /* nonzero, positive integer; note i took this from gcc's output */
	movabsq	$7378697629483820647, %r8
1:
	movq	%rdi, %rax
	imulq	%r8
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	addq	%rax, %rdx
	leal	(%rdx,%rdx), %eax
	leal	(%rax,%rax,4), %eax
	movl	%edi, %ecx
	subl	%eax, %ecx
	movb	%cl, (%rsi)
	incq	%rsi
	addq	$9, %rdi
	cmpq	$18, %rdi
	movq	%rdx, %rdi
	ja	1b
/* done parsing the string */
	sub %rsp, %rsi
	mov %rsp, %rdi
	call _strndup
	mov %rax, %rdi
	add $40, %rsp
	jmp kn_string_new

/* Converts a value to a number */
.globl kn_value_to_number
kn_value_to_number:
	test $NUM_BIT, %rdi     /* see if we are a number */
	jz 0f                   /* if we aren't, go forward */
/* At this point, we have a number; optimize for number -> number conversion. */
	sar $2, %rdi            /* remove number tag */
	mov %rdi, %rax
	ret
0: /* anything but a number */
	test $ALLOC_BIT, %rdi   /* check to see if we are an allocated type */
	jnz 0f                  /* if so, continue onwards */
/* at this point, we have a literal */
	xor %eax, %eax
	cmp $TRUE_BITS, %rdi
	sete %al                /* if it is true, we have a value of one. otherwise, zero. */
	ret
0: /* string, ident, or function */
	test $STRING_BIT, %rdi
	jz 0f
	and $~TAG_MASK, %rdi
	mov (%rdi), %rdi
	xor %esi, %esi
	mov $10, %edx
	jmp _strtoll
0: /* otherwise, execute the value, and then convert the result to a number */
	sub $8, %rsp
	call kn_value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp kn_value_to_number/* after running, try to parse the number again */

/*** TODO: this should return 0 or nonzero, not true/false. ***/
.globl kn_value_to_boolean
kn_value_to_boolean:
	test $ALLOC_BIT, %rdi
	jnz 0f
	xor %eax, %eax
	cmp $4, %rdi /* false=0, zero = 2, null = 4; 1 and 3 are allocated. */
	setg %al
	ret
0:
	test $STRING_BIT, %rdi
	jz 0f
	and $~TAG_MASK, %rdi /* remove the string tag */
	mov (%rdi), %rax      /* deref string struct */
	movzb (%rax), %eax    /* deref first byte--0 means empty */
	ret
0: /* otherwise, execute the value, and then convert the result to a boolean */
	sub $8, %rsp
	call kn_value_run
	mov %rax, %rdi
	add $8, %rsp
	jmp kn_value_to_boolean /* after running, try to parse the boolean again */

/* Clones the given value. */
.globl kn_value_clone
kn_value_clone:
	mov %rdi, %rax
	test $ALLOC_BIT, %rdi
	jnz 0f
	ret /* if it is not allocated, simply return the given value */
0: /* string, ident, or function */
	push %rbx /* for string and ident, we just need alignment. for func, we need rbx. */
	and $~TAG_MASK, %rdi    /* remove the tag */
	test $STRING_BIT, %rax
	jz 0f
	call kn_string_clone /* duplicate the string */
	mov %rax, %rdi
	pop %rbx
	jmp kn_value_new_string /* then create a new string */
0: /* ident or function */
	test $FUNC_BIT, %rax
	jnz 0f
	call _strdup /* duplicate the identifier */
	pop %rbx
	mov %rax, %rdi
	jmp kn_value_new_identifier
0: /* function */
	push %r12
	push %r13

	assert_test $FUNC_BIT, %rax
 	mov %rdi, %rbx /* Store the base address. */
 
 	mov (%rdi), %rax
 	movzb -1(%rax), %r12
 	lea 8(,%r12, 8), %r12 /* Load the length */
 	mov %r12, %rdi
 	call ddebug
# 0: /* Because order does not matter when freeing, we can count backwards */
# 	cmp %rbx, %r12
# 	je 1f                 /* If our ending argument is equal to the starting one, we are done. */
# 	mov (%r12), %rdi
# 	call kn_value_free    /* Free the current argument */
# 	sub $8, %r12          /* go one argument back */
# 	jmp 0b
# 1:
# 	mov %rbx, %rdi /* prepare to free the function struct itself */
# 	add $8, %rsp   /* restore previous values */
# 	pop %r12
# 	pop %rbx
# 	jmp _free      /* free the entire struct, as it was malloced. */
# 

	pop %r13
	pop %r12
	pop %rbx
# 	mov %rdi, %rax
# 	movb -1(%rax), %rax /* load the arity */
# 	lea (%rdi, %r12, 8), %r12 /* Load the last index into r12 */
# 	call ddebug
# 	pop %r13
# 	pop %r12
# 	todo "clone a function"
# 	add $8, %rsp

.globl kn_value_dump
kn_value_dump:
	mov %rdi, %rsi
	test $NUM_BIT, %rsi
	jz 0f
	sar $2, %rsi
	lea num_fmt(%rip), %rdi
	jmp _printf
0: // true, false, null, string, ident, function
	mov %rdi, %rsi
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
	mov (%rsi), %rsi
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
	mov (%rsi), %rsi
	lea kn_func_stop(%rip), %rdi
	jmp _printf
0: // unknown
	lea invalid_fmt(%rip), %rdi
	call abort

.data:
invalid_fmt: .asciz "unknown value type: '%d'\n"
kn_func_start: .asciz "Func(%p):\n"
kn_func_stop: .asciz "Func(%p)/\n"
num_fmt: .asciz "Number(%lli)\n"
string_fmt: .asciz "String(%s)\n"
ident_fmt: .asciz "Ident(%s)\n"
true_fmt: .asciz "True\n"
false_fmt: .asciz "False\n"
null_fmt: .asciz "Null\n"
