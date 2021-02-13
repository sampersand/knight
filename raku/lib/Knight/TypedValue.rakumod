# This role is used for literal values (asides from `Null`) within Knight, as they all have an associated Raku type
# That we're essentially wrapping.
#
# - `::T` type should be the builtin type, such as `Bool` or `Str`.
# - `$cmp` should be a binary function that returns an `Order` describing the first value's relation to the second.
unit role TypedValue[::T, $cmp, $eql] does Value;

has T $!value is built;

method new(T $value)    { self.bless :$value }
method BUILD(T :$value) { $!value = $value }

method cmp(Value $rhs, --> Order) { $cmp($!value, T($rhs)) }
multi method eql(::?CLASS $rhs, --> Bool) { $eql($!value, T($rhs)) }

method Str(--> Str) is pure   { $!value.Str }
method Bool(--> Bool) is pure { $!value.Bool }
method Int(--> Int) is pure   { $!value.Int }
method run(--> Value) is pure { self }
