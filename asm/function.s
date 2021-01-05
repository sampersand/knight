# RDI, RSI, RDX, RCX, R8, R9

.global get_function
get_function:
	sub $8, %rsp
	cmp $'-', %rdi
	jne 0f
	lea kn_add(%rip), %eax
	jmp done_getting_function
0:
done_getting_function:
	add $8, %rsp
	ret

	.quad 2
kn_add:
	ret
# 
# foo1:
# 	ret
	# 
# FUNCTIONS:
#         .zero   528
#         .long   1
#         .zero   4
#         .quad   1010
#         .zero   48
#         .long   2
#         .zero   4
#         .quad   1017
#         .long   2
#         .zero   4
#         .quad   1025
#         .zero   48
#         .long   2
#         .zero   4
#         .quad   1015
#         .long   2
#         .zero   4
#         .quad   1013
#         .zero   16
#         .long   2
#         .zero   4
#         .quad   1014
#         .zero   16
#         .long   2
#         .zero   4
#         .quad   1016
#         .zero   176
#         .long   2
#         .zero   4
#         .quad   1022
#         .long   2
#         .zero   4
#         .quad   1020
#         .long   2
#         .zero   4
#         .quad   1023
#         .long   2
#         .zero   4
#         .quad   1021
#         .long   2
#         .zero   4
#         .quad   1019
#         .zero   32
#         .long   9
#         .zero   4
#         .quad   foo1
#         .long   1
#         .zero   4
#         .quad   1007
#         .zero   16
#         .long   1
#         .zero   4
#         .quad   1006
#         .long   0
#         .zero   4
#         .quad   1003
#         .long   3
#         .zero   4
#         .quad   1028
#         .zero   16
#         .long   3
#         .zero   4
#         .quad   1027
#         .zero   32
#         .long   1
#         .zero   4
#         .quad   1011
#         .zero   16
#         .long   0
#         .zero   4
#         .quad   1004
#         .long   1
#         .zero   4
#         .quad   1012
#         .long   0
#         .zero   4
#         .quad   1000
#         .long   1
#         .zero   4
#         .quad   1009
#         .long   0
#         .zero   4
#         .quad   1001
#         .long   4
#         .zero   4
#         .quad   1029
#         .long   0
#         .zero   4
#         .quad   1002
#         .zero   32
#         .long   2
#         .zero   4
#         .quad   1024
#         .zero   96
#         .long   2
#         .zero   4
#         .quad   1018
#         .zero   16
#         .long   1
#         .zero   4
#         .quad   1008
#         .zero   432
#         .long   2
#         .zero   4
#         .quad   1026
#         .zero   2080
