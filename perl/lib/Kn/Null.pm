package Kn::Null;
use strict;
use warnings;

use parent 'Kn::Value';

use overload
	'0+' => sub { 0 },
	'""' => sub { 'null' };

my $null;
# Unlike every other value, `Null`s do not take arguments.
sub new {
	bless \$null, shift
}

# Parses a null from the stream, which must start with `N`, and then may include
# any number of upper case letters.
#
# Returns `undef` if the stream doesn't start with null.
sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\AN[A-Z]*//p or return;

	$class->new();
}

# You are not allowed to compare null.
sub cmp {
	die "Comparing by null is not allowed.";
}


# Dumps the class's info. Used for debugging.
sub dump {
	'Null()';
}

1;
