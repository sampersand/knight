unit class String does TypedValue[Str, * cmp *, * eq *];

method Int(--> Int) is pure {
	$!value ~~ /^ <[\d]>* /;
	$<>.Int
}

method add(Value $rhs, --> String) { String.new: $!value ~ $rhs.Str }
method mul(Value $rhs, --> String) { String.new: $!value x $rhs.Str }
