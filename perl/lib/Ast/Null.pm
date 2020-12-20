package Ast::Null;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

use overload
	'0+' => sub { 0 },
	'""' => sub { 'NULL' };

sub new() {
	bless {}, shift
}

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s/\AN[A-Z]*//p and $class->new();
}

sub run($$) {
	shift;
}

1;
