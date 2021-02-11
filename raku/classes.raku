unit module Knight;

class Identifier { ... }

role Value {
	method assign(Value $value, --> Value) {
		Identifier.new($.Str).assign($value);
	}

	method lth(Value $rhs, --> Bool) {
		$.cmp($rhs) === Less
	}

	method gth(Value $rhs, --> Bool) {
		$.cmp($rhs) === More
	}

	multi method eql(Value $, --> False) is pure {}
}

role TypedValue[::T, $cmp, $eql] does Value {
	has T $!value is built;

	method new(T $value) {
		self.bless :$value;
	}

	method BUILD(T :$value) is pure {
		$!value = $value;
	}

	method cmp(Value $rhs, --> Order) {
		$cmp($!value, T($rhs))
	}

	multi method eql(::?CLASS $rhs, --> Bool) {
		$eql($!value, T($rhs))
	}

	method Str(--> Str) is pure {
		$!value.Str
	}

	method Bool(--> Bool) is pure {
		$!value.Bool
	}

	method Int(--> Int) is pure {
		$!value.Int
	}

	method run(--> Value) is pure {
		self
	}
}


class Boolean does TypedValue[Bool, * <=> *, * == *] {
	method Str(--> Str) is pure {
		$!value ?? "true" !! "false"
	}
}

class Null {
	method new() is pure { self.bless }

	method Str(--> "null") is pure { }
	method Bool(--> False) is pure { }
	method Int(--> 0) is pure { }

	method cmp(Value $) is pure { die "Cannot compare Null." }

	multi method eql(Null $, --> True) is pure { }
}

class String does TypedValue[Str, * cmp *, * eq *] {
	method Int(--> Int) is pure {
		$!value ~~ /^ <[\d]>* /;
		$<>.Int
	}

	method add(Value $rhs, --> String) {
		String.new: $!value ~ $rhs.Str
	}

	method mul(Value $rhs, --> String) {
		String.new: $!value x $rhs.Str
	}
}

class Number does TypedValue[Int, * <=> *, * == *] {
	method add(Value $rhs, --> Number) {
		Number.new: $!value + $rhs.Int
	}

	method sub(Value $rhs, --> Number) {
		Number.new: $!value - $rhs.Int
	}

	method mul(Value $rhs, --> Number) {
		Number.new: $!value * $rhs.Int
	}

	method div(Value $rhs, --> Number) {
		Number.new: $!value div ($rhs.Int or die "Cannot divide by zero!");
	}

	method mod(Value $rhs, --> Number) {
		Number.new: $!value mod ($rhs.Int or die "Cannot modulo by zero!")
	}

	method pow(Value $rhs, --> Number) {
		Number.new: $!value ** $rhs.Int
	}
}

class Identifier is Value {
	has Str $!ident is built;

	my %ENV;

	method new(Str $ident) {
		self.bless :$ident
	}

	method run(--> Value) {
		%ENV{$!ident} or die "unknown variable '$!ident'";
	}

	method assign(Value $value, --> Value) {
		%ENV{$!ident} = $value;
		$value
	}
}

class Function is Value {
	has $!func is built;
	has @!args is built;

	our %FUNCS = (
		'P' => sub (--> Value) {
			String.new: get;
		}

		'R' => sub (--> Value) {
			Number.new: (^0xffff_ffff).pick;
		}

		'E' => sub (Value $str, --> Value) {
			Knight::run $str.Str;
		}

		'B' => sub (Value $block, --> Value) {
			$block;
		}

		'C' => sub (Value $block, --> Value) {
			$block.run.run;
		}

		'`' => sub (Value $str, --> String) {
			String.new: qx<$str>;
		}

		'Q' => sub (Value $code) {
			exit $code;
		}

		'!' => sub (Value $bool, --> Boolean) {
			Boolean.new: !$bool;
		}

		'L' => sub (Value $str, --> Number) {
			Number.new: $str.Str.chars;
		}

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

		'+' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.add: $rhs.run;
		}

		'-' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.sub: $rhs.run;
		}

		'*' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.mul: $rhs.run;
		}

		'/' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.div: $rhs.run;
		}

		'%' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.mod: $rhs.run;
		}

		'^' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run.pow: $rhs.run;
		}

		'<' => sub (Value $lhs, Value $rhs, --> Value) {
			Boolean.new: $lhs.run.lth: $rhs.run;
		}

		'>' => sub (Value $lhs, Value $rhs, --> Value) {
			Boolean.new: $lhs.run.gth: $rhs.run;
		}

		'?' => sub (Value $lhs, Value $rhs, --> Value) {
			Boolean.new: $lhs.run.eql: $rhs.run;
		}

		'&' => sub (Value $lhs, Value $rhs, --> Value) {
			($lhs = $lhs.run) ?? $rhs.run !! $lhs;
		}

		'|' => sub (Value $lhs, Value $rhs, --> Value) {
			($lhs = $lhs.run) ?? $lhs !! $rhs.run;
		}

		';' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.run;
			$rhs.run;
		}

		'=' => sub (Value $lhs, Value $rhs, --> Value) {
			$lhs.assign: $rhs;
		}

		'W' => sub (Value $cond, Value $body, --> Value) {
			my $ret = Null.new;

			$ret = $body.run while $cond.run;

			$ret;
		}

		'I' => sub (Value $cond, Value $iftrue, Value $iffalse, --> Value) {
			$cond.run ?? $iftrue.run !! $iffalse.run
		}

		'G' => sub (Value $str, Value $idx, Value $len, --> Value) {
			String.new: $str.Str.substr($idx.Int, $len.Int)
		}

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
		$!func(|@!args);
	}
}
