package Kn::Identifier;
use strict;
use warnings;

use parent 'Kn::Value';
use Kn::Environment;

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\A[a-z_][a-z0-9_]*//p or return;

	$class->new(${^MATCH});
}

sub add {
	shift->run()->add(shift->run())
}

sub sub {
	shift->run()->sub(shift->run())
}

sub mul {
	shift->run()->mul(shift->run())
}

sub div {
	shift->run()->div(shift->run())
}

sub mod {
	shift->run()->mod(shift->run())
}

sub pow {
	shift->run()->pow(shift->run())
}

sub cmp {
	shift->run()->cmp(shift->run())
}

sub eql {
	shift->run()->eql(shift->run())
}

sub run {
	Kn::Environment->get(shift->{value});
}

1;
