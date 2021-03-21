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
    .macro assert op:req lhs:req rhs:req success_jmp:req msg="assertion failed\n"
	\op \lhs, \rhs
	\success_jmp kn_debug_assert_jmp_\@

    .ifc \rhs, %rsi
    .ifc \lhs, %rdi
	mov \rhs, %rax
	mov \lhs, %rsi
	mov %rax, %rdi
    .endif
    .endif
        mov \rhs, %rdi
        mov \lhs, %rsi

	lea kn_debug_assert_msg_\@(%rip), %rdi
	jmp kn_debug_write_abort
    .pushsection .data, ""
    kn_debug_assert_msg_\@:
	.asciz "\msg"
    .popsection

    kn_debug_assert_jmp_\@:
    .endm
.else
    .macro assert op:req lhs:req rhs:req success_jmp:req msg=""
    .endm
.endif

.ifdef KN_DEBUG
    .macro assert_eq lhs:req rhs:req msg="assertion failed\n"
	cmp \lhs, \rhs
	je assert_eq_\@
	// We just assume (for now) that lhs and rhs arent equal to rsi or rdi
	mov \lhs, %rsi
	mov \rhs, %rdx
	lea kn_debug_assert_\@(%rip), %rdi
	jmp kn_debug_write_abort
    .pushsection .data, ""
    kn_debug_assert_\@:
	.asciz "\msg"
    .popsection

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

.macro assert_test lhs:req rhs:req rest="assertion for test failed\n"
	assert test, \lhs, \rhs, jz, "\rest"
.endm


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