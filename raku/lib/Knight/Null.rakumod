use Knight::Value;
use Knight::TypedValue;

#| The "null" type within Knight.
unit class Knight::Null does Knight::Value;

#| The sole instance of this class.
my ::?CLASS \INSTANCE = ::?CLASS.bless; # ::?CLASS.bless;

#| Creates a new Null instance.
method new(--> ::?CLASS) is pure { INSTANCE }

#| Simply returns `"null"`.
method Str(--> 'null') is pure { }

#| Simply returns `0`.
method Int(--> 0) is pure { }

#| Simply returns `False`.
method Bool(--> False) is pure { }

#| Comparison to Null is invalid.
#|
#| If attempted the program will `die`.
method cmp(Knight::Value $, --> Order) {
	die 'Cannot compare Null.';
}

#| All Nulls are equal to eachother.
multi method eql(::?CLASS $, --> True) is pure { }

#| Running a `Null` simply returns itself.
method run(--> Knight::Value) is pure { self }

#| Gets an internal representation of the class; used in debugging.
method gist(--> Str) {
	"Null()";
}
