.include "debug.s"
.include "value_header.s"

.macro define_function argc:req suffix:req
.balign 16
.space 15
.byte \argc
.global kn_func_\suffix
kn_func_\suffix:
.endm // define_function

.macro is_string reg:req
	and $STRING_TAG, \reg
	sub $STRING_TAG, \reg
.endm /* is_string */
.macro as_string reg:req
	and $~TAG_MASK, \reg
.endm /* is_string */

define_function 0, prompt
	todo "kn_func_prompt"

define_function 0, random
	sub $8, %rsp
	call _rand
	add $8, %rsp
	mov %rax, %rdi
	jmp kn_value_new_number

define_function 1, block
	mov (%rdi), %rdi
	jmp kn_value_clone // simply clone the value given

define_function 1, call
	push %rbx
	mov (%rdi), %rdi
	call kn_value_run  // execute the passed parameter
	mov %rax, %rbx     // store the value so we can free it later.
	mov %rax, %rdi     // prepare for calling the returned value
	call kn_value_run  // execute the returend value
	mov %rbx, %rdi     // prepare for freeing the first value
	mov %rax, %rbx     // store the value to return
	call kn_value_free // free the first value
	mov %rbx, %rax     // restore the return value
	pop %rbx
	ret

define_function 1, eval
	push %rbx
	mov (%rdi), %rdi
	call kn_value_to_string // convert the passed parameter into a string
	mov %rax, %rbx          // store the string to free it later
	mov (%rax), %rdi        // prepare for calling kn_run
	call kn_run             // execute the string and get the result
	mov %rbx, %rdi          // prepare for calling kn_string_free
	mov %rax, %rbx          // store the result of kn_run
	call kn_string_free     // free the string that was evaluated
	mov %rbx, %rax          // restore the return value
	pop %rbx
	ret

define_function 1, debug
	push %rbx
	mov (%rdi), %rdi
	mov %rdi, %rbx
	call kn_value_dump
	mov %rbx, %rdi
	call kn_value_run
	mov %rax, %rbx
	mov %rax, %rdi
	call kn_value_dump
	mov %rbx, %rax
	pop %rbx
	ret

define_function 1, system
	todo "kn_func_system"

define_function 1, quit
	sub $8, %rsp
	mov (%rdi), %rdi
	call kn_value_to_number
	mov %rax, %rdi
	call _exit

define_function 1, not
	sub $8, %rsp
	mov (%rdi), %rdi          // fetch the first argument
	call kn_value_to_boolean  // convert it to a boolean
	add $8, %rsp
	cmp $0, %rax              // check to see if the value is false
	je kn_value_new_true      // if they are, then return true
	jmp kn_value_new_false    // otherwise, return false.

define_function 1, length
	push %rbx
	mov (%rdi), %rdi        // deref the first argument
	call kn_value_to_string // convert it to a string
	mov %rax, %rbx          // save the returned string, so we can free later
	mov (%rax), %rdi        // deref the string to get its char * ptr
	call _strlen            // fetch the strings length
	mov %rbx, %rdi          // prepare for freeing the string
	mov %rax, %rbx          // save the length
	call kn_string_free     // free the string
	mov %rbx, %rdi          // restore the return value
	pop %rbx
	jmp kn_value_new_number


define_function 1, output
	todo "kn_func_output"

define_function 2, add
	push %rbx
	mov 8(%rdi), %rbx  /* save arg2 */
	mov (%rdi), %rdi   /* fetch arg1 */
	call kn_value_run  /* run arg1 */
	mov %rax, %rdi     /* save arg1's value */
	is_string %eax     /* check if arg1 is a string */
	jz .string_concat  /* if it is a string, go to concat */
/* integer addition */
	call kn_value_to_number /* convert arg1 to a number */
	mov %rbx, %rdi          /* prepare to convert arg2 to a number */
	mov %rax, %rbx          /* save arg1's number */
	call kn_value_to_number /* convert arg2 to a number */
	add %rbx, %rax          /* add arg1's number to arg2's number */
	mov %rax, %rdi          /* prepare to make a new number */
	pop %rbx
	jmp kn_value_new_number /* create a new number with the addition result */
.string_concat: /* string concatenation */
//	mov %rdi, %rbx          /* save the first string */
//	as_string %rbx
//	mov 8(%rdi), %rdi       /* load arg2 */
//	call kn_value_to_string /* convert arg2 to a string */
//	push %r12
//	sub $8, %rsp
//
//	add $8, %rsp
//	pop %r12
	todo "string_concat"

define_function 2, sub
	push %rbx
	mov 8(%rdi), %rbx       /* Save the second arg for the future. */
	mov (%rdi), %rdi
	call kn_value_to_number /* Convert the first arg to a number. */
	mov %rbx, %rdi          /* Load the second arg. */
	mov %rax, %rbx
	call kn_value_to_number /* Convert the second arg to a number. */
	sub %rax, %rbx
	mov %rbx, %rdi
	pop %rbx
	jmp kn_value_new_number

define_function 2, mul
	push %rbx
	mov 8(%rdi), %rbx  /* save arg2 */
	mov (%rdi), %rdi   /* fetch arg1 */
	call kn_value_run  /* run arg1 */
	mov %rax, %rdi     /* save arg1's value */
	is_string %eax     /* check if arg1 is a string */
	jz .string_dupl    /* if it is a string, duplicate it */
/* integer addition */
	call kn_value_to_number /* convert arg1 to a number */
	mov %rbx, %rdi          /* prepare to convert arg2 to a number */
	mov %rax, %rbx          /* save arg1's number */
	call kn_value_to_number /* convert arg2 to a number */
	imul %rbx, %rax         /* multiply arg2 by arg1 */
	mov %rax, %rdi          /* prepare to make a new number */
	pop %rbx
	jmp kn_value_new_number /* create a new number with the multiplication result */
.string_dupl: /* string multiplication */
//	mov %rdi, %rbx          /* save the first string */
//	as_string %rbx
//	mov 8(%rdi), %rdi       /* load arg2 */
//	call kn_value_to_string /* convert arg2 to a string */
//	push %r12
//	sub $8, %rsp
//
//	add $8, %rsp
//	pop %r12
	todo "string_dupl"


define_function 2, div
	push %rbx
	mov 8(%rdi), %rbx       /* Save arg2 for the future. */
	mov (%rdi), %rdi
	call kn_value_to_number /* Convert the arg1 to a number. */
	mov %rbx, %rdi          /* Load arg2. */
	mov %rax, %rbx
	call kn_value_to_number /* Convert arg2 to a number. */
	cmp $0, %rax
	jz 0f
/* non-zero divisor division */
	mov %rax, %rcx /* store the denom */
	mov %rbx, %rax /* load the numer */
	cqto             /* no idea what this does, but gcc does it */
	idivq %rcx       /* divide by the denom */
	mov %rax, %rdi /* prepare to make a new number */
	pop %rbx
	jmp kn_value_new_number /* make a new number */
0: /* division by zero */
	lea divide_by_zero(%rip), %rdi
	jmp abort
.pushsection .data, ""
divide_by_zero:
	.asciz "Fatal error: Cannot divide by zero!\n"
.popsection

define_function 2, mod
	push %rbx
	mov 8(%rdi), %rbx       /* Save arg2 for the future. */
	mov (%rdi), %rdi
	call kn_value_to_number /* Convert the arg1 to a number. */
	mov %rbx, %rdi          /* Load arg2. */
	mov %rax, %rbx
	call kn_value_to_number /* Convert arg2 to a number. */
	cmp $0, %rax
	jz 0f
/* non-zero base modulus */
	todo "mod"
0: /* modulo by zero */
	lea modulo_by_zero(%rip), %rdi
	jmp abort
.pushsection .data, ""
modulo_by_zero:
	.asciz "Fatal error: Cannot modulo by zero!\n"
.popsection

define_function 2, pow
	push %rbx
	mov 8(%rdi), %rbx       /* Save the second arg for the future. */
	mov (%rdi), %rdi
	call kn_value_to_number /* Convert the base to a number. */
	mov %rbx, %rdi          /* Load the second arg. */
	mov %rax, %rbx          /* save the first arg */
	call kn_value_to_number /* Convert the exponent to a number. */
	mov %rax, %rcx          /* exponent = rcx */
	mov $1, %rdi            /* result = rdi = 1 */
	mov %rbx, %rax          /* base = rax */
	pop %rbx
0:
	cmp $0, %rcx
	je  kn_value_new_number /* if rcx is zero, we are at end */
	imul %rax, %rdi         /* result *= base */
	dec %rcx                /* exponent -- */
	jmp 0b

/* Check to see if two values are equal */
define_function 2, eql
	push %rbx
/* run the values */
	mov 8(%rdi), %rbx     /* save second argument for later */
	mov (%rdi), %rdi
	call kn_value_run     /* run the first argument */
	mov %rbx, %rdi
	mov %rax, %rbx        /* save the first argument */
	call kn_value_run     /* run the second argument */
/* compare tags to see if equal */
	mov %rbx, %r8
	mov %rax, %r9
	and $TAG_MASK, %r8b    /* get the tags */
	and $TAG_MASK, %r9b
	cmp %r8b, %r9b
	jne 1f
	test $ALLOC_BIT, %eax
	jnz 0f
/* we are literal values */
	mov %rbx, %rcx
	pop %rbx
	cmp %rcx, %rax
	je kn_value_new_true
	jmp kn_value_new_false
0: /* we are allocated */
	test $STRING_BIT, %eax
	jz 1f                   /* non-string values are _always_ not equal */
	and $~TAG_MASK, %rax    /* remove tags */
	and $~TAG_MASK, %rbx
	push %r12
	sub $8, %rsp
/* call strcmp */
	mov %rax, %r12
	mov (%rax), %rdi
	mov (%rbx), %rsi
	call _strcmp
/* free the strings we were given */
	mov %r12, %rdi
	mov %rax, %r12
	call kn_string_free
	mov %rbx, %rdi
	call kn_string_free
/* restore previous state and call kn_value_new_boolean */
	xor %edi, %edi
	cmp $0, %r12
	sete %dil /* set edi to whether r12 was zero */
	add $8, %rsp
	pop %r12
	pop %rbx
	jmp kn_value_new_boolean
1: /* the values weren't the same */
	mov %rax, %rdi
	call kn_value_free      /* free the arguments we were given */
	mov %rbx, %rdi
	call kn_value_free
	pop %rbx
	jmp kn_value_new_false

define_function 2, lth
	todo "kn_func_lth"

define_function 2, gth
	todo "kn_func_gth"

define_function 2, then
	push %rbx
	mov 8(%rdi), %rbx
	mov (%rdi), %rdi
	call kn_value_run
	mov %rax, %rdi
	call kn_value_free
	mov %rbx, %rdi
	pop %rbx
	jmp kn_value_run

define_function 2, assign
	push %rbx
	mov (%rdi), %eax
	and $TAG_MASK, %eax
	cmp $IDENT_TAG, %eax
	jne 0f
/* we are an identifier */
	mov (%rdi), %rbx
	mov 8(%rdi), %rdi
	call kn_value_run
	mov %rax, %rsi
	mov %rbx, %rdi
	and $~TAG_MASK, %rdi
	pop %rbx
	jmp kn_env_set
0:
	todo "kn_func_assign for non-identifiers"

define_function 2, while
	todo "kn_func_while"

define_function 2, and
	todo "kn_func_and"

define_function 2, or
	todo "kn_func_or"


define_function 3, if
	todo "kn_func_if"

define_function 3, get
	todo "kn_func_get"


define_function 4, set
	todo "kn_func_set"
