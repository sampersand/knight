package Kn::Boolean;
use strict;
use warnings;

use parent 'Kn::Value';

use overload
	'""' => sub { shift() ? 'true' : 'false' };

sub new {
	my ($class, $val) = @_;
	bless { value => ($val ? 1 : 0) }, $class;
}

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s/\A([TF])[A-Z]*//p or return;

	$class->new($1 eq 'T');
}

1;
