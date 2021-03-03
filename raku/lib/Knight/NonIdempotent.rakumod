use Knight::Value;

#| A role used to indicate that, when run, a value may yield different results.
unit role Knight::NonIdempotent;

#| NonIdempotent types must implement `run`.
method run(--> Knight::Value) { … }

#| Runs `self` and compares it against `$rhs`.
method cmp(Knight::Value $rhs, --> Order) {
	$.run.cmp: $rhs
}

#| Runs `self` and checks to see if it's return value is equal to `$rhs`.
multi method eql(::?CLASS $rhs, --> Bool) {
	$.run.eql: $rhs
}

#| Runs `self` and converts the result to a `Str`.
method Str(--> Str) {
	~$.run
}

#| Runs `self` and converts the result to a `Bool`.
method Bool(--> Bool) {
	?$.run
}

#| Runs `self` and converts the result to an `Int`.
method Int(--> Int) {
	+$.run
}

