#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#define ll long long
#define U(s)((s)&~15)
#define B(s)((char*)U(s))
#define D(s)((s)>>4)
#define E(n)(1|(n)<<4)
#define F(n)(2|(ll)(n))
#define G(n)((n)?12:4)
#define A(n)(((ll*)U(v))[n])
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
#define T A(1)
#define V A(2)
int H,J;char*S,*O[99999],BF[24];ll M[99999];size_t z,a,b,d;ll p(){ll*f,v;char*t,
c,n[2]={0,0};J=v=0;W(N(S,"\t\n\f\r {}[]():#"))I(*S-35)++S;else W(*S++!=10);W(///
isdigit(*S))v=v*10+*S++-(J=48);I(J)R 1|v<<4;W(islower(*n=c=*S)||isdigit(c)||95==
c)J?++S:(J=1,t=S);I(J)R 4|(ll)Z(t,S-t);I(c==34||c==39){t=++S;W(*S++-c){}R 2|(ll)
Z(t,S-t-1);}++S;I(isupper(c))W(isupper(*S)||*S==95)++S;I(N(n,"TFN"))R c-85?c-78?
8:4:12;v=8|(ll)(f=calloc(40,1));*f=c;I (c==82||c==80)R v;f[1]=p();I(N(n,//yay!:)
"EBC`Q!LDO"))R v;f[2]=p();I(!N(n,"GIS"))R v;f[3]=p();I(c-71&&c-73)f[4]=p();R v;}
ll r(ll);char*tos(ll v){R v&1?sprintf(BF,"%lld",v>>4),BF:v&2?B(v):v<13?v-4?v-8?
"true":"null":"false":tos(r(v));}_Bool tob(ll v){R v&1?v>>4:v&2?*B(v):v<13?v==12
:tob(r(v));}ll ton(ll v){R v&1?v>>4:v&2?strtoll(B(v),0,10):v<13?v==12:ton(r(v));
}ll r(ll v){ll t,t2,t3=1;char*s,*ts,*ts2;FILE*f;I(v&3||v<13)R v;I(v&4)for(J=H;+J
--;)I(!Q(B(v),O[J]))R M[J];I(J=H,v&4)W(J--)I(!Q(B(v),O[J]))R M[J];switch(*(ll*)U
(v)){C'R':R E(rand());C'P':s=0;getline(&s,&z,stdin);R F(Z(s,L(s)));C'E':S=tos(T)
;R r(p());C'B':R T;C'C':R r(r(T));C'`':f=popen(tos(T),"r");s=malloc(b=2048);W((a
=fread(s+d,1,b-d,f)))I((d+=a)==b)s=realloc(s,b*=2);R F(s);C'Q':exit(ton(T));C'!'
:R G(!tob(T));C'L':R E(L(tos(T)));C'D':I((v=r(T))&1)R P("Number(%lld)",v>>4),v;I
(v&2)R P("String(%s)",B(v)),v;I(v-8)R P("Boolean(%s)",v-12?"false":"true"),v;R//
P("Null()"),v;C'O':I((z=L(s=tos(T)))&&s[z]=='\\')s[z]='\0',P("%s", s),s[z]='\\';
else puts(s);R 8;C'+':I(1&(t=r(T)))R E(D(t)+ton(V));s=B(t);ts=tos(V);R F(K(K(///
calloc(1+L(s)+L(ts),1),s),ts));C'-':R E(ton(T)-ton(V));C'*':I((t=r(T))&1)R E(D(t
)*ton(V));s=B(t);ts=malloc(1+L(s)*(t=ton(V)));for(*ts=0;t;--t)K(ts,s);R F(ts);C+
'/':R E(ton(T)/ton(V));C'%':R E(ton(T)%ton(V));C'^':t=ton(T);t2=ton(V);I(t==-1)R
t2&1?-1:1;/*I (t2==-1)R t2&1?-1:1;*/I(t2<2)R t2==1?t:t2==0;for(;t2>0;--t2)t3*=t;
R E(t3);C'<':R G((1&(t=r(T)))?D(t)<ton(V):2&t?Q(B(t),tos(V))<0:tob(V)&&t!=12);C
'>':R G((1&(t=r(T)))?D(t)>ton(V):2&t?Q(B(t),tos(V))>0:!tob(V)&&t==12);C'?':R G((
2&(t=r(T)))?2&(t2=r(V))&&!Q(B(t),B(t2)):t==r(V));C'&':R tob(t=r(T))?r(V):t;C'|':
R tob(t=r(T))?t:r(V);C';':R r(T),r(V);C'=':t=r(V);for(J=0;J<H;++J)I(!Q(B(T),O[J]
))R M[J]=t;R O[H]=B(T),M[H++]=t;C'W':W(tob(T))r(V);R 8;C'I':R r(A(tob(T)?2:3));C
'G':s=tos(T)+ton(V);R F(Z(s,ton(A(3))));C'S':ts=tos(T);t=ton(V);t2=ton(A(3));ts2
=tos(A(4));R F(K(K(strncat(calloc(L(ts)+L(ts2)+t2+1,1),ts,t),ts2),ts+t2));}R 1;}
int main(int c,char**v){srand((int)v);I(!c)R fprintf(stderr,//knight, in c, 2021
"usage: %s (-e 'program' | -f file)\n",*v),1;I('f'-v[1][1])S=v[2];else getdelim(
&S,&z,0,fopen(v[2],"r"));r(p());}
