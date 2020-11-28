package String;

use Value;
use base qw(Value);

use strict;
use warnings;
use overload
	'""' => \&interpolate;

sub add() {
	String->new(shift->{value} + shift->to_string()->{value});
}

sub mul() {
	String->new(shift->{value} x shift->to_number()->{value});
}

sub mod() {
	String->new(sprintf shift->{value}, shift->{value});
}

sub cmp() {
	Number->new(shift->{value} cmp shift->to_string()->{value});
}

sub to_bool {
	shift->to_number()->to_bool()
}

sub to_number {
	Number->new(shift->{value});
}

sub to_array {
	Array->new(map &String->new, split('', shift->{value}));
}

sub interpolate {
	'"' . quotemeta(shift->{value}) . '"';
}

1;
