use Knight;
use Knight::Value;
use Knight::String;
use Knight::Boolean;
use Knight::Number;
use Knight::Null;
use Knight::NonIdempotent;

#| The function type within Knight.
#|
#| Functions are registered via the `register` function.
unit class Knight::Function does Knight::Value does Knight::NonIdempotent;

#| The name of this class.
has $!name is built;

#| The function associated with this instance.
has #`(Callable) &!func is built;

#| The arguments to pass to the function.
has #`(Array[Knight::Value]) @!args is built;

my %FUNCS;

sub register(Str $name where *.chars == 1, &func) is export {
	%FUNCS{$name} = &func;
}

#| Prompts for a string from stdin.
register 'P', { Knight::String.new: get };

#| Gets a random number.
register 'R', { Knight::Number.new: (^0xffff_ffff).pick };

#| Evaluates the given argument.
register 'E', { Knight::run .Str };

#| Simply returns the block argument.
register 'B', *.self;

#| Calls the given argument twice, evaluating blocks that are given.
register 'C', *.run.run;

#| Runs the shell command, then returns its stdout.
register '`', { Knight::String.new: qqx<$_> };

#| Exits the program with the given code.
register 'Q', &exit;

#| Inverts the given argument.
register '!', { Knight::Boolean.new: .not };

#| Gets the amount of characters for the given argument.
register 'L', { Knight::Number.new: .Str.chars; };

#| Runs a value, dumps the result to stdout, then returns the same result.
register 'D', {
	given (.run) {
		print .gist;
		$_;
	}
}

#| Prints the given value out, returning the original value.
#|
#| If the argument does not end with `\`, a trailing newline will be printed.
#| If it does, the newline is not printed, and the trailing slash is removed.
register 'O', {
	my $result = $^a.run;
	my $to-output = $result.Str;

	if $to-output && $to-output.substr(* - 1) eq '\\' {
		print $to-output.substr(0, * - 1);
	} else {
		say $to-output;
	}

	$result;
};

#| Adds the two operands together.
register '+', { $^a.run.add: $^b.run };

#| Subtracts the second operand from the first.
register '-', { $^a.run.sub: $^b.run };

#| Multiplies the two operands together.
register '*', { $^a.run.mul: $^b.run };

#| Divides the first operand by the second.
register '/', { $^a.run.div: $^b.run };

#| Modulos the first operand by the second.
register '%', { $^a.run.mod: $^b.run };

#| Raises the first operand to the second.
register '^', { $^a.run.pow: $^b.run };

#| Checks to see if the first operand is less than the first.
register '<', { Knight::Boolean.new: $^a.run.lth: $^b.run };

#| Checks to see if the first operand is greater than the first.
register '>', { Knight::Boolean.new: $^a.run.gth: $^b.run };

#| Checks to see if the operands are equivalent.
register '?', { Knight::Boolean.new: $^a.run.eql: $^b.run };

#| Returns the first argument if its falsey, otherwise runs and returns the second.
register '&', { $^a.run and $^b.run };

#| Returns the first argument if its truthy, otherwise runs and returns the second.
register '|', { $^a.run or $^b.run };

#| Assigns the first argument to the second's value.
register '=', { $^a.assign: $^b };

#| Runs the first argument, followed by the second.
register ';', { $^a.run; $^b.run; };

#| Runs the second argument while the first evaluates to true.
#|
#| Returns the last return value of the body, or `Knight::Null` if the body was never run.
register 'W', {
	my $ret = Knight::Null.new;

	$ret = $^b.run while $^a;

	$ret;
};

#| Runs the second argument if the first is truthy. If it's falsey, the third argument is run instead.
register 'I', { $^a ?? $^b.run !! $^c.run };

#| Fetches a susbtring of the first argument.
#|
#| The substring starts at the second argument, and has the length of the third.
register 'G', {
	Knight::String.new: $^a.Str.substr($^b.Int, $^c.Int)
};

#| Returns a new string with the substring replaced.
#|
#| The substring starts at the second argument, has the length of the third, and is replaced by the fourth.
register 'S', {
	my $tostr = $^a.Str.clone;

	$tostr.substr-rw($^b.Int, $^c.Int) = $^d.Str;

	Knight::String.new: $tostr
};

#| Creates a new function with the given name and arguments.
#|
#| If a function with the name `$name` does not exist, then the program will die.
method new(Str $name where *.chars == 1, *@args) {
	die "unknown function '$name'" unless $name ~~ %FUNCS;

	self.bless: :@args, :$name, func => %FUNCS{$name}
}

#| Executes the function, returning the result of its execution.
#|
#| The function will nto execute its arguments before it passes them to the function.
method run(--> Knight::Value) {
	&!func(|@!args)
}

#| Gets an internal representation of the class; used in debugging.
method gist(--> Str) {
	"Function($!name" ~ @!args».gist.map(', ' ~ *).join ~ ")";
}

#| Checks to see if `$rhs` is the exact same object as `self`.
multi method eql(::?CLASS $rhs, --> Bool) {
	self =:= $rhs
}
