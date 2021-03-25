.include "envh.s"

/*
variable:
	[8 bytes] value
	[8 bytes] pointer to name
bucket:
	[8 bytes] length
	[8 bytes] pointer to bucket
*/

.ifndef KN_ENV_NBUCKETS
.equ KN_ENV_NBUCKETS, 65536
.endif

.ifndef KN_ENV_CAPACITY
.equ KN_ENV_CAPACITY, 256
.endif

.equ KN_ENV_VAR_SIZE, 16
.equ KN_ENV_BUCKET_SIZE, 16
.equ KN_ENV_BUCKET_OFFSET_PTR, 8

.globl kn_env_startup
kn_env_startup:
	push %rbx

	;/* load starting and ending positions */
	lea KN_ENV_NBUCKETS(%rip), %ebx
	lea , %r12
0:
	;/* allocate and assign the bucket's pointer */
	mov $(KN_ENV_CAPACITY * KN_ENV_VAR_SIZE), %rdi
	call xmalloc
	mov %rax, KN_ENV_BUCKET_OFFSET_PTR(%rbx)

	;/* increment the current bucket and allocate the next one */
	add $KN_ENV_BUCKET_SIZE, %rbx
	cmp %(KN_ENV_NBUCKETS * KN_ENV_BUCKET_SIZE)(%rbx), %rbx
	jne 0b

	/* restore the startup and return */
	pop %rbx
	ret
/*
setup:
        pushq   %rbx
        movl    $65536, %ebx
.L2:
        movl    $4096, %edi
        call    malloc
        movq    %rax, map+8(%rip)
        subl    $1, %ebx
        jne     .L2
        popq    %rbx
        ret
map:
        .zero   1048576
        */

.globl kn_env_shutdown
kn_env_shutdown:
	ret


.globl kn_env_fetch
kn_env_fetch:
	push %rbx
	mov %rdi, %rbx
	mov $KN_ENV_VAR_SIZE, %rdi
	call xmalloc

.bss
kn_env_map:
	.zero (KN_ENV_BUCKET_SIZE * KN_ENV_NBUCKETS)

/*

void kn_env_startup() {
	// make sure we haven't started, and then set started to true.
	assert(!kn_env_has_been_started && (kn_env_has_been_started = true));
	assert(KN_ENV_CAPACITY != 0);

	for (size_t i = 0; i < KN_ENV_NBUCKETS; ++i) {
		kn_env_map[i] = (struct kn_env_bucket_t) {
			// technically redundant, b/c it's set to 0 in `kn_env_shutdown`.
			.length = 0,
			.capacity = KN_ENV_CAPACITY,
			.variables = xmalloc(sizeof(struct kn_variable_t [KN_ENV_CAPACITY]))
		};
	}
}
*/