unit module Knight;

# A stub class for Identifier so `Value::assign` can access it
class Identifier { ... }

# All types in Knight are `Value`s.
#
# Unlike Raku, Knight does not have separate operators for integral and string-like types: There's one "add" operator,
# one "multiplication" operator, one "equality" operator, etc. As such, overloading operators with Knight types would
# not be very idiomatic: One would expect `Knight::String.new("12") + Knight::String.new("34")` to return a 
# `Knight::Integer.new(46)`, _not_ `Knight::String.new("1234")`. Thus, I've opted to use methods, such as `add`, `mul`,
# `eql`, etc.
role Value {
	# Assigns a value to `self`.
	#
	# All values (asides from `Identifier`s) are first converted to `Str`s, after which an `Identifier` is created and
	# assigned the given `$value`.
	method assign(Value $value, --> Value) { Identifier.new($.Str).assign: $value }

	# Checks to see if `self` is less than `$rhs`.
	#
	# This method simply checks to see if `cmp` is `Less`.
	method lth(Value $rhs, --> Bool) { $.cmp($rhs) === Less }

	# Checks to see if `self` is greater than `$rhs`.
	#
	# This method simply checks to see if `cmp` is `More`.
	method gth(Value $rhs, --> Bool) { $.cmp($rhs) === More }

	# Executes `self` and returns its result.
	#
	# For literals, such as `Number`s and `String`s, this simply returns `self`. For more complex types, such as
	# `Identifier` and `Function`, this should actually execute the value and return its result.
	method run(--> Value) { ... }

	# Compares `self` to `$rhs`, returning an `Order` determining if it's less than, greater, or equal to the other.
	#
	# This method is only used for the `gth` and `lth` functions; notably not `eql`. As such, it should coerce `$rhs` to
	# the proper type.
	method cmp(Value $rhs, --> Order) { ... }

	# Checks to see if `self` is equal to `$rhs`.
	#
	# As per the Knight specs, `eql` is the only function that does not automatically coerce the `$rhs`. As such, `$rhs`
	# must be the same type as `self`. Implementors should also use `multi`
	multi method eql(Value $rhs, --> False) is pure {}
}

# The "null" type within Knight.
#
# Like null it other langues, `Null` is a unit type, and as such all of its methods are pure.
class Null does Value {
	# `Null`'s string representation is simply `null`.
	method Str(--> 'null') is pure { }

	# `Null` is always falsey.
	method Bool(--> False) is pure { }

	# `Null`s are always zero.
	method Int(--> 0) is pure { }

	# You're not allowed to compare nulls to other values.
	method cmp(Value $) { die 'Cannot compare Null.' }

	# `Null` is always equal to itself.
	multi method eql(Null $, --> True) is pure { }

	# Running `Null` is a no op.
	method run(--> Value) is pure { self }
}

# This role is used for literal values (asides from `Null`) within Knight, as they all have an associated Raku type
# That we're essentially wrapping.
#
# - `::T` type should be the builtin type, such as `Bool` or `Str`.
# - `$cmp` should be a binary function that returns an `Order` describing the first value's relation to the second.
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

role NonLiteral does Value {
	method cmp(Value $rhs, --> Order) { $.run.cmp($rhs) }
	multi method eql(::?CLASS $rhs, --> Bool) { $.run.cmp($rhs) }
	method run(--> Value) { ... }

	method Str(--> Str)   { $.run.Str }
	method Bool(--> Bool) { $.run.Bool }
	method Int(--> Int)   { $.run.Int }
}

class Identifier does NonLiteral {
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

class Function does NonLiteral {
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
		my $ret = $!func(|@!args);
		say 'input', @!args, 'output', $ret.gist;
		die "oop" if $ret.Str eq '_v';
		$ret
	}
}
