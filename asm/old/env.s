.include "debug.s"

.bss
kn_env_data:
kn_env_capacity:
	.quad 0
kn_env_length:
	.quad 0
kn_env_keys:
	.quad 0
kn_env_vals:
	.quad 0

.text
/*
struct kn_env_t {
	size_t capacity;
	size_t length;

	const char **keys;
	struct kn_value_t *vals;
};

// The singleton value of `kn_env_t`.
static struct kn_env_t KN_ENV;
*/

.globl kn_env_initialize	
kn_env_initialize:
	push %rbx
	mov %rdi, %rbx
	movq %rdi, kn_env_capacity(%rip)
	imul $8, %rbx
	mov %rbx, %rdi
	call xmalloc
	mov %rax, kn_env_keys(%rip)
	mov %rbx, %rdi
	call xmalloc
	mov %rax, kn_env_vals(%rip)
	pop %rbx
	ret

.globl kn_env_get
kn_env_get:
	todo "kn_env_get"
	// NOTE: if the value does not exist, crash.

.globl kn_env_set
kn_env_set:
	push %rbx
	push %r12
	push %r13
	mov %rdi, %r12
	mov %rsi, %r13
	xor %ebx, %ebx

	cmp %ebx, kn_env_length(%rip)
	

	pop %r13
	pop %r12
	pop %rbx
	ret
