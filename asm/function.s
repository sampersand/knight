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
	jmp kn_value_new_integer

define_function 1, block
	todo "kn_func_block"

define_function 1, eval
	todo "kn_func_eval"

define_function 1, call
	todo "kn_func_call"

define_function 1, system
	todo "kn_func_system"

define_function 1, quit
	todo "kn_func_quit"

define_function 1, not
	sub $8, %rsp
	mov (%rdi), %rdi          // fetch the first argument
	call kn_value_to_boolean  // convert it to a boolean
	cmp $0, %rax              // check to see if the value is false
	je kn_value_new_true      // if they are, then return true
	jmp kn_value_new_false    // otherwise, return false.

define_function 1, length
	todo "kn_func_length"
	/*
	mov (%rdi), %rdi
	call value_run
	mov %rax, %rdi
	call value_to_integer
	mov %rax, %rdi
	jmp kn_value_new_integer */


define_function 1, output
	todo "kn_func_output"


define_function 2, add
	todo "kn_func_add"

define_function 2, sub
	todo "kn_func_sub"

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
