package Ast::Null;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';


sub new() {
	bless {}, shift
}

sub to_number {
	Ast::Number->new(0);
}

sub to_boolean {
	shift->to_number()->to_boolean()
}

sub to_string {
	Ast::String->new("null");
}

sub parse {
	$_[1] =~ s/\AN[A-Z]*//p and Ast::Null->new();
}

1;
