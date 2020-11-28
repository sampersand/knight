package Null;

use Value;
use base qw(Value);

use strict;
use warnings;

sub new {
	my $class = shift;
	bless {}, $class;
}

sub to_string {
	String->new("null");
}

sub to_bool {
	Boolean->new(0);
}

1;
