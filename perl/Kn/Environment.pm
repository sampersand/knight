package Kn::Environment;
use strict;
use warnings;

use lib '..';

# The environment in which all variables are stored.
my %ENVIRONMENT;

# Fetches a singnle 
sub get($$) {
	$ENVIRONMENT{"$_[1]"}
}

sub set($$$) {
	my ($cls, $var, $val) = @_;

	$ENVIRONMENT{"$var"} = $val;
}

1;
