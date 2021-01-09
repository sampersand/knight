package Kn::String;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

use overload
	'0+' => sub { no warnings; int shift->{value}; };

sub add($$) {
	Kn::String->new(shift . shift);
}

# Duplicates `self` by `rhs` times
sub mul($$) {
	Kn::String->new(shift() x shift);
}

# Compares the two strings lexicographically.
sub cmp($$) {
	"".shift cmp "".shift;
}

sub parse($$) {
	my ($class, $stream) = @_;

	return unless $$stream =~ s/\A(["'])((?:(?!\1).)*)(\1|\z)//p;
	die "missing closing quote" unless $3;
	$class->new($2)
}

1;

