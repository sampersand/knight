use Knight::Value;
use Knight::NonIdempotent;

#| An identifier within Knight.
#|
#| As per the Knight specs, all variables are global.
unit class Knight::Identifier does Knight::Value does Knight::NonIdempotent;

#| The list of all known identifiers.
my %ALL;

#| The identifier for this string.
has Str $!ident is built;

#| Creates a new identifier with the given variable name.
method new(Str $ident, --> ::?CLASS) {
	self.bless :$ident
}

#| Fetches the value associated with this identifier.
#|
#| If the identifier hasn't been assigned before, the program `die`s.
method run(--> Knight::Value)  {
	die "unknown variable '$!ident'" unless $!ident ~~ %ALL;

	%ALL{$!ident}
}

#| Assigns a value to this identifier.
#|
#| Note that `$value` is evaluated.
method assign(Knight::Value $value, --> Knight::Value) {
	my $result = $value.run;

	%ALL{$!ident} = $result; # needs to be evaluated so `= a O 3` will have `3` printed.

	$result
}

#| Gets an internal representation of the class; used in debugging.
method gist(--> Str) {
	"Identifier($!ident)";
}
