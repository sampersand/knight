#| All types in Knight are `Value`s.
#|
#| Unlike Raku, Knight does not have separate operators for integral and string-like types: There's one "add" operator,
#| one "multiplication" operator, one "equality" operator, etc. As such, overloading operators with Knight types would
#| not be very idiomatic: One would expect `Knight::String.new("12") + Knight::String.new("34")` to return a 
#| `Knight::Integer.new(46)`, _not_ `Knight::String.new("1234")`. Thus, I've opted to use methods, such as `add`, `mul`,
#| `eql`, etc.
unit role Knight::Value;

#| Assigns a value to `self`.
#|
#| All values (asides from `Identifier`s) are first converted to `Str`s, after which an `Identifier` is created and
#| assigned the given `$value`.
method assign(Knight::Value $value, --> Knight::Value) {
	require Knight::Identifier;
	::('Knight::Identifier').new($.Str).assign: $value
}

#| Checks to see if `self` is less than `$rhs`.
#|
#| This method simply checks to see if `cmp` is `Less`.
method lth(Knight::Value $rhs, --> Bool) {
	$.cmp($rhs) === Less
}

#| Checks to see if `self` is greater than `$rhs`.
#|
#| This method simply checks to see if `cmp` is `More`.
method gth(Knight::Value $rhs, --> Bool) {
	$.cmp($rhs) === More
}

#| Executes `self` and returns its result.
#|
#| For literals, such as `Number`s and `String`s, this simply returns `self`. For more complex types, such as
#| `Identifier` and `Function`, this should actually execute the value and return its result.
method run(--> Knight::Value) { … }

#| Compares `self` to `$rhs`, returning an `Order` determining if it's less than, greater, or equal to the other.
#|
#| This method is only used for the `gth` and `lth` functions; notably not `eql`. As such, it should coerce `$rhs` to
#| the proper type.
method cmp(Knight::Value $rhs, --> Order) { … }

#| Checks to see if `self` is equal to `$rhs`.
#|
#| As per the Knight specs, `eql` is the only function that does not automatically coerce the `$rhs`. As such, `$rhs`
#| must be the same type as `self`. Implementors should also use `multi`
multi method eql(Knight::Value $rhs, --> False) is pure { }

#| All `Value`s must be convertible to an `Int`.
method Int(--> Int) { … }

#| All `Value`s must be convertible to a `Str`.
method Str(--> Str) { … }

#| All `Value`s must be convertible to a `Bool`.
method Bool(--> Bool) { … }
