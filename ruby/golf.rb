#!/usr/bin/env ruby
class Object;alias call itself;class<< self;alias d define_method end;d(:B){self
};d(:coerce){to_i.coerce _1};d(:to_s){call.to_s};d(:to_sym){to_s.to_sym};d(:to_i
){to_s.to_i};d(:to_int){to_i};d(:to_str){to_s};d(:P){|*|gets};d(:E){s(+to_s).()}
d(:R){|*|rand 0..0xffff_ffff};d(:!){!call};d(:Q){exit to_i};d(:`){Kernel.`(to_s)
};d(:C){call.()};d(:L){to_s.size};d(:D){call.tap{$><< _1.inspect}};d(:O){$><<(s=
call.to_s)[..-2];s[-1]!=?\\&&puts(s[-1])};d(:+){call+_1.()};d(:-){call-_1.()};d(
:*){call*_1.()};d(:/){call/_1.()};d(:%){call%_1.()};d(:^){(call**_1.()).to_i};d(
:<){call<_1.()};d(??){(s=call).is_a?((r=_1.()).class)&&s==r};d(:G){to_s[_1.to_i,
_2.to_i]};d(:>){call>_1.()};d(?;){call;_1.()};d(:&){!(x=call)?x:_1.()};d(:W){_1.
()until !self;nil};d(?=){$E[_1]=_2.()};d(:|){!(x=call)?_1.():x};d(:S){t=+to_s;t[
_1.to_i,_2.to_i]=_3.to_s;t};d(:I){(!self ?_2: _1).()}end;class String;d(:inspect
){"String(#{to_s})"};d(:!){empty?};alias m *;d(:*){m _1.to_i}end;class<< false;d(
:!){!0};d(:inspect){'Boolean(false)'};d(:>){|_|!1};d(:<){!!_1}end;class<< true;d(
:inspect){'Boolean(true)'};d(:<){|_|!1};d(:>){!_1};d(:to_i){1};$E={};d(:!){false
}end;class Symbol;d(:to_s){call.to_s};d(:inspect){"Identifier(#{to_s})"};d(:call
){$E.fetch(self){abort}}end;class<< nil;d(:inspect){'Null()'};d(:to_s){'null'};d(
:!){!0}end;class Integer;d(:inspect){"Number(#{to_s})"};d(:!){zero?};d(:/){(to_f
./_1.to_i).truncate}end;def s i;i.slice! /\A(?:[\s(){}\[\]:]|#.*?\n)*/;i.slice!(
/\A\d+/)?$&.to_i: i.slice!(/\A([TF])[A-Z_]*/)?$1==?T:i.slice!(/\A[a-z_0-9]+/)?$&
.to_sym: i.slice!(/\AN[A-Z_]*/)?nil: i.slice!(/\A'([^']*)'|\A"([^"]*)"/)?$+:(a=(
(i.slice! /\A([A-Z][A-Z_]*|.)/;m=Object.instance_method$&[0]).arity).succ.times.
maps i};a=[0]if[]==a;proc{m.bind_call *a})end;%w[-e -f].include?(f=$*.shift)&&(
v=$*.shift)&&$*==[]or abort"usage: #$0 (-e 'program' | -f file)";s(f=='-f'?open(
v,&:read):+v).()


__END__
case
when i.slice!(/\A\d+/)then$&.to_i
when i.slice!(/\A([TF])[A-Z]*/)then$1 == ?T
when i.slice!(/\AN[A-Z]*/)then
when i.slice!(/\A[a-z_][a-z0-9_]*/)then$&.to_sym
when i.slice!(/\A(?:'([^']*)'|"([^"]*)")/)then$+
when (meth = Object.instance_method(i[0]) rescue false)then	i.slice! /\A([A-Z][A-Z_]*|.)/
	args = meth.arity.succ.times.map { s i }
	args=[0] if args.empty?
	proc { meth.bind_call(*args) }
else abort "nothing found"
end
end
%w%-e -f%.include?(f=$*.shift)&&(v=$*.shift)&&$*.empty?or abort"usage: #$0 (-e 'program' | -f file)"
s(f=='-e'? +v : open(v, &:read)).()
