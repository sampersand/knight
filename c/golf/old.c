#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<time.h>
#define ll long long
#define W while
#define R return
#define RV R v;
#define SS strspn
#define C(c) case c:
#define US(x) ((char*)(x&~8))
#define U(x) ((ll*)(x&~8))
#define V(n) U(v)[n]
#define P printf
int MN;
char*MI[1000];
ll MV[1000];

char*s,*t;
int i;
size_t z,y;

ll ps(){char c,p[2];ll*rp,v;
	W(SS(s,"\t\n\f\r {}[]():#"))if(*s++=='#')W(*s!='\n')++s;
	i=v=0;W(isdigit(*s))i=1,v=v*10+*s++-'0';if(i){R 1|(v<<4); }
	W(islower(c=*s)||isdigit(c)||c=='_')!i?i=1,t=s:++s;
		if(i){v=4|(ll)strndup(t,s-t);RV}
	if((c=*s++)=='\''||c=='"'){t=s-2;W(*s++!=c){}v=2|(ll)strndup(t,s-t-1);RV}
	if(isupper(c))W(isupper(*s)||*s=='_')++s;
	if(c=='T'||c=='F'||c=='N')R c=='T'?12:c=='F'?4:8;
	rp=malloc(32);v=8|((ll)rp);*rp=*p=c;
	if(c=='R'||c=='P')RV
	rp[1]=ps();if(SS(p,"EBC`Q!LDO"))RV
	rp[2]=ps();if(SS(p,"+-*/%^<>?&|;=W"))RV
	rp[3]=ps();if(c=='G'||c=='S')RV
	rp[4]=ps();RV
}

ll rn(ll);

char* tos(ll v){static char buf[24];
	return v&1?(sprintf(buf,"%lld",v>>4),buf):v&2?US(v):
		v<13?(v==4?"false":v==8?"null":"true"):tos(rn(v));
}
int tob(ll v){return v&1?v<2:v&2?!*US(v):v<13?v==12:tob(rn(v));}
int toi(ll v){return v&1?v>>4:v&2?strtoll(US(v),0,10):v<13?v==12:toi(rn(v));}

ll rn(ll v){ll a,b;
	if(v&3||v<13)RV
	if(v&4)for(i=0;i<MN;++i)if(!strcmp((char*)(v-4),MI[i]))R MV[i];

	switch(*(ll*)(v-=8)){
		C('B')RV
		C('R')R 1|rand()<<4;
		C('P')s=0;getline(&s,&z,stdin);R 2|(ll)strdup(s);
		C('E')s=tos(V(1));R ps();
		C('C')R rn(rn(V(1)));
		C('`')exit(2);
		C('Q')exit(toi(V(1)));
		C('!')R tob(V(1))?12:4;
		C('L')R 1|strlen(tos(V(1)))<<4;
		C('D')v=rn(V(1));
			v&1?P("Number(%lld)",v>>4):
			v&2?P("String(%s)",US(v)):
			v==8?P("Null()"):P("Boolean(%s)",v==4?"false":"true");
			RV
		C('O')s=tos(V(1));*s&&s[i=strlen(s)-1]=='\\'?
			s[i]='\0',P("%s",s),s[i]='\\':P("%s\n",s);R 8;
		C('+')R (a=rn(V(1)))&2
			?2|(ll)(strcat(US(a),tos(V(2))))
			:1|((a>>4) + toi(V(2)))<<4;
		C('-')R 1|(toi(V(1)) - toi(V(2)))<<4;
		C('*')a=rn(V(1));b=toi(V(2));if(a&1)R 1|((a>>4)*b)<<4;
			t=malloc(strlen(s=US(a))*b+2); // this is ub, but cba to fix it rn.
			t[0]='\0';
			for(i=0;i<=b;++i)strcat(t,s);R 2|(ll)t;
		C('/')R 1|(toi(V(1)) / toi(V(2)))<<4;
		C('%')R 1|(toi(V(1)) % toi(V(2)))<<4;
		C('^')exit(3);
		C('<')R ((a=rn(V(1)))&1?a<toi(V(2)):a&2?strcmp(US(a),tos(V(2)))<0:
			tob(V(2))&&a==4)?12:4;
		C('>')R ((a=rn(V(1)))&1?a>toi(V(2)):a&2?strcmp(US(a),tos(V(2)))>0:
			!tob(V(2))&&a!=4)?12:4;
		C('?')R ((a=rn(V(1)))&2?!strcmp(US(a),tos(V(2))):a==rn(V(2)))?12:4;
		C('&')R tob(a=rn(V(1)))?rn(V(2)):a;
		C('|')R tob(a=rn(V(1)))?a:rn(V(2));
		C(';')rn(V(1));R rn(V(2));
		C('=')exit(4);
		C('W')while(tob(V(1)))rn(V(2));R 8;
		C('I')R V(tob(V(1))?2:3);
		C('G') {
			char *string = tos(V(1));
			printf("[%1$c](%1$x, 0)\n", string[0] & 0xff);
			printf("[%1$c](%1$x, 1)\n", string[1] & 0xff);
			printf("[%1$c](%1$x, 2)\n", string[2] & 0xff);
			printf("[%1$c](%1$x, 3)\n", string[3] & 0xff);
			printf("[%1$c](%1$x, 4)\n", string[4] & 0xff);
			printf("[%1$c](%1$x, 5)\n", string[5] & 0xff);
			printf("[%1$c](%1$x, 6)\n", string[6] & 0xff);
			ll w = toi(V(2));
			s=tos(V(1));s+=toi(V(2));R 2|(ll)strndup(s,toi(V(3)));
		}
		C('S')exit(5);
	}

	exit(1);
}

int main(int c, char**v) {
	srand(time(0));
	if(c!=3)fprintf(stderr,"usage: %s (-e 'program' | -f file)\n",*v),exit(1);
	if(v[1][1]-'f')s=v[2]; // ie if it's `-e`
	else exit(1);
	// a:
	// MN=1;

	// MI[0] = "a";
	// MV[0] = (1<<4)|1;
	// s=" P \"a1ac_\" #   4\n1";
	// s="  D L T";
	rn(ps());
	// printf()
	// printf("[%s]%d\n", v-1,9);
	// printf("%lld", strspn(" 12 "," \t\n"));
	// malloc(0);
}