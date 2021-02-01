unit module Knight;


role Null {}
multi sub infix:«<»(Null $, $) { die "cannot compare null"; }
multi sub infix:«>»(Null $, $) { die "cannot compare null"; }

role Boolean {}
multi sub infix:«<»(Boolean $lhs, $rhs) { $lhs < ?$rhs but Boolean }
multi sub infix:«>»(Boolean $lhs, $rhs) { $lhs > ?$rhs but Boolean }

role String {
	method Int(--> Int) {
		self ~~ /^ <[\d]>* /;
		$<>.Int
	}

	method Numeric(--> Numeric) { $.Int }
	method Real(--> Real) { $.Int }
}

multi sub infix:«+»(String $lhs, $rhs) { $lhs ~ $rhs but String }
multi sub infix:«*»(String $lhs, $rhs) { $lhs x $rhs but String }
multi sub infix:«<»(String $lhs, $rhs) { $lhs lt $rhs but Boolean }
multi sub infix:«>»(String $lhs, $rhs) { $lhs gt $rhs but Boolean }

role Number {}
multi sub infix:«div»(Number $lhs, $rhs) {
	exit;
	($lhs but not Number) div ($rhs.Int or die "Cannot divide by zero") but Number
}

my $x = 3  Number;
say $x div 0;
exit;

=finish
my $x = "foo" but String;
say $x * 4;
=finish
# my $z = "quux" but $x.^roles;
# say $x.^roles;
# say $z + 3;
# exit;


say $x + 3;
exit;

class Value {
	method new($value) { self.bless :$value }

	method Str(--> Str) { $.run.value.Str }
	method Bool(--> Bool) { $.run.value.Bool }
	method Int(--> Int) { $.run.value.Int }
	method Numeric(--> Numeric) { $.Int }
	method Real(--> Real) { $.Int }

	method run(--> Value) { self }
}

#multi sub infix:«===»(Value $, $, --> Bool) { False }

# class Boolean is Value {
# 	has Bool $.value;
# 
# 	method Str(--> Str) {
# 		$.value ?? "true" !! "false"
# 	}
# }


=finish
use MONKEY-TYPING;


role Value does Numeric {
	method add(Value $rhs, --> Value) { self + $rhs }
}

augment class Int but Value {
}
#augment class Int does Value {}
say 12.add(34);
=finish
#`(
            div|mod|gcd|lcm|eq|ne|lt|gt|le|ge|leg|(uni)?cmp|x|xx|o|temp|let|
            does|but|before|after|equiv|min|max|minmax|
            not|so|and|(not)?andthen|or(else)?|xor
)
exit;

say 12.add(34);

=finish

my $x = [] but Value;
say $x.cos;

=finish
class Foo does Real {

}

say Foo.new.^methods;
#say Foo.new;

say "a".^roles;
say Numeric.^methods;
#say True.^roles
