/*
string layout:
[1 byte] 0 allocated, 1 for embedded
either:
	[1 byte] length
	[30 bytes] embedded
or:
	[7 bytes] padding
	[8 bytes] pointer to string
	[8 bytes] length
	[8 bytes] refcount
*/

