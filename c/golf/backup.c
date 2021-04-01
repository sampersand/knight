#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<time.h>
#define ll long long

#define TNUM 1
#define TSTR 2
#define TVAR 4
#define TFNC 8
#define TRU 12
#define FLS 4
#define NUL 8
#define UNMASK(s) ((s) & ~15)
#define ASSTR(s) ((char *) UNMASK(s))
#define ASNUM(s) ((s) >> 4)
#define NEWNUM(n) (TNUM | (n) << 4)
#define NEWSTR(n) (TSTR | (ll) (n))
#define NEWBOOL(n) ((n) ? TRU : FLS)
#define ARG(n) (((ll *) UNMASK(value))[n])

char *stream;

ll parse() {
	int i;
	ll value, *func;
	char c, *tmp,spn[2]={0,0};

	i = value = 0;

	// strip whitespace
	while(strspn(stream, "\t\n\f\r {}[]():#")) {
		if (*stream == '#') while(*stream != '\n') ++stream;
		else ++stream;
	}

	// check for numbers
	while (isdigit(*stream)) i = 1, value = value * 10 + *(stream++) - '0';
	if (i) return TNUM | (value << 4);

	// check for variables
	while(islower(c = *stream) || isdigit(c) || c == '_')
		if (i) ++stream;
		else i=1,tmp=stream;
	if (i) return TVAR | (ll) strndup(tmp, stream - tmp);

	// check for strings
	if (c == '\'' || c == '"') {
		tmp = ++stream; // increase past the start
		while (*stream++ != c);
		return TSTR | (ll) strndup(tmp, stream - tmp - 1);
	}
	++stream;

	// functions
	spn[0] = c;
	if (isupper(c)) while(isupper(*stream) || *stream == '_')++stream;
	if (strspn(spn, "TFN")) return c == 'T' ? TRU : c == 'F' ? FLS : NUL;

	value = TFNC | (ll)(func = calloc(40,1));
	func[0] = c;

	// nullary functions
	if (c == 'R' || c == 'P') return value;

	// unary functions
	func[1] = parse();
	if (strspn(spn, "EBC`Q!LDO")) return value;

	// binary functions
	func[2] = parse();
	if (!strspn(spn, "GIS")) return value;

	// ternary and quaternary funcvtions
	func[3] = parse();
	if (c == 'G'|| c == 'I') return value;

	func[4] = parse();
	return value;
}

ll run(ll);

char *tos(ll value) {
	static char buf[24];
	if (value & TNUM) return sprintf(buf, "%lld", value >> 4), buf;
	if (value & TSTR) return ASSTR(value);
	if (value < 13) return value == TRU ? "true" : value == FLS ? "false" : "null";
	return tos(run(value));
}

_Bool tob(ll value) {
	if (value & TNUM) return value >> 4;
	if (value & TSTR) return *ASSTR(value);
	if (value < 13) return value == TRU;
	return tob(run(value));
}

ll ton(ll value) {
	if (value & TNUM) return value >> 4;
	if (value & TSTR) return strtoll(ASSTR(value), 0, 10);
	if (value < 13) return value == TRU;
	return ton(run(value));
}

int MAPN, i;
char *MAPI[1000];
ll MAP[1000];
size_t size;

ll run(ll value) {
	ll tval,tval2,tval3=1;
	char *str, *tstr,*tstr2;
	FILE* file;
	static size_t tmp,cap,len;

	if (value & (TNUM | TSTR) || value < 13) return value;

	if (value & TVAR)
		for (i = 0; i < MAPN; ++i)
			if (!strcmp(ASSTR(value), MAPI[i])) return MAP[i];

	switch(*(ll*) UNMASK(value)) {
		case 'R': return NEWNUM(rand());
		case 'P': str = 0; getline(&str, &size, stdin); return NEWSTR(strdup(str));

		case 'E': stream = tos(ARG(1)); return run(parse());
		case 'B': return ARG(1);
		case 'C': return run(run(ARG(1)));
		case '`': 
			file=popen(tos(ARG(1)),"r");
			str=malloc(cap=2048);
			while ((tmp = fread(str + len, 1, cap - len, file)))
				if ((len += tmp) == cap) str = realloc(str, cap *= 2);
			return NEWSTR(str);
		case 'Q': exit(ton(ARG(1)));
		case '!': return NEWBOOL(!tob(ARG(1)));
		case 'L': return NEWNUM(strlen(tos(ARG(1))));
		case 'D':
			if ((value=run(ARG(1))) & TNUM) printf("Number(%lld)", value>>4);
			else if (value & TSTR) printf("String(%s)", ASSTR(value));
			else if (value == NUL) printf("Null()");
			else printf("Boolean(%s)", value == TRU ? "true" : "false");
			return value;
		case 'O':
			if ((size = strlen(str = tos(ARG(1)))) && str[size] == '\\') {
				str[size]='\0';
				printf("%s", str);
				str[size]='\\';
			} else puts(str);
			return NUL;

		case '+':
			if (TNUM & (tval = run(ARG(1)))) {
				return NEWNUM(ASNUM(tval) + ton(ARG(2)));
			}
			str=ASSTR(tval);
			tstr=tos(ARG(2));
			return NEWSTR(strcat(strcat(calloc(1+strlen(str)+strlen(tstr),1),str),tstr));
		case '-': return NEWNUM(ton(ARG(1)) - ton(ARG(2)));
		case '*':
			if ((tval = run(ARG(1))) & TNUM) return NEWNUM(ASNUM(tval) * ton(ARG(2)));
			str = ASSTR(tval);
			tstr=malloc(1+strlen(str)*(tval=ton(ARG(2))));
			for(*tstr='\0';tval;--tval)strcat(tstr,str);
			return NEWSTR(tstr);
		case '/': return NEWNUM(ton(ARG(1)) / ton(ARG(2)));
		case '%': return NEWNUM(ton(ARG(1)) % ton(ARG(2)));
		case '^':
			tval=ton(ARG(1));
			tval2=ton(ARG(2));
			if (tval == -1) return tval2 & 1 ? -1 : 1;
			// if (tval2 == -1) return tval2 & 1 ? -1 : 1;
			if (tval2 < 2) return tval2==1 ? tval : tval2==0;
			for(;tval2>0;--tval2)tval3*=tval;
			return NEWNUM(tval3);
			// exit(1);
	// // there's no builtin way to do integer exponentiation, so we have to
	// // do it manually.
	// if (base == 1) result = 1;
	// else if (base == -1) result = exponent & 1 ? -1 : 1;
	// else if (exponent == 1) result = base;
	// else if (exponent == 0) result = 1;
	// else if (exponent < 0) result = 0; // already handled `base == -1`
	// else {
	// 	for (result = 1; exponent > 0; --exponent)
	// 		result *= base;
	// }

		case '<':
			return NEWBOOL(
				(TNUM & (tval=run(ARG(1)))) ? ASNUM(tval) < ton(ARG(2))
				: (TSTR & tval) ? strcmp(ASSTR(tval), tos(ARG(2))) < 0
				: tob(ARG(2)) && tval!=TRU);

		case '>': // todo: memcp then >`?
			return NEWBOOL(
				(TNUM & (tval=run(ARG(1)))) ? ASNUM(tval) > ton(ARG(2))
				: (TSTR & tval) ? strcmp(ASSTR(tval), tos(ARG(2))) > 0
				: !tob(ARG(2)) && tval==TRU);
		case '?':
			return NEWBOOL(
				(TSTR & (tval=run(ARG(1))))
				? (TSTR & (tval2=run(ARG(2)))) && !strcmp(ASSTR(tval), ASSTR(tval2))
				: tval == run(ARG(2)));
		case '&': return tob(tval = run(ARG(1))) ? run(ARG(2)) : tval;
		case '|': return tob(tval = run(ARG(1))) ? tval : run(ARG(2));
		case ';': run(ARG(1)); return run(ARG(2));
		case '=':
			tval=run(ARG(2));
			for (i = 0; i < MAPN; ++i)
				if (!strcmp(ASSTR(ARG(1)), MAPI[i]))
					return MAP[i]=tval;
			MAPI[MAPN]=ASSTR(ARG(1));
			return MAP[MAPN++]=tval;
		case 'W': while(tob(ARG(1))) run(ARG(2)); return NUL;
		case 'I': return run(ARG(tob(ARG(1))?2:3));
		case 'G': str=tos(ARG(1))+ton(ARG(2)); return NEWSTR(strndup(str, ton(ARG(3))));
		case 'S':
			tstr=tos(ARG(1));
			tval=ton(ARG(2));
			tval2=ton(ARG(3));
			tstr2=tos(ARG(4));
			return NEWSTR(strcat(strcat(strncat(calloc(
				strlen(tstr)+strlen(tstr2)+tval2+1,1),tstr,tval),tstr2),tstr+tval2));
		default:
		printf("invalid character '%c'", (char) (*(ll*) UNMASK(value)) );
		exit(1);
	}
}

int main(int argc, char**argv){
	srand(time(0));
	if(!argc)fprintf(stderr,"usage: %s (-e 'program' | -f file)\n",*argv),exit(1);
	if(argv[1][1]-'f')stream=argv[2]; // ie if it's `-e`
	else getdelim(&stream,&size,'\0',fopen(argv[2],"r"));
	run(parse());
}
