use Knight::TypedValue;
use Knight::Value;

unit class Knight::String does Knight::TypedValue[Str, * cmp *, * eq *];

method Int(--> Int) #`(is pure) {
	$!value ~~ /^ <[\d]>* /;
	$<>.Int
}

method add(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value ~ $rhs.Str
}

method mul(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value x $rhs.Str
}

