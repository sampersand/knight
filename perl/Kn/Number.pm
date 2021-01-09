package Kn::Number;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s/\A\d+//p or return;

	$class->new(${^MATCH});
}

1;
