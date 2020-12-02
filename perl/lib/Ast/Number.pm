package Ast::Number;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

use overload
	'bool' => sub { shift->{value} != 0 },
	'.' => sub { " a " };


sub add() { Ast::Number->new(shift->{value} + shift->to_number()->{value}); }
sub sub() { Ast::Number->new(shift->{value} - shift->to_number()->{value}); }
sub mul() { Ast::Number->new(shift->{value} * shift->to_number()->{value}); }
sub div() { Ast::Number->new(shift->{value} / shift->to_number()->{value}); }
sub mod() { Ast::Number->new(shift->{value} % shift->to_number()->{value}); }
sub pow() { Ast::Number->new(shift->{value} **shift->to_number()->{value}); }

sub band(){ Ast::Number->new(shift->{value} & shift->to_number()->{value}); }
sub bor() { Ast::Number->new(shift->{value} | shift->to_number()->{value}); }
sub bxor(){ Ast::Number->new(shift->{value} ^ shift->to_number()->{value}); }
sub shl() { Ast::Number->new(shift->{value}<<(shift->to_number()->{value}));}
sub shr() { Ast::Number->new(shift->{value} >>shift->to_number()->{value}); }
sub cmp() { Ast::Number->new(shift->{value}<=>shift->to_number()->{value}); }

sub to_number {
	shift;
}

sub to_boolean{
	Ast::Boolean->new(shift->{value} != 0);
}

sub parse($$) {
	my ($class, $stream) = @_;
	$class->new($1) if $$stream =~ s/\A\s*(\d+)//p
}

1;



