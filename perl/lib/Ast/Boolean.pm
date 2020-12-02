package Ast::Boolean;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

# ANDs both operands.
sub band() {
	Ast::Boolean->new(shift->{value} & shift->to_boolean()->{value});
}

sub bor() {
	Ast::Boolean->new(shift->{value} | shift->to_boolean()->{value});
}

sub bxor() {
	Ast::Boolean->new(shift->{value} ^ shift->to_boolean()->{value});
}

sub cmp() {
	Ast::Boolean->new(shift->{value} <=> shift->to_boolean()->{value});
}

sub to_number {
	shift;
}

sub to_boolean {
	Ast::Boolean->new(shift->{value} != 0);
}

sub parse($$) {
	my ($class, $stream) = @_;

	$class->new($1) if $$stream =~ s/\A([TF])[A-Z]*//p;
}

1;
