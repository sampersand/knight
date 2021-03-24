#!/usr/bin/env ruby
class Object;alias call itself;class<< self;alias d define_method end;d(:B){self
};d(:coerce){to_i.coerce _1};d(:to_s){call.to_s};d(:to_sym){to_s.to_sym};d(:to_i
){to_s.to_i};d(:to_int){to_i};d(:to_str){to_s};d(:P){|*|gets};d(:E){s(+to_s).()}
d(:R){|*|rand 0..0xffff_ffff};d(:!){!call};d(:Q){exit to_i};d(:`){Kernel.`(to_s)
};d(:C){call.()};d(:L){to_s.size};d(:D){call.tap{$><< _1.x}};d(:^){(call**_1.())
.to_i};d(:+){call+_1.()};d(:-){call-_1.()};d(:O){$><<(s=call.to_s)[..-2];s[-1]!=
?\\&&puts(s[-1])};d(:*){call*_1.()};d(:/){call/_1.()};d(:%){call%_1.()};d(:|){!(
x=call)?_1.():x};d(:<){call<_1.()};d(:G){to_s[_1.to_i,_2.to_i]};d(:>){call>_1.()
};d(??){(s=call).is_a?((r=_1.()).class)&&s==r};d(:&){!(x=call)?x:_1.()};d(:W){_1
.()until !self;nil};d(?=){$E[self]=_1.()};d(?;){call;_1.()};d(:I){(!self ?_2: _1
).()};d(:S){t=+to_s;t[_1.to_i,_2.to_i]=_3.to_s;t}end;class String;d(:!){empty?};
d(:x){"String(#{to_s})"};alias m *;d(:*){m _1.to_i}end;class<< false;d(:>){|_|!1}
d(:!){!0};d(:x){'Boolean(false)'};d(:<){!!_1}end;class<< true;d(:<){|_|!1};d(:>){
!_1};d(:x){'Boolean(true)'};d(:to_i){1};$E={};d(:!){false}end;class<< nil;d(:!){!
0};d(:x){'Null()'};d(:to_s){'null'}end;class Symbol;d(:x){"Identifier(#{to_s})"}
d(:call){$E.fetch(self){abort}};d(:to_s){call.to_s}end;class Integer;d(:/){(to_f
./_1.to_i).truncate};d(:x){"Number(#{to_s})"};d(:!){zero?};end;def s i;i.slice!\
/\A(?:[\s(){}\[\]:]|#.*?\n)*/;i.slice!(/\A\d+/)?$&.to_i: i.slice!(/\A([TF])[A-Z_
]*/)?$1==?T:i.slice!(/\A[a-z_0-9]+/)?$&.to_sym: i.slice!(/\AN[A-Z_]*/)?nil: # /
i.slice!(/\A'([^']*)'|\A"([^"]*)"/)?$+:(a=((i.slice! /\A([A-Z][A-Z_]*|.)/;m=Object
.instance_method$&[0]).arity).succ.times.map{s i};a=[0]if[]==a;proc{m.bind_call *a
})end;%w[-e -f].include?(f=$*.shift)&&(v=$*.shift)&&$*==[]or abort\
"usage: #$0 (-e 'program' | -f file)";s(f=='-f'?open(v,&:read):+v).()
__END__

1080 wide
1200 tall
