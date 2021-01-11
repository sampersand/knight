package Kn::Environment;
use strict;
use warnings;

# The environment in which all variables are stored.
my %ENVIRONMENT;

# Fetches a single variable from the environment. 
sub get {
	$ENVIRONMENT{"$_[1]"};
}

# Assigns a single variable to the environment
sub set {
	$ENVIRONMENT{"$_[1]"} = $_[2];
}

1;
