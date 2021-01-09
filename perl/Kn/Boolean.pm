package Kn::Boolean;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

use overload
	'""' => sub { shift->{value} ? 'true' : 'false' };

sub new($$) {
	my ($class, $val) = @_;
	bless { value => ($val ? 1 : 0) }, $class;
}

sub parse($) {
	my ($class, $stream) = @_;

	 $$stream =~ s/\A([TF])[A-Z]*//p and $class->new($1 eq 'T');
}

1;
