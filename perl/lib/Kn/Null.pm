package Kn::Null;
use strict;
use warnings;

use parent 'Kn::Value';

use overload
	'0+' => sub { 0 },
	'""' => sub { 'null' };

sub new {
	bless {}, shift
}

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\AN[A-Z]*//p or return;

	$class->new();
}

1;
