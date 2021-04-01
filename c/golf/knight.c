/*

*/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#define ll long long

#define U(s) ((s) & ~15)
#define ASSTR(s) ((char *) U(s))
#define ASNUM(s) ((s) >> 4)
#define NN(n) (1 | (n) << 4)
#define NS(n) (2 | (ll) (n))
#define NB(n) ((n) ? 12 : 4)
#define A(n) (((ll *) U(v))[n])
#define L strlen
#define P printf
#define W while
#define K strcat
#define I if
#define R return
#define C case
#define Q strcmp
#define N strspn
#define Z strndup
#define A1 A(1)
#define A2 A(2)
int MN, i;
char*S,*MI[1000],BF[24];
ll M[1000];
size_t z,tmp,cap,len;

ll p() {
	ll*f,v;char*t,c,n[2]={0,0};i=v=0;
	W(N(S,"\t\n\f\r {}[]():#"))I(*S-35)++S;else W(*S++!=10);
	W(isdigit(*S))v=v*10+*S++-(i=48);I(i)R 1|v<<4;
	W(islower(*n=c=*S)||isdigit(c)||c==95)i?++S:(i=1,t=S);I(i)R 4|(ll)Z(t,S-t);
	I(c==34||c==39){t=++S;W(*S++-c){}R 2|(ll)Z(t,S-t-1);}

	++S;
	I(isupper(c))W(isupper(*S)||*S==95)++S;
	I(N(n,"TFN"))R c-85?c-78?8:4:12;
	v=8|(ll)(f=calloc(40,1));
	*f=c;I (c==82||c==80)R v;
	f[1]=p();I(N(n,"EBC`Q!LDO"))R v;
	f[2]=p();I(!N(n,"GIS"))R v;
	f[3]=p();I(c-71&&c-73)f[4]=p();R v;
}

ll r(ll);

char*tos(ll v){R v&1?sprintf(BF,"%lld",v>>4),BF:v&2?ASSTR(v):v<13?v-4?v-8?
"true":"null":"false":tos(r(v));}
_Bool tob(ll v){R v&1?v>>4:v&2?*ASSTR(v):v<13?v==12:tob(r(v));}
ll ton(ll v){R v&1?v>>4:v&2?strtoll(ASSTR(v),0,10):v<13?v==12:ton(r(v));}

ll r(ll v){ll t,t2,t3=1;char*s,*ts,*ts2;FILE*f;
	I(v&3||v<13)R v;
	I(v&4)for(i=MN;i--;)I(!Q(ASSTR(v),MI[i]))R M[i];
	I(i=MN,v&4)W(i--)I(!Q(ASSTR(v),MI[i]))R M[i];

	switch(*(ll*)U(v)){
	C'R':R NN(rand());
	C'P':s=0;getline(&s,&z,stdin);R NS(strdup(s));
	C'E':S=tos(A1);R r(p());
	C'B':R A1;
	C'C':R r(r(A1));
	C'`':f=popen(tos(A1),"r"); s=malloc(cap=2048);
		W((tmp=fread(s+len,1,cap-len,f)))I((len+=tmp)==cap)s=realloc(s,cap*=2);
		R NS(s);
	C'Q':exit(ton(A1));
	C'!':R NB(!tob(A1));
	C'L':R NN(L(tos(A1)));
	C'D':I((v=r(A1))&1)R P("Number(%lld)",v>>4),v;I(v&2)R P("String(%s)",ASSTR(v)),v;
		I(v-8)R P("Boolean(%s)",v-12?"false":"true"),v;R P("Null()"),v;
	C'O':I((z=L(s=tos(A1)))&&s[z]=='\\')s[z]='\0',P("%s", s),s[z]='\\';
		else puts(s);R 8;
	C'+':I(1&(t=r(A1)))R NN(ASNUM(t)+ton(A2));
		s=ASSTR(t);ts=tos(A2);R NS(K(K(calloc(1+L(s)+L(ts),1),s),ts));
	C'-':R NN(ton(A1)-ton(A2));
	C'*':I((t=r(A1))&1)R NN(ASNUM(t)*ton(A2));
		s=ASSTR(t);ts=malloc(1+L(s)*(t=ton(A2)));
		for(*ts=0;t;--t)K(ts,s);
		R NS(ts);
	C'/':R NN(ton(A1)/ton(A2));
	C'%':R NN(ton(A1)%ton(A2));
	C'^':t=ton(A1);t2=ton(A2);
		I(t==-1)R t2&1?-1:1;
		// I (t2 == -1) R t2 & 1 ? -1 : 1;
		I(t2<2)R t2==1?t:t2==0;
		for(;t2>0;--t2)t3*=t;
		R NN(t3);
	C'<':R NB((1&(t=r(A1)))?ASNUM(t)<ton(A2):2&t?Q(ASSTR(t),tos(A2))<0:tob(A2)&&t!=12);
	C'>':R NB((1&(t=r(A1)))?ASNUM(t)>ton(A2):2&t?Q(ASSTR(t),tos(A2))>0:!tob(A2)&&t==12);
	C'?':R NB((2&(t=r(A1)))?2&(t2=r(A2))&&!Q(ASSTR(t),ASSTR(t2)):t==r(A2));
	C'&':R tob(t=r(A1))?r(A2):t;
	C'|':R tob(t=r(A1))?t:r(A2);
	C';':R r(A1),r(A2);
	C'=':t=r(A2);for(i=0;i<MN;++i)I(!Q(ASSTR(A1),MI[i]))R M[i]=t;
		R MI[MN]=ASSTR(A1),M[MN++]=t;
	C'W':W(tob(A1))r(A2);R 8;
	C'I':R r(A(tob(A1)?2:3));
	C'G':s=tos(A1)+ton(A2);R NS(Z(s,ton(A(3))));
	C'S':ts=tos(A1);t=ton(A2);t2=ton(A(3));ts2=tos(A(4));
		R NS(K(K(strncat(calloc(L(ts)+L(ts2)+t2+1,1),ts,t),ts2),ts+t2));
	}
	R 1;
}

int main(int c,char**v){
	srand((int)v);
	I(!c)R fprintf(stderr,"usage: %s (-e 'program' | -f file)\n",*v),1;
	I('f'-v[1][1])S=v[2];
	else getdelim(&S,&z,0,fopen(v[2],"r"));
	r(p());
}
