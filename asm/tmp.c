int check(long l, long r) {
	return l % r;
}
int main(int argc, char **argv) {
	long lhs = strtol(argv[1], 0, 10);
	long rhs = strtol(argv[2], 0, 10);

	return lhs % rhs;
}


// void ltoa(long val, char *idx) {
// 	do {
// 		long tmp = val;
// 		val /= 10;
// 		*(idx++) = (tmp - val * 10);
// 	} while (val);
// }

// char *doit(long val) {
// 	char ret[21];
// 	if (val == 0) return "0";
// 	char *idx = ret;
// 	if (val < 0) { *(idx++) = '-'; val = -val; }
// 	ltoa(val, idx);
// 	return ret;
// }

// int main(int argc, char **_) {
// 	(void) _;
// 	volatile long x = argc;
// 	volatile char *y = doit(x);

// }
