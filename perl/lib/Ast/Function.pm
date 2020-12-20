package Ast::Function;
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
	return Ast::String->new(<STDIN>) if $op eq 'P';

	### Arity 1 ###
	# `BLOCK` is weird and shouldn't evaluate its parameter...
	return $args[0] if $op eq 'B';

	return Ast::Number->new(length $args[0]) if $op eq 'L';
	return 1 if $op =~ /[OEBCQ`!L]/;
	return 2 if $op =~ qr$[-+*/^<>&|!;=WR]$;
	return 3 if $op =~ /[IG]/;
	return 4 if $op eq 'S';


}

1;
