package Kn::Identifier;
use strict;
use warnings;

use parent 'Kn::Value';
use Kn::Environment;

# Parse an identifier from the start of the stream, which must start with a
# lower case letter (or `_`), and then may contain any number of digits, lower
# case letters, or `_`s.
#
# Returns `undef` if the stream doesn't start with an identifier.
sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\A[a-z_][a-z0-9_]*//p or return;

	$class->new(${^MATCH});
}

# Run this argument by fetching its value from the environment.
sub run {
	Kn::Environment->get(${shift()});
}

# Dumps the class's info. Used for debugging.
sub dump {
	'Identifier(' . ${shift()} . ')';
}

1;
