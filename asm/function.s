.include "debug.s"

.macro define_function argc:req suffix:req
.balign 16
.space 15
.byte \argc
.global kn_func_\suffix
kn_func_\suffix:
.endm // define_function

define_function 0, prompt
	todo "kn_func_prompt"

define_function 0, random
	sub $8, %rsp
	call _random
	add $8, %rsp
	mov %rax, %rdi
	jmp kn_value_new_number

define_function 1, block
	jmp kn_value_clone // simply clone the value given

define_function 1, call
	push %rbx
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
	mov %rax, %rdi
	# pop %rbx
#	call kn_value_new_string

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
	todo "kn_func_add"

define_function 2, sub
	push %rbx
	lea 8(%rdi), %rbx       /* Save the second arg for the future. */
	mov (%rdi), %rdi
	call kn_value_to_number /* Convert the first arg to a number. */
	mov (%rbx), %rdi        /* Load the second arg. */
	mov %rax, %rbx
	call kn_value_to_number /* Convert the second arg to a number. */
	sub %rax, %rbx
	mov %rbx, %rdi
	pop %rbx
	jmp kn_value_new_number

define_function 2, mul
	todo "kn_func_mul"

define_function 2, div
	todo "kn_func_div"

define_function 2, mod
	todo "kn_func_mod"

define_function 2, pow
	todo "kn_func_pow"

define_function 2, eql
	todo "kn_func_eql"

define_function 2, lth
	todo "kn_func_lth"

define_function 2, gth
	todo "kn_func_gth"

define_function 2, then
	todo "kn_func_then"

define_function 2, assign
	todo "kn_func_assign"

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
