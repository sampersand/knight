package Types::Value;
use strict;
use warnings;

use overload
	'""' => \&interpolate;

# Creates a new `Value` (or whatever its subclass is) by simply accepting
# whatever value's given
sub new {
	my $class = shift;
	my $self = bless { value => shift }, $class;
	$self->_validate();
	$self;
}

# Validate that the value is correctly constructed.
# By default, all types are.
sub _validate {
	1;
}

# Converts this type to a String by simply using `value` as a type.
sub to_string {
	String->new(shift->{value});
}

# All types are truthy by default.
sub to_bool {
	Boolean->new(1);
}

# Interpolation simply calls `to_string` and gets its value.
sub interpolate {
	shift->to_string()->{value};
}

# Logical negation of this value.
sub not {
	shift->to_bool()->not();
}

# Checks to see if two values are equal. This default only works for

sub eql {
	Boolean->new(shift->cmp(shift)->{value} == 0)
}

1;
