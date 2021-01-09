package Kn::Identifier;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s/\A[a-z_][a-z0-9_]*//p and $class->new(${^MATCH});
}

sub run($) {
	our %ENVIRONMENT;
	$ENVIRONMENT{shift->{value}};
}

1;
