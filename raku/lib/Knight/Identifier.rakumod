unit class Identifier does NonIdempotent is export;

has Str $!ident is built;

my %ENV;

method new(Str $ident) { self.bless :$ident }

method run(--> Value)  {
	unless $!ident ~~ %ENV {
		say %ENV.keys;
		die "unknown variable '$!ident'" unless $!ident ~~ %ENV;
	}
	%ENV{$!ident}
}

method assign(Value $value, --> Value) {
	say $!ident;
	%ENV{$!ident} = $value;
	$value
}
