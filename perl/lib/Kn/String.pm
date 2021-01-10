package Kn::String;
use strict;
use warnings;

use parent 'Kn::Value';

use overload
	'0+' => sub { no warnings; int shift->{value}; };

sub add {
	Kn::String->new(shift->run() . shift->run());
}

# Duplicates `self` by `rhs` times
sub mul {
	Kn::String->new(shift->run() x int(shift->run()));
}

# Compares the two strings lexicographically.
sub cmp {
	"".shift->run() cmp "".shift->run();
}

sub eql {
	my ($lhs, $rhs) = @_;

	ref($lhs) eq ref($rhs) && $lhs->{value} eq $rhs->{value}
}


sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\A(["'])((?:(?!\1).)*)(\1|\z)//s or return;
	die "missing closing quote" unless $3;
	$class->new($2)
}

1;

