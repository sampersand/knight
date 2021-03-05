// RDI, RSI, RDX, RCX, R8, R9
.include "debug.s"

.data
source:
	# .asciz "; = a 3 : O + 'a*4=' * a 4"
	.asciz "
# foo bar
; = a 3
: D a
"
.text
.globl _main
_main:
	push %rbx

	call process_arguments    // process command line arguments
	mov %rax, %rbx
	call kn_initialize        // Start up knight

	mov %rbx, %rdi
	call kn_run

	xor %eax, %eax
	pop %rbx
	ret

process_arguments:
	; mov 16(%rsi), %rdi
	lea source(%rip), %rdi
	jmp _strdup

.globl kn_run
kn_run:
	push %rbx
	call kn_parse      /* parse the stream */
	mov %rax, %rbx     /* save the parsed value for later so we can free it */
	mov %rax, %rdi
	call kn_value_run  /* execute the parsed value */
	mov %rbx, %rdi
	mov %rax, %rbx     /* record the result of running it */
	call kn_value_free /* free the memory of the parsed value */
	mov %rbx, %rax     /* restore the return value */
	pop %rbx
	ret

kn_initialize:
	sub $8, %rsp
	xor %edi, %edi
	call _time
	mov %eax, %edi
	call _srand
	mov $4096, %edi /* amount of identifiers to start with */
	call kn_env_initialize
	add $8, %rsp
	ret

.globl xmalloc
xmalloc:
	sub $8, %rsp
	call _malloc
	cmp $0, %rax
	je 0f
	add $8, %rsp
	ret
0:
	lea kn_allocation_failure(%rip), %rdi
	call abort

.pushsection .text, ""
kn_allocation_failure:
	.asciz "allocation failure occured!\n"
.popsection

.globl abort
abort:
	call _printf
	call die

.globl die
die:
	mov $1, %rdi
	jmp _exit


.globl ddebug
ddebug:
	sub $8, %rsp
	call debug
	call die

dbg_fmt:
	.asciz "\
(rax) %8$ 20ld\11\11%8$ 16lx (rax)\n\
(rbx) %9$ 20ld\11\11%9$ 16lx (rbx)\n\
(rcx) %3$ 20ld\11\11%3$ 16lx (rcx)\n\
(rdx) %2$ 20ld\11\11%2$ 16lx (rdx)\n\
(rdi) %16$ 20ld\11\11%16$ 16lx (rdi)\n\
(rsi) %1$ 20ld\11\11%1$ 16lx (rsi)\n\
(rsp) %22$ 20ld\11\11%22$ 16lx (rsp)\n\
(rbp) %7$ 20ld\11\11%7$ 16lx (rbp)\n\
(r8 ) %4$ 20ld\11\11%4$ 16lx (r8)\n\
(r9 ) %5$ 20ld\11\11%5$ 16lx (r9)\n\
(r10) %10$ 20ld\11\11%10$ 16lx (r10)\n\
(r11) %11$ 20ld\11\11%11$ 16lx (r11)\n\
(r12) %12$ 20ld\11\11%12$ 16lx (r12)\n\
(r13) %13$ 20ld\11\11%13$ 16lx (r13)\n\
(r14) %14$ 20ld\11\11%14$ 16lx (r14)\n\
(r15) %15$ 20ld\11\11%15$ 16lx (r15)\n\n"

.globl debug
debug:
// RDI, RSI, RDX, RCX, R8, R9
	//sub $88, %rsp
	push %rsp
	push %rsi
	push %rdx
	push %rcx
	push %r8
	push %r9
	push %rdi
	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %rbx
	push %rax
	push %rbp
	sub $8, %rsp
	lea dbg_fmt(%rip), %rdi
	call _printf
	add $8, %rsp
	pop %rbp
	pop %rax
	pop %rbx
	pop %r10
	pop %r11
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	pop %rdi
	pop %r9
	pop %r8
	pop %rcx
	pop %rdx
	pop %rsi
	add $8, %rsp
	ret
