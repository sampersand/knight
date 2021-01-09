package Kn::Environment;
use strict;
use warnings;

use lib '..';

my %ENVIRONMENT;

sub get($$) {
	$ENVIRONMENT{"" . pop}
}

sub set($$$) {
	my ($cls, $var, $val) = @_;

	$ENVIRONMENT{"$var"} = $val;
}

1;
