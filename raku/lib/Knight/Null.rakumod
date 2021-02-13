# The "null" type within Knight.
#
# Like null it other langues, `Null` is a unit type, and as such all of its methods are pure.
unit module Knight::Null;

# `Null`'s string representation is simply `null`.
method Str(--> 'null') is pure { }

# `Null` is always falsey.
method Bool(--> False) is pure { }

# `Null`s are always zero.
method Int(--> 0) is pure { }

# You're not allowed to compare nulls to other values.
method cmp(Value $) {
	die 'Cannot compare Null.'
}

# `Null` is always equal to itself.
multi method eql(Null $, --> True) is pure { }

# Running `Null` is a no op.
method run(--> Value) is pure { self }
