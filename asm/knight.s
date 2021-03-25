// RDI, RSI, RDX, RCX, R8, R9
.include "debugh.s"

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

.globl kn_startup
kn_startup:
	sub $8, %rsp
	xor %edi, %edi
	call _time
	mov %eax, %edi
	call _srand
	mov $4096, %edi /* amount of identifiers to start with */
	call kn_env_startup
	add $8, %rsp
	ret

.globl kn_shutdown
kn_shutdown:
	jmp kn_env_shutdown	

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