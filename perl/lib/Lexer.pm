package Lexer;
use strict;
use warnings;

use lib '.';

use Ast::String;
use Ast::Number;
use Ast::Function;
use Ast::Identifier;

use overload "bool" => sub { $_[0]->stream };

sub new($$) {
	my ($class, $stream) = @_;

	bless { stream => $stream }, $class;
}

sub next_token($) {
	my ($self) = @_;

	return unless $self;
	goto &next_token if $self->{stream} =~ s/\A([\s(){}\[\]:)]|#.*)//;

	Ast::String->parse($self) ||
		Ast::Identifier->parse($self) ||
		Ast::Number->parse($self) ||
		Ast::Function::parse($self) ||
		die "bad token start '$self->{stream}->[0]'.";
}

1;
