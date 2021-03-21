/*
string layout:
[1 byte] 0 allocated, 1 for embedded
emebdded:
	[1 byte] length
	[2 bytes] padding
	[4 bytes] refcount
	[23 bytes] embedded
	[1 byte] '\0'
allocated:
	[3 bytes] padding
	[4 bytes] refcount (note same position as embedded)
	[8 bytes] pointer to string
	[8 bytes] length
	[8 bytes] padding
*/

.equ KN_STR_ALLOC, 0
.equ KN_STR_EMBED, 1
.equ KN_STR_EMBED_LENGTH, 23
.equ KN_STR_SIZE, 32
.equ KN_STR_OFFSET_RC, 4
.equ KN_STR_E_OFFSET_DATA, 8
.equ KN_STR_E_OFFSET_LEN, 1
.equ KN_STR_A_OFFSET_PTR, 8
.equ KN_STR_A_OFFSET_LEN, 16

/* clones a string. */
.macro kn_str_clone reg:req
	incb KN_STR_OFFSET_RC(\reg)
.endm