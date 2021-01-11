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

sub new {
	my ($class, $func, @args) = @_;

	bless { func => $func, args => \@args }, $class;
}

sub run {
	$_[0]->{func}->run(@{$_[0]->{args}})
} 

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s$\A[A-Z]+|\A[-+*/%^<?>&|!`;=]$$p or return;
	my $op = substr ${^MATCH}, 0, 1;
	my $func = Kn::Function->get($op) or die "unknown function '$op'";

	my $ret = $class->new($func,
		map { Kn::Value->parse($stream) } (1..$func->arity())
	);
	$ret
}

1;
