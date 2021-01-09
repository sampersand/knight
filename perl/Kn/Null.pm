package Kn::Null;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

use overload
	'0+' => sub { 0 },
	'""' => sub { 'null' };

sub new() {
	bless {}, shift
}

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s/\AN[A-Z]*//p and $class->new();
}

1;
