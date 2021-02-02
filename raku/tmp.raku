unit module Knight;

class Value {
	method new($value, --> ::?CLASS) { self.bless :$value }

	method Str(--> Str) { $.run.value.Str }
	method Bool(--> Bool) { $.run.value.Bool }
	method Int(--> Int) { $.run.value.Int }

	method run(--> Value) is pure { self }

	method lth(Value $rhs, --> Bool) is pure { $.cmp($rhs) === Less }
	method gth(Value $rhs, --> Bool) is pure { $.cmp($rhs) === More }
	multi method eql(Value $, --> Bool) is pure { False };
}

class Boolean is Value {
	has Bool $.value;

	method Str(--> Str) is pure { $.value ?? "true" !! "false"}
	method cmp(Value $rhs, --> Order) is pure { $.value <=> ?$rhs }

	multi method eql(Boolean $rhs, --> Bool) { $.value == $rhs.value }
}

say Boolean.new(True).cmp(Boolean.new(False));
