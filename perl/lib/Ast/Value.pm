package Ast::Value;
use strict;
use warnings;

use overload
	'""' => \&interpolate;

# Creates a new `Value` (or whatever its subclass is) by simply accepting
# whatever value's given
sub new($$) {
	my $class = shift;
	bless { value => shift }, $class;
}

# Converts this type to a String by simply using `value` as a type.
sub to_string {
	Ast::String->new(shift->{value});
}

# All types are truthy by default.
sub to_boolean {
	Ast::Boolean->new(1);
}

# Interpolation simply calls `to_string` and gets its value.
sub interpolate {
	shift->to_string()->{value};
}

# Logical negation of this value.
sub not {
	shift->to_boolean()->not();
}

# Checks to see if two values are equal. This default only works for

sub eql {
	shift->cmp(shift)->{value}->not()
}

1;
