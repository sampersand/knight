package Ast::Function;
use strict;
use warnings;

use lib '..';

use overload '""' => sub { $_[0]->value };

sub new($$) {
	my $class = shift;
	bless { value => shift }, $class;
}

1;
