package Kn::Function;
use strict;
use warnings;

use lib '..';
use parent 'Kn::Value';

use overload
	'0+' => 'run',
	'""' => 'run',
	'bool' => 'run';

sub new(&@) {
	my ($class, $func, @args) = @_;

	bless { func => $func, args => \@args }, $class;
}

sub run($) {
	my $self = shift;

	$self->{func}->(@{$self->{args}})
} 

my %funcs = (
	'P' => [0, sub() { Kn::String->new(scalar <> ); }],
	'R' => [0, sub() { Kn::Number->new(int rand 0xffffffff); }],
	'E' => [1, sub($) { die "todo"; }],
	'B' => [1, sub($) { shift; }],
	'C' => [1, sub($) { shift->run()->run(); }],
	'`' => [1, sub($) { my $cmd = shift; Kn::String->new(`$cmd`); }],
	'Q' => [1, sub($) { exit shift }],
	'!' => [1, sub($) { Kn::Boolean->new(!shift); }],
	'L' => [1, sub($) { Kn::Number->new(length(shift)); }],
	'O' => [1, sub($) {
		my $val = shift->run();
		my $str = "$val";
		print(substr($str, -1) == '\\' ? substr($str, 0, -1) : "$str\n");
		$val
	}],
	'+' => [2, sub($$) { shift->add(shift); }],
	'-' => [2, sub($$) { shift->sub(shift); }],
	'*' => [2, sub($$) { shift->mul(shift); }],
	'/' => [2, sub($$) { shift->div(shift); }],
	'%' => [2, sub($$) { shift->mod(shift); }],
	'^' => [2, sub($$) { shift->pow(shift); }],
	'?' => [2, sub($$) { Kn::Boolean->new(shift->eql(shift)); }],
	'<' => [2, sub($$) { Kn::Boolean->new(shift->lth(shift)); }],
	'>' => [2, sub($$) { Kn::Boolean->new(shift->gth(shift)); }],
	';' => [2, sub($$) { shift->run(); shift->run(); }],
	'=' => [2, sub($$) {
		our %ENVIRONMENT;
		my $var = shift;
		$var = $var->isa('Kn::Identifier') ? $var->{value} : "$var";
		$ENVIRONMENT{$var} = shift->run();
	}],
	'W' => [2, sub($$) {
		my ($cond, $body) = @_;
		my $ret;

		$ret = $body->run() while $cond;
		defined($ret) ? $ret : Kn::Null->new();
	}],
	'&' => [2, sub($$) { my $lhs = shift->run(); $lhs ? shift->run() : $lhs }],
	'|' => [2, sub($$) { my $lhs = shift->run(); $lhs ? $lhs : shift->run() }],
	'I' => [3, sub($$$) { (shift() ? shift : pop)->run() }],
	'G' => [3, sub($$$) { no warnings; Kn::String->new(substr(shift, shift, shift) || ""); }],
	'S' => [4, sub($$$$) {
		my $str = "" . shift;
		my $start = 0 + shift;
		my $len = 0 + shift;
		my $repl = "" . shift;

		Kn::String->new(substr($str, 0, $start) . $repl . substr($str, $start + $len));
	}]
);

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s$\A([A-Z]+|[-+*/^<>&|!;=])$$p or return;
	my $op = substr ${^MATCH}, 0, 1;

	my ($arity, $func) = @{$funcs{$op} || die "unknown function '$op'"};

	$class->new($func, map { Kn::Value->parse($stream) } (1..$arity));
}

=cut
__END__
sub new($$@) {
	my ($class, $value, @args) = @_;

	bless { value => $value, args => \@args }, $class;
}

sub interop($;$) {
	my ($self, $index) = @_;
	$index = 1 unless defined $index;
	my $ret = $self->{value};

	for (0..arity($self->{value}) - 1) {
		$ret .= "\n" . ("  " x $index);
		my $arg = $self->{args}[$_];

		$ret .= $arg->isa(__PACKAGE__) ? $arg->interop($index + 1) : "$arg";
	}

	$ret;
}

sub arity($) {
	my $op = shift;

	return 0 if $op eq 'P';
	return 1 if $op =~ /[OEBCQ`!L]/;
	return 2 if $op =~ qr$[-+*/^<>&|!;=WR]$;
	return 3 if $op =~ /[IG]/;
	return 4 if $op eq 'S';

	die "unknown operator '$op'.";
}

sub parse($$) {
	my ($class, $stream) = @_;

	$$stream =~ s$\A([A-Z]+|[-+*/^<>&|!;=])$$p or return;
	my $op = substr ${^MATCH}, 0, 1;
	return $class->new($op, map { $stream->next() } (1..arity($op)));
	die "unknown operator '$op'.";
}

sub run($$) {
	my ($self, $env) = @_;

	my $op = $self->{value};
	my @args = @{$self->{args}};

	### Arity 0 ###
	return Kn::String->new(<STDIN>) if $op eq 'P';
	return Kn::Number->new(int(rand() * ~0)) if $op eq 'R';


	### Arity 1 ###
	return Parser->new($args[0]->to_string())->next()->run() if $op eq 'E';
	return $args[0] if $op eq 'B';
	return $args[0]->run() if $op eq 'C';
	return `$args[0]->to_string()` if $op eq '`';
	exit $args[0]->to_number()->value if $op eq 'Q';
	print "<", $args[0]->to_boolean(), ">";
	exit;
	# return $args[0]->
=begin
my $parser = Parser->new("T O 4 ; = a 3 : O + a 4");

	KN_TT_EVAL = ARITY_INCR,
	KN_TT_BLOCK, 
	KN_TT_CALL,
	KN_TT_SYS,
	KN_TT_QUIT, 
	KN_TT_NOT,
	KN_TT_LENGTH,
	KN_TT_OUPTUT,

 =cut
	return Kn::Number->new(length $args[0]) if $op eq 'L';
	return 1 if $op =~ /[OEBCQ`!L]/;
	return 2 if $op =~ qr$[-+*/^<>&|!;=WR]$;
	return 3 if $op =~ /[IG]/;
	return 4 if $op eq 'S';

	die "unknown op $op";
}

1;
