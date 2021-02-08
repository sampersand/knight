unit module Knight;

use MONKEY-SEE-NO-EVAL;

EVAL qx<cat classes.raku>;

grammar Syntax {
	rule TOP { <expr> .* }
	rule expr { <.ws> [<literal> | <function>] }
	token ws { [ <[(){}[\]:\s\n]> | '#' \N* \n? ]* }

	proto token literal { * }
	token literal:sym«variable» { <[a..z_]> <[a..z0..9_]>* }
	token literal:sym«number» { \d+ }
	token literal:sym«string» { '"' .*? '"' | "'" .*? "'" }
	token literal:sym«null» { 'N' <.kw-rest> }
	token literal:sym«boolean» { <[TF]> <.kw-rest> }
	token kw-rest { <[A..Z]>* }

	token function {
		| <nullary>
		| <unary> <expr>
		| <binary> <expr> <expr>
		| <ternary> <expr> <expr> <expr>
		| <quatenary> <expr> <expr> <expr> <expr>
	}

	proto token nullary { * }
	token nullary:sym«prompt» { 'P' <.kw-rest> }
	token nullary:sym«random» { 'R' <.kw-rest> }

	proto token unary { * }
	token unary:sym«eval» { 'E' <.kw-rest> }
	token unary:sym«block» { 'B' <.kw-rest> }
	token unary:sym«call» { 'C' <.kw-rest> }
	token unary:sym«system» { '`' }
	token unary:sym«quit» { 'Q' <.kw-rest> }
	token unary:sym«not» { '!' }
	token unary:sym«length» { 'L' <.kw-rest> }
	token unary:sym«output» { 'O' <.kw-rest> }

	proto token binary { * }
	token binary:sym«+» { <sym> }
	token binary:sym«-» { <sym> }
	token binary:sym«*» { <sym> }
	token binary:sym«/» { <sym> }
	token binary:sym«%» { <sym> }
	token binary:sym«^» { <sym> }
	token binary:sym«?» { <sym> }
	token binary:sym«&» { <sym> }
	token binary:sym«|» { <sym> }
	token binary:sym«;» { <sym> }
	token binary:sym«=» { <sym> }
	token binary:sym«<» { <sym> }
	token binary:sym«>» { <sym> }
	token binary:sym«while» { 'W' <.kw-rest> }

	proto token ternary { * }
	token ternary:sym«if» { 'I' <.kw-rest> }
	token ternary:sym«get» { 'G' <.kw-rest> }

	proto token quatenary { * }
	token quatenary:sym«set» { 'S' <.kw-rest> }
}

say Syntax.parse(q<+ 1 2>);
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
	method literal:sym«variable» ($/) { make Variable.new("$/"); }
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

