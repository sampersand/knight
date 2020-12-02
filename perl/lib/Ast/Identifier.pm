package Ast::Identifier;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

sub parse {
	$_[1] =~ s/\AN[A-Z]*//p and Ast::Null->new();
}

1;
