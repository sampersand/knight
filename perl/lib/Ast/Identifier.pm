package Ast::Identifier;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s/\A[a-z_][a-z0-9_]*//p and $class->new(${^MATCH});
}

sub run($$) {
	$_[1]->{$_[0]}
}

1;
