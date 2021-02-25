package Kn::Value;
use strict;
use warnings;

use overload
	'""' => sub { ${shift->run()} },
	'bool' => sub { ${shift->run()} }, # this is redundant really.
	'0+' => sub { ${shift->run()} };

# Creates a new `Value` (or whatever subclasses it) by simply getting a
# reference to the second argument.
sub new {
	my ($class, $data) = @_;
	bless \$data, $class;
}

# Adds two Values together by converting them both to numbers.
sub add {
	Kn::Number->new(int(shift) + int(shift));
}

# Subtract two Values by converting them both to numbers.
sub sub {
	Kn::Number->new(int(shift) - int(shift));
}

# Multiply two Values by converting them both to numbers.
sub mul {
	Kn::Number->new(int(shift) * int(shift));
}

# Divides the first number by the second, `die`ing if the second's zero.
sub div {
	my $lhs = int shift;
	my $rhs = int shift;

	die "cant divide by zero" unless $rhs;

	Kn::Number->new(int($lhs / $rhs));
}

# Modulo the first number by the second, `die`ing if the second's zero.
sub mod {
	my $lhs = int shift;
	my $rhs = int shift;

	die "cant modulo by zero" unless $rhs;

	Kn::Number->new($lhs % $rhs);
}

# Raises the first number to the power of the second.
sub pow {
	Kn::Number->new(int(int(shift) ** int(shift)));
}

# Converts both values to integers and compares them.
sub cmp {
	int(shift) <=> int(shift);
}

# Checks to see if the first argument is less than the second.
sub lth {
	shift->cmp(shift) < 0
}

# Checks to see if the first argument is greater than the second.
sub gth {
	shift->cmp(shift) > 0
}

# Checks to see if the first argument is equal to the second by comparing their
# types and inner data.
sub eql {
	my ($lhs, $rhs) = @_;

	ref($lhs) eq ref($rhs) && $$lhs == $$rhs
}

# Running a normal value simply returns it.
sub run {
	shift;
}

use Kn::Number;
use Kn::Boolean;
use Kn::Identifier;
use Kn::Null;
use Kn::String;
use Kn::Ast;

# Parses a Value from the stream, stripping leading whitespace and comments.
# If the first character of the stream is invalid, the program `die`s.
sub parse {
	my $stream = $_[1];
	my $ret;

	while ($$stream =~ s/\A(?:[\s()\[\]{}:]+|#[^\n]*)//) {
		# do nothing, we're stripping the steram.
	}

	for (qw(Kn::Number Kn::Identifier Kn::Null Kn::String Kn::Boolean Kn::Ast)){
		$ret = $_->parse($stream);
		return $ret if defined $ret;
	}

	die "unknown token start '" . substr($$stream, 0, 1) . "'";
}

1;
