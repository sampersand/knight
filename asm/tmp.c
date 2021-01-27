long tak(long x, long y, long z) {
	if (y < x) {
		return tak(
			tak(x-1, y, z),
			tak(y-1, z, x),
			tak(z-1, x, y)
		);
	} else {
		return z;
	}
}

int main(int argc, char **argv) {
	printf("%lu\n", tak(
		strtol(argv[1], 0, 10),
		strtol(argv[2], 0, 10),
		strtol(argv[3], 0, 10)
	));
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
