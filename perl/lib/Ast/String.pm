package Ast::String;
use strict;
use warnings;

use lib '..';
use parent 'Ast::Value';

use overload
	'0+' => sub { shift->{value} },
	'""' => \&interpolate;

# Adds the RHS to this value.
sub add {
	Ast::String->new(shift->{value} . shift->to_string()->{value});
}

# Duplicates `$self` by `rhs` times
sub mul {
	Ast::String->new(shift->{value} x shift->to_number()->{value});
}


# Performs a single printf substitution.
sub mod {
	Ast::String->new(sprintf shift->{value}, shift->{value});
}

# Compares the two strings lexicographically.
sub cmp {
	Ast::Number->new(shift->{value} cmp shift->to_string()->{value});
}

# Simply returns the original string.
sub to_string {
	shift;
}

# Converts this to a Boolean by checking to see if we're an empty string or not.
sub to_bool {
	Ast::Boolean->new(length shift->{value})
}

sub to_number {
	Ast::Number->new(shift->{value});
}

sub interpolate {
	'"' . quotemeta(shift->{value}) . '"';
}

sub parse($$) {
	my ($class, $stream) = @_;

	return unless $$stream =~ s/\A(["'])([^\1]*)(\1)//p;
	die "missing closing quote" unless $3;
	$class->new($2)
}

sub run($$) {
	shift;
}

1;

