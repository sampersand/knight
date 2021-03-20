.macro peek where:req
	movzb (%r12), \where
.endm

.macro advance
	inc %r12
.endm

.macro unadvance
	dec %r12
.endm

.globl kn_parse
kn_parse:
	push %r12
	mov %rdi, %r12
handle_stream:
	peek %eax
	advance
	lea (,%rax,8), %rcx
	add parse_table(%rip), %rcx
	jmp *(%rcx)

done_parsing:
	mov %r12, %rdi /* this is used by kn_value_new_function */
	pop %r12
	ret

/* A token was expected, but could not be found. */
expected_token:
	lea expected_token_fmt(%rip), %rdi
	jmp abort

.equ whitespace, handle_stream

/* parse a comment out */
comment:
	peek %ecx
	advance
	cmp $'\n', %ecx      /* check to see if we're at end of line */
	je handle_stream     /* if we are, go find the next token */
	jecxz expected_token /* if we are at eof, we expected a token */
	jmp comment          /* otherwise, get the next character */

integer:
	lea -'0'(%rax), %rdi
	lea done_parsing(%rip), %rax
	push %rax
0:
	peek %eax
	sub $'0', %eax
	cmp $9, %rax
	jg kn_value_new_number
	advance
	imul $10, %rdi
	add %rax, %rdi
	jmp 0b

identifier:
	unadvance
	mov %r12, %rdi
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

	mov %r12, %rsi
	sub %rdi, %rsi
	dec %rsi
	call _strndup
	mov %rax, %rdi
	lea done_parsing(%rip), %rax
	push %rax
	jmp kn_value_new_identifier

string:
	mov %r12, %rdi // store the quote start.
0:
	peek %ecx
	advance
	jecxz 1f
	cmp %al, %cl
	jne 0b

	mov %r12, %rsi
	sub %rdi, %rsi
	dec %rsi
	call _strndup
	mov %rax, %rdi
	call kn_string_new
	mov %rax, %rdi
	lea done_parsing(%rip), %rax
	push %rax
	jmp kn_value_new_string
1: // An unterminated quote was encountered.
	dec %rdi
	mov %rdi, %rsi
	lea unterminated_quote_msg(%rip), %rdi
	jmp abort

invalid:
	lea invalid_token_fmt(%rip), %rdi
	mov %rax, %rsi
	jmp abort

literal_true:
	lea done_parsing(%rip), %rax
	push %rax
0:
	peek %eax
	advance
	sub $'A', %al
	cmp $('Z' - 'A'), %rax
	jle 0b
	unadvance
	jmp kn_value_new_true

literal_false:
	lea done_parsing(%rip), %rax
	push %rax
0:
	peek %eax
	advance
	sub $'A', %al
	cmp $('Z' - 'A'), %rax
	jle 0b
	unadvance
	jmp kn_value_new_false

literal_null:
	lea done_parsing(%rip), %rax
	push %rax
0:
	peek %eax
	advance
	sub $'A', %al
	cmp $('Z' - 'A'), %rax
	jle 0b
	unadvance
	jmp kn_value_new_null


.macro decl_sym_function label:req
function_\label:
	lea kn_func_\label(%rip), %rdi
	jmp function
.endm

.macro decl_kw_function label:req
function_\label:
	lea kn_func_\label(%rip), %rdi
	jmp keyword_function
.endm

decl_sym_function not
decl_sym_function mod
decl_sym_function and
decl_sym_function mul
decl_sym_function add
decl_sym_function sub
decl_sym_function div
decl_sym_function then
decl_sym_function lth
decl_sym_function assign
decl_sym_function gth
decl_sym_function eql
decl_sym_function pow
decl_sym_function system
decl_sym_function or

decl_kw_function block
decl_kw_function debug
decl_kw_function call
decl_kw_function eval
decl_kw_function get
decl_kw_function length
decl_kw_function output
decl_kw_function prompt
decl_kw_function quit
decl_kw_function random
decl_kw_function set
decl_kw_function while

function_if: // optimization because if is used so often.
	lea kn_func_if(%rip), %rdi
keyword_function:
	peek %eax
	advance
	sub $'A', %al
	cmp $('Z' - 'A'), %rax
	jle keyword_function
	unadvance
function:
	lea done_parsing(%rip), %rax
	push %rax
	jmp kn_value_new_function

.data

expected_token_fmt:
	.asciz "expected a token.\n"
invalid_token_fmt:
	.asciz "unknown token character '%1$c' (0x%1$x).\n"
unterminated_quote_msg:
	.asciz "unterminated quote encountered: %s\n"
parse_table:
	.quad 	parse_table+8
	.quad 	expected_token   /* \x00 */
	.quad 	invalid          /* \x01 */
	.quad 	invalid          /* \x02 */
	.quad 	invalid          /* \x03 */
	.quad 	invalid          /* \x04 */
	.quad 	invalid          /* \x05 */
	.quad 	invalid          /* \x06 */
	.quad 	invalid          /* \a   */
	.quad 	invalid          /* \b   */
	.quad 	whitespace       /* \t   */
	.quad 	whitespace       /* \n   */
	.quad 	whitespace       /* \v   */
	.quad 	whitespace       /* \f   */
	.quad 	whitespace       /* \r   */
	.quad 	invalid          /* \x0E */
	.quad 	invalid          /* \x0F */
	.quad 	invalid          /* \x10 */
	.quad 	invalid          /* \x11 */
	.quad 	invalid          /* \x12 */
	.quad 	invalid          /* \x13 */
	.quad 	invalid          /* \x14 */
	.quad 	invalid          /* \x15 */
	.quad 	invalid          /* \x16 */
	.quad 	invalid          /* \x17 */
	.quad 	invalid          /* \x18 */
	.quad 	invalid          /* \x19 */
	.quad 	invalid          /* \x1A */
	.quad 	invalid          /* \e   */
	.quad 	invalid          /* \x1C */
	.quad 	invalid          /* \x1D */
	.quad 	invalid          /* \x1E */
	.quad 	invalid          /* \x1F */
	.quad 	whitespace       /* <space> */
	.quad 	function_not     /* !    */
	.quad 	string           /* "    */
	.quad 	comment          /* #    */
	.quad 	invalid          /* $    */
	.quad 	function_mod     /* %    */
	.quad 	function_and     /* &    */
	.quad 	string           /* '    */
	.quad 	whitespace       /* (    */
	.quad 	whitespace       /* )    */
	.quad 	function_mul     /* *    */
	.quad 	function_add     /* +    */
	.quad 	invalid          /* ,    */
	.quad 	function_sub     /* -    */
	.quad 	invalid          /* .    */
	.quad 	function_div     /* /    */
	.quad 	integer          /* 0    */
	.quad 	integer          /* 1    */
	.quad 	integer          /* 2    */
	.quad 	integer          /* 3    */
	.quad 	integer          /* 4    */
	.quad 	integer          /* 5    */
	.quad 	integer          /* 6    */
	.quad 	integer          /* 7    */
	.quad 	integer          /* 8    */
	.quad 	integer          /* 9    */
	.quad 	whitespace       /* :    */
	.quad 	function_then    /* ;    */
	.quad 	function_lth     /* <    */
	.quad 	function_assign  /* =    */
	.quad 	function_gth     /* >    */
	.quad 	function_eql     /* ?    */
	.quad 	invalid          /* @    */
	.quad 	invalid          /* A    */
	.quad 	function_block   /* B    */
	.quad 	function_call    /* C    */
	.quad 	function_debug   /* D    */
	.quad 	function_eval    /* E    */
	.quad 	literal_false    /* F    */
	.quad 	function_get     /* G    */
	.quad 	invalid          /* H    */
	.quad 	function_if      /* I    */
	.quad 	invalid          /* J    */
	.quad 	invalid          /* K    */
	.quad 	function_length  /* L    */
	.quad 	invalid          /* M    */
	.quad 	literal_null     /* N    */
	.quad 	function_output  /* O    */
	.quad 	function_prompt  /* P    */
	.quad 	function_quit    /* Q    */
	.quad 	function_random  /* R    */
	.quad 	function_set     /* S    */
	.quad 	literal_true     /* T    */
	.quad 	invalid          /* U    */
	.quad 	invalid          /* V    */
	.quad 	function_while   /* W    */
	.quad 	invalid          /* X    */
	.quad 	invalid          /* Y    */
	.quad 	invalid          /* Z    */
	.quad 	whitespace       /* [    */
	.quad 	invalid          /* \    */
	.quad 	whitespace       /* ]    */
	.quad 	function_pow     /* ^    */
	.quad 	identifier       /* _    */
	.quad 	function_system  /* `    */
	.quad 	identifier       /* a    */
	.quad 	identifier       /* b    */
	.quad 	identifier       /* c    */
	.quad 	identifier       /* d    */
	.quad 	identifier       /* e    */
	.quad 	identifier       /* f    */
	.quad 	identifier       /* g    */
	.quad 	identifier       /* h    */
	.quad 	identifier       /* i    */
	.quad 	identifier       /* j    */
	.quad 	identifier       /* k    */
	.quad 	identifier       /* l    */
	.quad 	identifier       /* m    */
	.quad 	identifier       /* n    */
	.quad 	identifier       /* o    */
	.quad 	identifier       /* p    */
	.quad 	identifier       /* q    */
	.quad 	identifier       /* r    */
	.quad 	identifier       /* s    */
	.quad 	identifier       /* t    */
	.quad 	identifier       /* u    */
	.quad 	identifier       /* v    */
	.quad 	identifier       /* w    */
	.quad 	identifier       /* x    */
	.quad 	identifier       /* y    */
	.quad 	identifier       /* z    */
	.quad 	whitespace       /* {    */
	.quad 	function_or      /* |    */
	.quad 	whitespace       /* }    */
	.quad 	invalid          /* ~    */
	.quad   invalid          /* 0x7f */
