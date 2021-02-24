package Kn::Ast;

use strict;
use warnings;
no warnings 'recursion';

use parent 'Kn::Value';

use Kn::Function;

use overload
	'0+' => 'run',
	'""' => 'run',
	'bool' => 'run';

# Creates a new `Ast` with the given function, operator name, and arguments.
#
# While not strictly necessary, the operator name is used for debugging.
sub new {
	my ($class, $func, $op, @args) = @_;

	bless { func => $func, op => $op, args => \@args }, $class;
}

# An Ast is only equivalent to itself.
sub eql {
	use Scalar::Util 'refaddr';
	my ($lhs, $rhs) = @_;

	ref($lhs) eq ref($rhs) && refaddr($lhs) == refaddr($rhs);
}


sub run {
	$_[0]->{func}->run(@{$_[0]->{args}})
} 

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s$\A[A-Z]+|\A[-+*/%^<?>&|!`;=]$$p or return;
	my $op = substr ${^MATCH}, 0, 1;
	my $func = Kn::Function->get($op) or die "unknown function '$op'";

	my $ret = $class->new($func, $op,
		map { Kn::Value->parse($stream) } (1..$func->arity())
	);
	$ret
}

# Dumps the class's info. Used for debugging.
sub dump {
	my $this = shift;
	my $ret = "Function($this->{op}";

	for my $x(@{$this->{args}}) {
		$ret .= ', ' . $x->dump();
	};

	"$ret)";
}

1;
