package Kn::Value;
use strict;
use warnings;

use overload
	'""' => sub { shift->run()->{value} },
	'bool' => sub { shift->run()->{value} },
	'0+' => sub { shift->run()->{value} };

# Creates a new `Value` (or whatever its subclass is) by simply accepting
# whatever value's given
sub new {
	my $class = shift;
	bless { value => shift }, $class;
}

sub add {
	Kn::Number->new(int(shift) + int(shift));
}

sub sub {
	Kn::Number->new(int(shift) - int(shift));
}

sub mul {
	Kn::Number->new(int(shift) * int(shift));
}

sub div {
	Kn::Number->new(int(shift) / int(shift));
}

sub mod {
	Kn::Number->new(int(shift) % int(shift));
}

sub pow {
	Kn::Number->new(int(shift) ** int(shift));
}

sub cmp {
	int(shift) <=> int(shift);
}

sub lth {
	shift->cmp(shift) < 0
}

sub gth {
	shift->cmp(shift) > 0
}

sub eql {
	my ($lhs, $rhs) = @_;

	ref($lhs) eq ref($rhs) && int($lhs->{value}) == int($rhs->{value})
}

sub run {
	shift;
}

use Kn::Number;
use Kn::Boolean;
use Kn::Identifier;
use Kn::Null;
use Kn::String;
use Kn::Function;

sub parse {
	my $stream = $_[1];
	my $ret;

	while ($$stream =~ s/\A(?:[\s()\[\]{}:]+|#[^\n]*)//) {
		# do nothing, we're stripping the steram.
	}

	for (qw(Kn::Number Kn::Identifier Kn::Null Kn::String Kn::Boolean Kn::Function)) {
		$ret = $_->parse($stream);
		return $ret if defined $ret;
	}

	die "unknown token start '" . substr($$stream, 0, 1) . "'";
}

1;
