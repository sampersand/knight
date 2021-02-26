use Knight::TypedValue;
use Knight::Value;

#| The number type in Knight.
#|
#| As per the Knights specifications, numbers are integral numbers only.
#|
#| Because implementations are allowed to define the maximum and minimum values, I've chosen to use Raku's
#| `Int` type, as it allows for arbitrary-precision arithmetic.
unit class Knight::Number does Knight::TypedValue[Int, * <=> *, * == *];

#| Gets an internal representation of the class; used in debugging.
method gist(--> Str) {
	"Number($.Str)";
}

#| Converts `$rhs` to an `Int`, then adds it to `self`, returning a new Number.
method add(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value + $rhs.Int
}

#| Converts `$rhs` to an `Int`, then subtracts it from `self`, returning a new Number.
method sub(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value - $rhs.Int
}

#| Converts `$rhs` to an `Int`, then multiplies `self` by it, returning a new Number.
method mul(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value * $rhs.Int
}

#| Converts `$rhs` to an `Int`, then divides `self` by it (rounding down), returning a new Number.
#|
#| `$rhs` may not be zero when converted to an int, or the program will `die`.
method div(Knight::Value $rhs, --> ::?CLASS) {
	# We have to use `(x / y).Int`, as `div` rounds incorrectly with negative numbers.
	::?CLASS.new: ($!value / ($rhs.Int or die 'Cannot divide by zero!')).Int
}

#| Converts `$rhs` to an `Int`, then modulos `self` by it, returning a new Number.
#|
#| `$rhs` may not be zero when converted to an int, or the program will `die`.
method mod(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: $!value mod ($rhs.Int or die 'Cannot modulo by zero!')
}

#| Converts `$rhs` to an `Int`, then raises `self` to its power.
method pow(Knight::Value $rhs, --> ::?CLASS) {
	::?CLASS.new: ($!value ** $rhs.Int).Int
}
