.ifndef KN_DEBUG
	.equ KN_DEBUG, 1
.endif // .ifndef KN_DEBUG

.macro todo msg:req
	lea kn_debug_todo_\@(%rip), %rdi
 	// ensure that the stack is properly aligned
	mov %rsp, %rcx
	mov %esp, %eax
	sub $8, %rcx
	and $8, %eax
	cmp $0, %eax
	cmovnz %rcx, %rsp
	jmp abort
.pushsection .data, ""
kn_debug_todo_\@:
	.asciz "todo: \msg\n"
.popsection
.endm

.ifdef KN_DEBUG
    .macro assert_eq lhs:req rhs:req
	cmp \lhs, \rhs
	je assert_eq_\@
	// We just assume (for now) that lhs and rhs arent equal to rsi or rdi
	mov \lhs, %rsi
	mov \rhs, %rdi
	lea assertion_failed(%rip), %rdi
	jmp kn_debug_write_abort
    assert_eq_\@:
    .endm
.else
    .macro assert_eq lhs:req rhs:req
    .endm
.endif

.ifdef KN_DEBUG
    .macro assert_ne lhs:req rhs:req
	cmp \lhs, \rhs
	jne assert_ne_\@
	// We just assume (for now) that lhs and rhs arent equal to rsi or rdi
	mov \lhs, %rsi
	mov \rhs, %rdi
	lea assertion_failed(%rip), %rdi
	jmp kn_debug_write_abort
    assert_ne_\@:
    .endm
.else
    .macro assert_ne lhs:req rhs:req
    .endm
.endif // .macro assert_eq

.macro assert_test lhs:req rhs:req
	test \lhs, \rhs
	jnz assert_test_\@
	// We just assume (for now) that lhs and rhs arent equal to rsi or rdi
	mov \lhs, %rsi
	mov \rhs, %rdi
	lea assertion_failed(%rip), %rdi
	jmp kn_debug_write_abort
assert_test_\@:
.endm // .macro assert_test


// Note that this is not a proper function, it takes arguments 
// at `rsi` and `4dx`.
kn_debug_write_abort:
 	// ensure that the stack is properly aligned
	mov %rsp, %rcx
	mov %esp, %eax
	sub $8, %rcx
	and $8, %eax
	cmp $0, %eax
	cmovnz %rcx, %rsp
	jmp abort

.pushsection .data, ""
assertion_failed:
	.asciz "assertion failed\n" // : %1$ld (0x%1$lx) != %2$ld (0x%2$lx)\n
.popsection

.macro assert_z reg:req
	assert_eq $0, \reg
.endm // assert_z


.globl ddebug