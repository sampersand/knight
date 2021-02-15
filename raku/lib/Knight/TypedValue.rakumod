use Knight::Value;

# This role is used for literal values (asides from `Null`) within Knight, as they all have an associated Raku type
# That we're essentially wrapping.
#
# - `::T` type should be the builtin type, such as `Bool` or `Str`.
# - `$cmp` should be a binary function that returns an `Order` describing the first value's relation to the second.
unit role Knight::TypedValue[::T, $cmp, $eql] does Knight::Value;

# The value that the implementor has
#
# For example, Booleans have a `Bool` `$#value`.
has T $!value is built;

# Creates a new `self` with the given `$value`.
method new(T $value) {
	self.bless :$value
}

# Builds a new `self` with the given value.
#
# This is generally only called from `new`
method BUILD(T :$value) {
	$!value = $value
}

# Compares `self` with the `$rhs`.
#
# This simply calls `$cmp` with our `$!value` and `$rhs` transformed into a `T`.
method cmp(Knight::Value $rhs, --> Order) {
	$cmp($!value, T($rhs))
}

# Checks to see if `self` is equal to `$rhs`.
#
# According to the Knight specifications, only values that have the same type and values are equal
multi method eql(::?CLASS $rhs, --> Bool) {
	$eql($!value, T($rhs))
}

# Converts `self` to a string by converting `$!value` to a `Str`.
method Str(--> Str) #`(is pure) {
	$!value.Str
}

# Converts `self` to a boolean by converting `$!value` to a `Bool`.
method Bool(--> Bool) #`(is pure) {
	$!value.Bool
}

# Converts `self` to an integer by converting `$!value` to a `Inta`.
method Int(--> Int) #`(is pure) {
	$!value.Int
}

# Running a `TypedValue`, you simply return `self`.
method run(--> Knight::Value) #`(is pure) {
	self
}
