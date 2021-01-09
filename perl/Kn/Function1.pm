=cut
package Kn::Function;
use strict;
use warnings;

use lib '..';

use overload '""' => \&interop;

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
