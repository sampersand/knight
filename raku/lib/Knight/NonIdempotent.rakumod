unit role NonIdempotent;

method run(--> Value) { ... }

method cmp(Value $rhs, --> Order) {
	$.run.cmp($rhs)
}

multi method eql(::?CLASS $rhs, --> Bool) {
	$.run.cmp($rhs)
}

method Str(--> Str)   { $.run.Str }
method Bool(--> Bool) { $.run.Bool }
method Int(--> Int)   { $.run.Int }
