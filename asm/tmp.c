int main() {
	static char foo[8];

	foo[0] = 123;

	printf("%lld", *((long long *) foo));
}