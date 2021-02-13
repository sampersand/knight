unit class Number does TypedValue[Int, * <=> *, * == *] is export;

method add(Value $rhs, --> Number) { Number.new: $!value + $rhs.Int }
method sub(Value $rhs, --> Number) { Number.new: $!value - $rhs.Int }
method mul(Value $rhs, --> Number) { Number.new: $!value * $rhs.Int }
method div(Value $rhs, --> Number) { Number.new: $!value div ($rhs.Int or die "Cannot divide by zero!") }
method mod(Value $rhs, --> Number) { Number.new: $!value mod ($rhs.Int or die "Cannot modulo by zero!") }
method pow(Value $rhs, --> Number) { Number.new: $!value ** $rhs.Int }
