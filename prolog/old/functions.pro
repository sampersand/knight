kn_random(R) :-
	randomize,
	fd_max_integer(Max),
	random(0, Max, R).
kn_prompt(O) :- halt(1). % TODO

kn_eval(O, I) :- halt(1).
kn_block(N, N).
kn_call(O, I) :- kn_run(I, T), kn_run(T, O).
kn_system(O, I) :- kn_string
%hm
string(X).

/*
= (
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
		*/
+(string(bar)).
%+(foo(X, Y)).

main :- X = string(bar), foo(X).%kn_random(T), write(T).
