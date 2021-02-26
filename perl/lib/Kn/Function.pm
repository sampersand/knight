package Kn::Function;
use strict;
use warnings;
no warnings 'recursion';

use Kn::Environment;

my %funcs;

# Fetches the function associated with the given value, or `undef` if no such
# function exists.
sub get {
	$funcs{$_[1]};
}

# Registers a new function with the given name, arity, and body.
sub register {
	my ($class, $name, $argc, $block) = @_;

	$name = substr($name, 0, 1) or die 'a name is required';

	$funcs{$name} = bless { argc => $argc, block => $block }, $class;
}

sub arity {
	shift->{argc};
}

sub run {
	my ($self, @args) = @_;

	$self->{block}->(@args);
}

# Gets a single line from stdin.
__PACKAGE__->register('P', 0, sub {
	Kn::String->new(scalar <STDIN>);
});

# Gets a random number from `0 - 0xffff_ffff`.
__PACKAGE__->register('R', 0, sub {
	Kn::Number->new(int rand 0xffff_ffff);
});

# Evaluates a string as Knight code.
__PACKAGE__->register('E', 1, sub {
	Kn->run("$_[0]");
});

# Simply returns its argument, unevaluated.
__PACKAGE__->register('B', 1, sub {
	shift;
});

# Runs a previously unevaluated block of code.
__PACKAGE__->register('C', 1, sub {
	shift->run()->run();
});

# Executes the argument as a shell command, then returns the entire stdout.
__PACKAGE__->register('`', 1, sub {
	Kn::String->new(join '', `$_[0]`);
});

# Quits with the given exit code.
__PACKAGE__->register('Q', 1, sub {
	exit shift;
});

# Returns the logical negation of the argument.
__PACKAGE__->register('!', 1, sub {
	Kn::Boolean->new(!shift);
});

# Gets the length of the given argument as a string.
__PACKAGE__->register('L', 1, sub {
	Kn::Number->new(length shift);
});

# Dumps a value's representation, then returns it.
__PACKAGE__->register('D', 1, sub {
	my $val = shift->run();

	print $val->dump();
	return $val;
});

# Outputs the given argument, which it then returns. If the argument ends with
# a `\`, it's removed and no trailing newline is printed. Otherwise, a newline
# is added to the end of the string.
__PACKAGE__->register('O', 1, sub {
	my $val = shift->run();
	my $str = "$val";

	print(substr($str, -1) eq '\\' ? substr($str, 0, -1) : "$str\n");
	$val
});

# Adds two values together.
__PACKAGE__->register('+', 2, sub {
	shift->run()->add(shift->run());
});

# Subtracts the second vaule from the first
__PACKAGE__->register('-', 2, sub {
	shift->run()->sub(shift->run());
});

# Multiplies two values together.
__PACKAGE__->register('*', 2, sub {
	shift->run()->mul(shift->run());
});

# Divides the first number by the second.
__PACKAGE__->register('/', 2, sub {
	shift->run()->div(shift->run());
});

# Gets the modulo of the first number by the second.
__PACKAGE__->register('%', 2, sub {
	shift->run()->mod(shift->run());
});

# Raises the first argument to the power of the second.
__PACKAGE__->register('^', 2, sub {
	shift->run()->pow(shift->run());
});

# Checks to see if two values are equal.
__PACKAGE__->register('?', 2, sub {
	Kn::Boolean->new(shift->run()->eql(shift->run()));
});

# Checks to see if the first value is less than the second
__PACKAGE__->register('<', 2, sub {
	Kn::Boolean->new(shift->run()->lth(shift->run()));
});

# Checks to see if the first value is greater than the second
__PACKAGE__->register('>', 2, sub {
	Kn::Boolean->new(shift->run()->gth(shift->run()));
});

# Simply executes the first argument, then executes and returns second.
__PACKAGE__->register(';', 2, sub {
	shift->run();
	shift->run();
});

# Assigns the second argument to the first. If the first argument is not
# an identifier, it is first evaluated and then converted to a string.
__PACKAGE__->register('=', 2, sub {
	my $var = shift;
	$var = $var->isa('Kn::Identifier') ? $$var : "$var";
	Kn::Environment->set($var, shift->run());
});

# Executes the second argument while the first one evaluates to true. Returns
# the last result of running the body, or `Null` if the body never ran.
__PACKAGE__->register('W', 2, sub {
	my ($cond, $body) = @_;
	my $ret;

	$ret = $body->run() while $cond;
	defined($ret) ? $ret : Kn::Null->new();
});

# If the first argument is falsey, it's returned. Otherwise, the second argument
# is executed and returned.
__PACKAGE__->register('&', 2, sub {
	my $lhs = shift->run();
	$lhs ? shift->run() : $lhs
});

# If the first argument is truthy, it's returned. Otherwise, the second argument
# is executed and returned.
__PACKAGE__->register('|', 2, sub {
	my $lhs = shift->run();
	$lhs ? $lhs : shift->run()
});

# If the first argument is true, evaluates and runs the second argument.
# otherwise, evaluates and runs the third.
__PACKAGE__->register('I', 3, sub {
	$_[$_[0] ? 1 : 2]->run()
});

# Gets a substring of the first argument, starting at the second argument,
# with a length of the third argument.
__PACKAGE__->register('G', 3, sub {
	no warnings;

	Kn::String->new(substr($_[0], $_[1], $_[2]));
});

# Returns a new string where the first argument's substring starting at the 
# second argument with length the third argument is replaced with the fourth.
__PACKAGE__->register('S', 4, sub {
	my $str = "$_[0]";
	my $idx = int $_[1];
	my $len = int $_[2];
	my $repl = "$_[3]";

	no warnings;
	Kn::String->new(substr($str, 0, $idx) . $repl . substr($str, $idx + $len));
});

1;
