.macro peek where:req
	movzbl (%r12), \where
.endm

.macro advance
	inc %r12
.endm

.globl ast_parse
ast_parse:
	push %r12
	push %r13
	push %r14
	mov %rdi, %r12
handle_stream:
	peek %eax
	advance
	mov %rax, %rcx
	imul $8, %rcx
	add parse_table(%rip), %rcx
	push (%rcx)
	ret
done_parsing:
	mov %r12, %rdi
	pop %r14
	pop %r13
	pop %r12
	ret

expected_token:
	lea expected_token_fmt(%rip), %rdi
	call _printf
	jmp die

.equ whitespace, handle_stream

comment:
	peek %eax
	advance
	cmp $'\n', %rax
	je handle_stream
	cmp $'\0', %rax
	je done_parsing
	jmp comment

integer:
	xor %rdi, %rdi
	sub $'0', %rax
	jmp 1f
0:
	advance
1:
	imul $10, %rdi
	add %rax, %rdi
	peek %eax
	sub $'0', %al
	cmp $9, %rax
	jle 0b
	call value_new_integer
	jmp done_parsing


identifier:
	mov %r12, %r13
0:
	peek %eax
	advance
	sub $'0', %al
	cmp $9, %rax
	jle 0b
	cmp $('_' - '0'), %al
	jz 0b
	sub $('a' - '0'), %al
	cmp $('z' - 'a'), %rax
	jle 0b

	mov %r12, %r14
	sub %r13, %r14
	mov %r14, %rdi
	inc %rdi # b/c of trailing `\0`.
	call _malloc
	mov %rax, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	dec %rsi # b/c it advances it in the `handle_stream` function.
	call _strncat
	mov %rax, %rdi
	call value_new_identifier
	jmp done_parsing

string:
	mov %r12, %r13 # store the quote start.
0:
	peek %ecx
	advance
	cmp $0, %ecx
	je unterminated_quote
	cmp %eax, %ecx
	jne 0b

	mov %r12, %r14
	sub %r13, %r14
	mov %r14, %rdi
	call _malloc
	mov %rax, %rdi
	mov %r13, %rsi
	dec %r14
	mov %r14, %rdx
	call _strncat
	mov %rax, %rdi
	call string_new
	mov %rax, %rdi
	call value_new_string
	jmp done_parsing

unterminated_quote:
	dec %r13
	lea unterminated_quote_msg(%rip), %rdi
	mov %r13, %rsi
	call _printf
	jmp die

invalid:
	lea invalid_token_fmt(%rip), %rdi
	mov %rax, %rsi
	call _printf
	jmp die

.macro decl_sym_function label:req, fn:req
\label:
	lea \fn(%rip), %rdi
	jmp function
.endm

.macro decl_kw_function label:req, fn:req
\label:
	lea \fn(%rip), %rdi
	jmp keyword_function
.endm

decl_sym_function function_not, func_not
decl_sym_function function_mod, func_mod
decl_sym_function function_and, func_and
decl_sym_function function_mul, func_mul
decl_sym_function function_add, func_add
decl_sym_function function_sub, func_sub
decl_sym_function function_div, func_div
decl_sym_function function_then, func_then
decl_sym_function function_lth, func_lth
decl_sym_function function_assign, func_assign
decl_sym_function function_gth, func_gth
decl_sym_function function_eql, func_eql
decl_sym_function function_pow, func_pow
decl_sym_function function_system, func_system
decl_sym_function function_or, func_or

decl_kw_function function_block, func_block
decl_kw_function function_call, func_call
decl_kw_function function_eval, func_eval
decl_kw_function function_false, func_false
decl_kw_function function_get, func_get
decl_kw_function function_length, func_length
decl_kw_function function_null, func_null
decl_kw_function function_output, func_output
decl_kw_function function_prompt, func_prompt
decl_kw_function function_quit, func_quit
decl_kw_function function_random, func_random
decl_kw_function function_set, func_set
decl_kw_function function_true, func_true
decl_kw_function function_while, func_while

function_if:
	lea func_if(%rip), %rdi
keyword_function:
	peek %eax
	advance
	sub $'A', %al
	cmp $('Z' - 'A'), %rax
	jle keyword_function
function:
	call value_new_function
	jmp done_parsing

.data

expected_token_fmt:
	.asciz "expected a token.\n"
invalid_token_fmt:
	.asciz "unknown token character '%1$c' (0x%1$x).\n"
unterminated_quote_msg:
	.asciz "unterminated quote encountered: %s\n"
parse_table:
	.quad 	parse_table+8
	.quad 	expected_token   # \x00
	.quad 	invalid          # \x01
	.quad 	invalid          # \x02
	.quad 	invalid          # \x03
	.quad 	invalid          # \x04
	.quad 	invalid          # \x05
	.quad 	invalid          # \x06
	.quad 	invalid          # \a
	.quad 	invalid          # \b
	.quad 	whitespace       # \t
	.quad 	whitespace       # \n
	.quad 	whitespace       # \v
	.quad 	whitespace       # \f
	.quad 	whitespace       # \r
	.quad 	invalid          # \x0E
	.quad 	invalid          # \x0F
	.quad 	invalid          # \x10
	.quad 	invalid          # \x11
	.quad 	invalid          # \x12
	.quad 	invalid          # \x13
	.quad 	invalid          # \x14
	.quad 	invalid          # \x15
	.quad 	invalid          # \x16
	.quad 	invalid          # \x17
	.quad 	invalid          # \x18
	.quad 	invalid          # \x19
	.quad 	invalid          # \x1A
	.quad 	invalid          # \e
	.quad 	invalid          # \x1C
	.quad 	invalid          # \x1D
	.quad 	invalid          # \x1E
	.quad 	invalid          # \x1F
	.quad 	whitespace       # <space>
	.quad 	function_not     # !
	.quad 	string           # "
	.quad 	comment          # #
	.quad 	invalid          # $
	.quad 	function_mod     # %
	.quad 	function_and     # &
	.quad 	string           # '
	.quad 	whitespace       # (
	.quad 	whitespace       # )
	.quad 	function_mul     # *
	.quad 	function_add     # +
	.quad 	invalid          # ,
	.quad 	function_sub     # -
	.quad 	invalid          # .
	.quad 	function_div     # /
	.quad 	integer          # 0
	.quad 	integer          # 1
	.quad 	integer          # 2
	.quad 	integer          # 3
	.quad 	integer          # 4
	.quad 	integer          # 5
	.quad 	integer          # 6
	.quad 	integer          # 7
	.quad 	integer          # 8
	.quad 	integer          # 9
	.quad 	whitespace       # :
	.quad 	function_then    # ;
	.quad 	function_lth     # <
	.quad 	function_assign  # =
	.quad 	function_gth     # >
	.quad 	function_eql     # ?
	.quad 	invalid          # @
	.quad 	invalid          # A
	.quad 	function_block   # B
	.quad 	function_call    # C
	.quad 	invalid          # D
	.quad 	function_eval    # E
	.quad 	function_false   # F
	.quad 	function_get     # G
	.quad 	invalid          # H
	.quad 	function_if      # I
	.quad 	invalid          # J
	.quad 	invalid          # K
	.quad 	function_length  # L
	.quad 	invalid          # M
	.quad 	function_null    # N
	.quad 	function_output  # O
	.quad 	function_prompt  # P
	.quad 	function_quit    # Q
	.quad 	function_random  # R
	.quad 	function_set     # S
	.quad 	function_true    # T
	.quad 	invalid          # U
	.quad 	invalid          # V
	.quad 	function_while   # W
	.quad 	invalid          # X
	.quad 	invalid          # Y
	.quad 	invalid          # Z
	.quad 	whitespace       # [
	.quad 	invalid          # <backslash>
	.quad 	whitespace       # ]
	.quad 	function_pow     # ^
	.quad 	identifier       # _
	.quad 	function_system  # `
	.quad 	identifier       # a
	.quad 	identifier       # b
	.quad 	identifier       # c
	.quad 	identifier       # d
	.quad 	identifier       # e
	.quad 	identifier       # f
	.quad 	identifier       # g
	.quad 	identifier       # h
	.quad 	identifier       # i
	.quad 	identifier       # j
	.quad 	identifier       # k
	.quad 	identifier       # l
	.quad 	identifier       # m
	.quad 	identifier       # n
	.quad 	identifier       # o
	.quad 	identifier       # p
	.quad 	identifier       # q
	.quad 	identifier       # r
	.quad 	identifier       # s
	.quad 	identifier       # t
	.quad 	identifier       # u
	.quad 	identifier       # v
	.quad 	identifier       # w
	.quad 	identifier       # x
	.quad 	identifier       # y
	.quad 	identifier       # z
	.quad 	whitespace       # {
	.quad 	function_or      # |
	.quad 	whitespace       # }
	.quad 	invalid          # ~
	.quad   invalid          # 0x7f
