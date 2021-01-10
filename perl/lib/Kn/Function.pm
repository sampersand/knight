package Kn::Function;
use strict;
use warnings;
no warnings 'recursion';

use parent 'Kn::Value';

use Kn::Environment;

use overload
	'0+' => 'run',
	'""' => 'run',
	'bool' => 'run';

sub new(&@) {
	my ($class, $func, @args) = @_;

	bless { func => $func, args => \@args }, $class;
}

sub run {
	my $self = shift;

	$self->{func}->(@{$self->{args}})
} 

my %funcs = (
	'P' => [0, sub { Kn::String->new(scalar <STDIN>); }],
	'R' => [0, sub { Kn::Number->new(int rand 0xffffffff); }],
	'E' => [1, sub { my $val = "" . shift->run(); Kn::Value->parse(\$val)->run(); }],
	'B' => [1, sub { shift; }],
	'C' => [1, sub { shift->run()->run() }],
	'`' => [1, sub { my $cmd = "" . shift->run(); Kn::String->new(join '', `$cmd`); }],
	'Q' => [1, sub { exit int(shift->run()) }],
	'!' => [1, sub { Kn::Boolean->new(shift->run() ? 0 : 1); }],
	'L' => [1, sub { Kn::Number->new(length("" . shift->run())); }],
	'O' => [1, sub {
		my $val = shift->run();
		my $str = "$val";
		print(substr($str, -1) eq '\\' ? substr($str, 0, -1) : "$str\n");
		$val
	}],
	'+' => [2, sub { shift->run()->add(shift->run()); }],
	'-' => [2, sub { shift->run()->sub(shift->run()); }],
	'*' => [2, sub { shift->run()->mul(shift->run()); }],
	'/' => [2, sub { shift->run()->div(shift->run()); }],
	'%' => [2, sub { shift->run()->mod(shift->run()); }],
	'^' => [2, sub { shift->run()->pow(shift->run()); }],
	'?' => [2, sub { Kn::Boolean->new(shift->run()->eql(shift->run())); }],
	'<' => [2, sub { Kn::Boolean->new(shift->run()->lth(shift->run())); }],
	'>' => [2, sub { Kn::Boolean->new(shift->run()->gth(shift->run())); }],
	';' => [2, sub { shift->run(); shift->run(); }],
	'=' => [2, sub {
		my $var = shift;
		$var = $var->isa('Kn::Identifier') ? $var->{value} : "".$var->run();
		Kn::Environment->set($var, shift->run());
	}],
	'W' => [2, sub {
		my ($cond, $body) = @_;
		my $ret;

		$ret = $body->run() while ($cond->run() ? 1 : 0);
		defined($ret) ? $ret : Kn::Null->new();
	}],
	'&' => [2, sub { my $lhs = shift->run(); $lhs ? shift->run() : $lhs }],
	'|' => [2, sub { my $lhs = shift->run(); $lhs ? $lhs : shift->run() }],
	'I' => [3, sub { (shift->run() ? $_[0] : $_[1])->run() }],
	'G' => [3, sub {
		no warnings;
		Kn::String->new("" . substr("".shift->run(), int(shift->run()), int(shift->run()))); 
		}],
	'S' => [4, sub {
		my $str = "" . shift;
		my $start = int shift;
		my $len = int shift;
		my $repl = "" . shift;

		no warnings;
		my $x = (substr($str, 0, $start)) . $repl . (substr($str, $start + $len));
		Kn::String->new($x);
	}]
);

sub parse {
	my ($class, $stream) = @_;

	$$stream =~ s$\A[A-Z]+|\A[-+*/%^<?>&|!`;=]$$p or return;
	my $op = substr ${^MATCH}, 0, 1;
	die "unknown function '$op'" unless $funcs{$op};

	my ($arity, $func) = @{$funcs{$op}};

	my $ret = $class->new($func, map { Kn::Value->parse($stream) } (1..$arity));
	$ret
}

1;
