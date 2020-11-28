package Types::Array;
use strict;
use warnings;

use lib '..';
use parent 'Types::Value';

# Create a new Array with the given argument array.
sub new(@) {
	my $class = shift;
	bless { value => \@_ }, $class;
}

# Converts this Array to a String in the format `[arg1, arg2, ...]`.
sub to_string {
	local $" = ', ';
	String->new("[@{shift->{value}}]");
}

# Converts this Array to a Boolean, where empty arrays are false.
sub to_bool {
	shift->length()->to_bool()
}

# Simply returns itself.
sub to_array {
	shift;
}

# Gets the length of this Array.
sub length {
	Number->new($#{shift->{value}});
}

# Replaces all `undef`s with `Null`s
sub cleanup_undef {
	foreach (@{shift->{value}}) {
		$_ = Null->new() unless defined $_;
	}
}

# Inserts an element into the given location of the Array.
sub set_index {
	my ($self, $index, $value) = @_;
	$self->{value}[$index->to_number()->{value}] = $value;
	$self->cleanup_undef();
}

# Sets the value at the given location of the array.
sub insert {
	my ($self, $index, $value) = @_;
	splice @{$self->{value}}, $index->to_number()->{value}, 0, $value;
	$self->cleanup_undef();
}

# Deletes the element at the given index in the Array.
sub delete {
	my ($self, $index) = @_;
	$index = $index->to_number()->{value};

	return Null->new() if $index > $self->length()->{value};

	splice @{$self->{value}}, $index, 1;
}

# Returns the index of an element, or `Null` if it odesn't exist.
sub index_of {
	my @array = @{shift->{value}};
	my $element = shift;

	for (my $i = 0; $i < $#array; $i++) {
		return Number->new($i) if $element->eql($array[$i]);
	}

	Null->new();
}

# Gets an element out of the array.
sub index {
	my $result = shift->{value}[shift->to_number()->{value}];
	undef == $result ? Null->new() : $result;
}

# Adds another Array to this one by concating it.
sub add() {
	Array->new(@{shift->{value}}, @{shift->to_array()->{value}});
}

# Duplicates this array by the amount of elements given.
sub mul() {
	Array->new((@{shift->{value}}) x shift->to_number()->{value});
}

# Compares two arrays by returning `-1`, or `1` for the first value that doesn't compare,
# or `0` if they're equal.
sub cmp() {
	my @lhs = @{shift->{value}};
	my @rhs = @{shift->to_array()->{value}};
	my $cmp;

	for (my $i = 0; $i < scalar(@lhs); $i++) {
		$cmp = $lhs[$i]->cmp($rhs[$i]) and return Number->new($cmp);
	}

	Number->new(0);
}

# Checks to see if two arrays are equal.
sub eql {
	Boolean->new(shift->cmp(shift)->{value} == 0)
}

1;
