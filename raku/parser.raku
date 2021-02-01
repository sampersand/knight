#say $*RAKU.version;

module Knight {
	our %env = ();

	class Value {
		method _new(*@args) { my $x = self.new; $x._set: |@args; $x }

#		method value() { die "whoops, override me"; }
		method Str(--> Str) { $.value.Str }
		method Bool(--> Bool) { $.value.Bool }
		method Numeric(--> Numeric) { $.value.Numeric }
	}

	class Null is Value {
		method Str(--> Str) { "null" }
		method Bool(--> Bool) { False }
		method Numeric(--> Numeric) { 0 }
	}

	class Boolean is Value {
		has Bool $.value;

		method _set(Bool $val) { $!value = $val }
		method Stringy(--> Str) { $.value ?? "true" !! "false";}
	}

	class String is Value {
		has Str $.value;

		method _set(Str $val) { $!value = $val }
		method Numeric(--> Numeric) {
			$.value ~~ /^ <[\d]>* /;
			$<>.Numeric;
		}
	}

	class Number is Value {
		has Int $.value;

		method _set(Int $val) { $!value = $val }
	}

	class Variable is Value {
		has Str $!ident;

		method _set($n) { $!ident = $n; }

		method value(--> Value) {
			%env{$!ident} or die "unknown variable '$.ident'";
		}

		method assign(Value $val --> Value) {
			%env{$!ident} = $val;
			$val
		}
	}

	class Function is Value {
		has $!func;
		has @!args;

		constant %FUNCS = (
			P => sub { String._new: get },
			R => sub { Number._new: (^0xffff_ffff).pick },

			E => sub ($str) { Knight::run $str.to_str },
			B => sub ($block) { $block },
			C => sub ($block) { $block.run.run },
			'`' => sub ($str) { my $s = $str.to_str; String._new: qx<$s> },
			Q => sub ($code) { exit $code.to_int },
			'!' => sub ($bool) { Boolean._new: !$bool.to_bool },
			L => sub ($str) { Number._new: $str.to_str.chars },
			O => sub ($str) { die "output" },

			'+' => sub ($lhs, $rhs) { $lhs.run._add($rhs.run) },
			'-' => sub ($lhs, $rhs) { $lhs.run._sub($rhs.run) },
			'*' => sub ($lhs, $rhs) { $lhs.run._mul($rhs.run) },
			'/' => sub ($lhs, $rhs) { $lhs.run._div($rhs.run) },
			'%' => sub ($lhs, $rhs) { $lhs.run._mod($rhs.run) },
			'^' => sub ($lhs, $rhs) { $lhs.run._pow($rhs.run) },
			'<' => sub ($lhs, $rhs) { $lhs.run._lth($rhs.run) },
			'>' => sub ($lhs, $rhs) { $lhs.run._gth($rhs.run) },
			'?' => sub ($lhs, $rhs) { $lhs.run._eql($rhs.run) },
			'&' => sub ($lhs, $rhs) { $lhs.run._eql($rhs.run) },
			'&' => sub ($lhs, $rhs) { $lhs=$lhs.run; $lhs.to_bool ?? $rhs.run !! $lhs },
			'|' => sub ($lhs, $rhs) { $lhs=$lhs.run; $lhs.to_bool ?? $lhs !! $rhs.run },
			';' => sub ($lhs, $rhs) { $lhs.run; $rhs.run },
			'=' => sub ($lhs, $rhs) { die "eql" },
			W => sub ($cond, $body) {
				my $ret = Null.new;
				$ret = $body.run while $cond.run.to_bool;
				$ret;
			},

			I => sub ($cond, $iftrue, $iffalse) { $cond.run.to_bool ?? $iftrue.run !! $iffalse.run },
			G => sub ($str, $idx, $len) { die "todo: get"},
			S => sub ($str, $idx, $len, $repl) { die "todo: set"},
		);

		method _set($name, *@args) {
			$!func = %FUNCS{$name};
			@!args = @args;
			self;
		}

		method value(--> Value) { $!func(|@!args); }
	}

	sub run($thing) {
		# my $match = Knight::Syntax.parse($source, actions => Knight::Syntax-Exec);
		die "todo";
	}

	grammar Syntax {
		rule TOP { <expr> .* }
		rule expr {
			<literal> | <function> | <whitespace>+ <expr>
		}
		rule whitespace { <[(){}[\]:\h]>+ | \h* '#' .*? $$ }

		proto token literal { * }
		token literal:sym«variable» { <[a..z_]> <[a..z0..9_]>* }
		token literal:sym«number» { \d+ }
		token literal:sym«string» { '"' .*? '"' | "'" .*? "'" }
		token literal:sym«null» { ('N') <function-keyword-rest> }
		token literal:sym«boolean» { (<[TF]>) <function-keyword-rest> }
		token function-keyword-rest { <[A..Z]>* }

	#	token function { function-keyword | function-symbol }
		token function {
			  <nullary>
			| <unary> (<expr>)
			| <binary> (<expr> <expr>)
			| <ternary> (<expr> <expr> <expr>)
			| <quatenary> (<expr> <expr> <expr> <expr>)
		}

		proto token nullary { * }
		token nullary:sym«prompt» { 'P' <function-keyword-rest> }
		token nullary:sym«random» { 'R' <function-keyword-rest> }

		proto token unary { * }
		token unary:sym«eval» { 'E' <function-keyword-rest> }
		token unary:sym«block» { 'B' <function-keyword-rest> }
		token unary:sym«call» { 'C' <function-keyword-rest> }
		token unary:sym«system» { '`' }
		token unary:sym«quit» { 'Q' <function-keyword-rest> }
		token unary:sym«not» { '!' }
		token unary:sym«length» { 'L' <function-keyword-rest> }
		token unary:sym«output» { 'O' <function-keyword-rest> }

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
		token binary:sym«while» { 'W' <function-keyword-rest> }

		proto token ternary { * }
		token ternary:sym«if» { 'I' <function-keyword-rest> }
		token ternary:sym«get» { 'G' <function-keyword-rest> }

		proto token quatenary { * }
		token quatenary:sym«set» { 'S' <function-keyword-rest> }
	}


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
		method literal:sym«variable» ($/) { make Variable._new("$/"); }
		method literal:sym«number» ($/) { make Number._new(0+$/); }
		method literal:sym«string» ($/) { make String._new("$/".substr(1, *-1)); }
		method literal:sym«null» ($/) { make Null; }
		method literal:sym«boolean» ($/) { make Boolean._new($/[0] eq 'T') }
		method nullary($/) { make Function._new: $/[0] }
		method unary($/) { make Function._new: $/[0], make $/[1] }
		#method binary:sym«+»($/) { exit; }
		method function($/) {
			#make Function._new: $/<binary><sym>
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
}


my $source = q<+ 1 a>;
#my $source = q|+ 1 3|;
my $match = Knight::Syntax.parse($source, actions => Knight::Syntax-Exec);
#say $match<expr><literal>[0];
#say $match<function>.made;
#say $match<expr>.made;
say $match;
#say $match<expr><literal>.made.run;

