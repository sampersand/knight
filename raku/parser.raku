unit module Knight;

# use MONKEY-SEE-NO-EVAL;
# 
# EVAL qx<cat classes.raku>;
class Identifier { ... }

role Value {
	method assign(Value $value, --> Value) {my $x = $.Str; say "[$x]]"; Identifier.new($x).assign: $value }
	#method assign(Value $value, --> Value) { Identifier.new($.Str).assign: $value }
	method lth(Value $rhs, --> Bool) { $.cmp($rhs) === Less }
	method gth(Value $rhs, --> Bool) { $.cmp($rhs) === More }

	multi method eql(Value $, --> False) is pure {}
}

role TypedValue[::T, $cmp, $eql] does Value {
	has T $!value is built;

	method new(T $value)    { self.bless :$value }
	method BUILD(T :$value) { $!value = $value }

	method cmp(Value $rhs, --> Order) { $cmp($!value, T($rhs)) }
	multi method eql(::?CLASS $rhs, --> Bool) { $eql($!value, T($rhs)) }

	method Str(--> Str) is pure   { $!value.Str }
	method Bool(--> Bool) is pure { $!value.Bool }
	method Int(--> Int) is pure   { $!value.Int }
	method run(--> Value) is pure { self }
}

class Boolean does TypedValue[Bool, * <=> *, * == *] {
	method Str(--> Str) is pure { $!value ?? 'true' !! 'false' }
}

class Null does Value {
	method Str(--> 'null') is pure { }
	method Bool(--> False) is pure { }
	method Int(--> 0) is pure { }

	method cmp(Value $) is pure { die 'Cannot compare Null.' }
	multi method eql(Null $, --> True) is pure { }

	method run(--> Value) { self }
}

class String does TypedValue[Str, * cmp *, * eq *] {
	method Int(--> Int) is pure {
		$!value ~~ /^ <[\d]>* /;
		$<>.Int
	}

	method add(Value $rhs, --> String) { String.new: $!value ~ $rhs.Str }
	method mul(Value $rhs, --> String) { String.new: $!value x $rhs.Str }
}

class Number does TypedValue[Int, * <=> *, * == *] {
	method add(Value $rhs, --> Number) { Number.new: $!value + $rhs.Int }
	method sub(Value $rhs, --> Number) { Number.new: $!value - $rhs.Int }
	method mul(Value $rhs, --> Number) { Number.new: $!value * $rhs.Int }
	method div(Value $rhs, --> Number) { Number.new: $!value div ($rhs.Int or die "Cannot divide by zero!") }
	method mod(Value $rhs, --> Number) { Number.new: $!value mod ($rhs.Int or die "Cannot modulo by zero!") }
	method pow(Value $rhs, --> Number) { Number.new: $!value ** $rhs.Int }
}

class NonLiteral is Value {
	method cmp(Value $rhs, --> Order) { $.run.cmp($rhs) }
	multi method eql(::?CLASS $rhs, --> Bool) { $.run.cmp($rhs) }

	method Str(--> Str)   { $.run.Str }
	method Bool(--> Bool) { $.run.Bool }
	method Int(--> Int)   { $.run.Int }
}

class Identifier is NonLiteral {
	has Str $!ident is built;

	my %ENV;

	method new(Str $ident) { self.bless :$ident }

	method run(--> Value)  {
		unless $!ident ~~ %ENV {
			say %ENV.keys;
			die "unknown variable '$!ident'" unless $!ident ~~ %ENV;
		}
		%ENV{$!ident}
	}

	method assign(Value $value, --> Value) {
		say $!ident;
		%ENV{$!ident} = $value;
		$value
	}
}

sub run($) { ... }

class Function is NonLiteral {
	has $!func is built;
	has @!args is built;

	our %FUNCS = (
		'P' => sub (--> Value) { String.new: get }
		'R' => sub (--> Value) { Number.new: (^0xffff_ffff).pick }

		#'E' => sub (Value $str, --> Value)    { Knight::run $str.Str }
		'E' => sub (Value $str, --> Value)    { run $str.Str }
		'B' => sub (Value $block, --> Value)  { $block }
		'C' => sub (Value $block, --> Value)  { $block.run.run }
		'`' => sub (Value $str, --> String)   { String.new: qqx<$str> }
		'Q' => sub (Value $code)              { exit $code }
		'!' => sub (Value $bool, --> Boolean) { Boolean.new: !$bool }
		'L' => sub (Value $str, --> Number)   { Number.new: $str.Str.chars }
		'O' => sub (Value $str, --> Value) {
			my $result = $str.run;
			my $to-output = $result.Str;

			if $to-output.substr(*-1) eq '\\' {
				print $to-output.substr(0, *-1);
			} else {
				say $to-output;
			}

			$result;
		}

		'+' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.add: $rhs.run }
		'-' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.sub: $rhs.run }
		'*' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.mul: $rhs.run }
		'/' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.div: $rhs.run }
		'%' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.mod: $rhs.run }
		'^' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.run.pow: $rhs.run }
		'<' => sub (Value $lhs, Value $rhs, --> Value) { Boolean.new: $lhs.run.lth: $rhs.run }
		'>' => sub (Value $lhs, Value $rhs, --> Value) { Boolean.new: $lhs.run.gth: $rhs.run }
		'?' => sub (Value $lhs, Value $rhs, --> Value) { Boolean.new: $lhs.run.eql: $rhs.run }
		'&' => sub (Value $lhs is copy, Value $rhs, --> Value) { ($lhs = $lhs.run) ?? $rhs.run !! $lhs }
		'|' => sub (Value $lhs is copy, Value $rhs, --> Value) { ($lhs = $lhs.run) ?? $lhs !! $rhs.run }
		'=' => sub (Value $lhs, Value $rhs, --> Value) { $lhs.assign: $rhs.run }
		';' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run;
			$rhs.run;
		}

		'W' => sub (Value $cond, Value $body, --> Value) {
			my $ret = Null.new;

			$ret = $body.run while $cond;

			$ret;
		}

		'I' => sub (Value $cond, Value $iftrue, Value $iffalse, --> Value) { $cond.run ?? $iftrue.run !! $iffalse.run }
		'G' => sub (Value $str, Value $idx, Value $len, --> Value) { String.new: $str.Str.substr($idx.Int, $len.Int) }

		'S' => sub (Value $str, Value $idx, Value $len, Value $repl, --> Value) {
			my $tostr = $str.Str.clone;

			$tostr.substr-rw($idx.Int, $len.Int) = $repl.Str;

			String.new: $tostr
		}
	);

	method new($name, *@args) {
		my $func = %FUNCS{$name} or die "unknown function '$name'";
		self.bless: :$func, :@args
	}

	method run(--> Value) {
		$!func(|@!args)
	}
}

grammar Syntax {
	rule TOP { <expr> .* }
	rule expr { <.ws> [<literal> | <function>] }

	token ws { [ <[(){}[\]:\s\n]> | '#' \N* \n? ]* }
	token kw-rest { <[A..Z]>* }

	proto token literal { * }
	      token literal:sym«identifier» { <[a..z_]> <[a..z0..9_]>* }
	      token literal:sym«number»     { \d+ }
	      token literal:sym«string»     { '"' .*? '"' | "'" .*? "'" }
	      token literal:sym«null»       { 'N' <.kw-rest> }
	      token literal:sym«boolean»    { (<[TF]>) <.kw-rest> }

	proto rule function { * }

	rule function:«nullary»         { <nullary> }
	proto token nullary             { * }
	      token nullary:sym«prompt» { 'P' <.kw-rest> }
	      token nullary:sym«random» { 'R' <.kw-rest> }

	rule function:«unary»         { <unary> <expr> }
	proto token unary             { * }
	      token unary:sym«eval»   { 'E' <.kw-rest> }
	      token unary:sym«block»  { 'B' <.kw-rest> }
	      token unary:sym«call»   { 'C' <.kw-rest> }
	      token unary:sym«system» { '`' }
	      token unary:sym«quit»   { 'Q' <.kw-rest> }
	      token unary:sym«not»    { '!' }
	      token unary:sym«length» { 'L' <.kw-rest> }
	      token unary:sym«output» { 'O' <.kw-rest> }

	rule function:«binary»        { <binary> <expr> <expr> }
	proto token binary            { * }
	      token binary:sym«+»     { <sym> }
	      token binary:sym«-»     { <sym> }
	      token binary:sym«*»     { <sym> }
	      token binary:sym«/»     { <sym> }
	      token binary:sym«%»     { <sym> }
	      token binary:sym«^»     { <sym> }
	      token binary:sym«?»     { <sym> }
	      token binary:sym«&»     { <sym> }
	      token binary:sym«|»     { <sym> }
	      token binary:sym«;»     { <sym> }
	      token binary:sym«=»     { <sym> }
	      token binary:sym«<»     { <sym> }
	      token binary:sym«>»     { <sym> }
	      token binary:sym«while» { 'W' <.kw-rest> }

	rule function:«ternary»      { <ternary> <expr> <expr> <expr> }
	proto token ternary          { * }
	      token ternary:sym«if»  { 'I' <.kw-rest> }
	      token ternary:sym«get» { 'G' <.kw-rest> }

	rule function:«quatenary»      { <quatenary> <expr> <expr> <expr> <expr> }
	proto token quatenary          { * }
	      token quatenary:sym«set» { 'S' <.kw-rest> }
#
#	method missing-arg($token, $index) {
#		my $lineno = self.target.substr(0, self.pos).lines.elems;
#		die "[Line '$lineno'] Missing argument number $index for token '$token'";
#	}

#	method expr-index($index) {
#		/ <expr> / # || <missing-arg("?", $index)> /
#		; /<expr>/
#	}
}

class SyntaxAction {
	method TOP($/)  { make $<expr>.made; }
	method expr($/) { make $/.values[0].made; }

	method literal:sym«identifier»($/) { make Knight::Identifier.new: $/.Str }
	method literal:sym«number»($/)     { make Knight::Number.new: $/.Int }
	method literal:sym«string»($/)     { make Knight::String.new: $/.Str.substr(1, *-1) }
	method literal:sym«null»($/)       { make Knight::Null.new }
	method literal:sym«boolean»($/)    { make Knight::Boolean.new: $/[0] eq 'T' }

	method function:«nullary»($/)  { make Knight::Function.new: $<nullary>.made }
	method nullary:sym«prompt»($/) { make 'P' }
	method nullary:sym«random»($/) { make 'R' }

	method function:«unary»($/)  { make Knight::Function.new: $<unary>.made, |[$<expr>]».made }
	method unary:sym«eval»($/)   { make 'E' }
	method unary:sym«block»($/)  { make 'B' }
	method unary:sym«call»($/)   { make 'C' }
	method unary:sym«system»($/) { make '`' }
	method unary:sym«quit»($/)   { make 'Q' }
	method unary:sym«not»($/)    { make '!' }
	method unary:sym«length»($/) { make 'L' }
	method unary:sym«output»($/) { make 'O' }

	method function:«binary»($/) { make Knight::Function.new: $<binary>.made, |$<expr>».made }
	method binary:sym«+»($/)     { make $<sym>.Str }
	method binary:sym«-»($/)     { make $<sym>.Str }
	method binary:sym«*»($/)     { make $<sym>.Str }
	method binary:sym«/»($/)     { make $<sym>.Str }
	method binary:sym«%»($/)     { make $<sym>.Str }
	method binary:sym«^»($/)     { make $<sym>.Str }
	method binary:sym«?»($/)     { make $<sym>.Str }
	method binary:sym«&»($/)     { make $<sym>.Str }
	method binary:sym«|»($/)     { make $<sym>.Str }
	method binary:sym«;»($/)     { make $<sym>.Str }
	method binary:sym«=»($/)     { make $<sym>.Str }
	method binary:sym«<»($/)     { make $<sym>.Str }
	method binary:sym«>»($/)     { make $<sym>.Str }
	method binary:sym«while»($/) { make 'W' }

	method function:«ternary»($/) { make Knight::Function.new: $<ternary>.made, |$<expr>».made }
	method ternary:sym«if»($/)    { make 'I' }
	method ternary:sym«get»($/)   { make 'G' }

	method function:«quatenary»($/) { make Knight::Function.new: $<quatenary>.made, |$<expr>».made }
	method quatenary:sym«set»($/)   { make 'S' }
}

sub run($input --> Value) {
	my $stream = $input.Str;	
	my $func = Syntax.parse($stream, actions => SyntaxAction).made;

	die 'Syntax error encountered somewhere.' unless $func;
	$func.run;
}

#run "; = + 'a' 'b' 2 : OUTPUT ab";

run qqx<cat ../knight.kn>.Str;
=begin comment
my $stream = q:to/EOS/;
	; = fizzbuzz BLOCK
		; = n 0
		; = max + 1 max
		: WHILE < (= n + 1 n) max
		:	OUTPUT
		:		IF ! (% n 15)
		:			"FizzBuzz"
		: 	IF ! (% n 5)
		:			"Fizz"
		:		IF ! (% n 3)
		:			"Buzz"
		:			n
	; = max 100
	: CALL fizzbuzz
#	; O 9
#	: Q 0
#	; = x BLOCK 3
#	; OUTPUT CALL x
#	; Q 0
#	; = a 0
#	; = max + 1 max
#	; OUTPUT a
#	: OUTPUT + "A" 3
EOS
=end comment

#my $func = Syntax.parse($stream, actions => SyntaxAction).made;
#
#$func.run;
##say $func;

=finish
class Syntax-Exec {
	method TOP ($/){ make $/<expr>; }
	method expr ($/) {
		given $/ {
			return (say 1, make $<literal>) when $<literal>;
			return (say 2, make $<function>) when $<function>;
			die "unknown expr '$/'";
		}
	}

#	method expr ($/) { make $/; } # our $x; print "{$x++} $/\n"; }
	method literal:sym«identifier» ($/) { make Variable.new("$/"); }
	method literal:sym«number» ($/) { make Number.new(0+$/); }
	method literal:sym«string» ($/) { make String.new("$/".substr(1, *-1)); }
	method literal:sym«null» ($/) { make Null; }
	method literal:sym«boolean» ($/) { make Boolean.new($/[0] eq 'T') }
	method nullary($/) { make Function.new: $/[0] }
	method unary($/) { make Function.new: $/[0], make $/[1] }
	#method binary:sym«+»($/) { exit; }
	method function($/) {
		#make Function.new: $/<binary><sym>
		#say $/.gist;
		#say $/[0].map: *.make;
		#exit;
	}
	#method binary:sym<+>($/) {
		#say $/.gist;
		#say 4;
		#exit;
		##<expr>;
		#make Function._new($/[0], $/[1].make, $/[2].make)
	#}
	#method binary($/) { exit; make Function._new: $/[0], make $/[1], make $/[2] }
}

EVAL "a";
exit;
my $source = q<+ 1 a>;
#my $source = q|+ 1 3|;

my $match = Knight::Syntax.parse($source);#, actions => Knight::Syntax-Exec);
#say $match<expr><literal>[0];
#say $match<function>.made;
#say $match<expr>.made;
say $match;
#say $match<expr><literal>.made.run;



=finish

say Function.new('S', String.new("abcdefg"), Number.new(1), Number.new(1), String.new("lol")).run;

=finish
#say func(Stri, 4;
say String.new("a").assign(Number.new(3));
say Identifier.new("a").run;


#say 34.^methods.map(*.gist).grep(/<[A..Z]>/);
#say 34.BUILDALL: 3, 9;
#say 34.ACCEPTS ;
#say &sub(String.new("34\\"));
#.new("O", [Number.new(34)]).run

=finish
sub run(Str $input) {
	# my $match = Knight::Syntax.parse($source, actions => Knight::Syntax-Exec);
	die "todo";
}

