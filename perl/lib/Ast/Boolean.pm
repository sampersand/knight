package Ast::Boolean;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

use overload
	'0+' => sub { shift->{value} },
	'""' => sub { shift->{value} ? 'TRUE' : 'FALSE' };

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

sub parse($$) {
	my ($class, $stream) = @_;

	 $$stream =~ s/\A([TF])[A-Z]*//p and $class->new($1 eq 'T');
}

sub run($$) {
	shift;
}

1;
