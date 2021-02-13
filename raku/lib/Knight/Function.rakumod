unit class Function does NonIdempotent is export;

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
