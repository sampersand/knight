package Parser;
use strict;
use warnings;

use lib '.';

use Ast::String;
use Ast::Number;
use Ast::Boolean;
use Ast::Null;
use Ast::Function;
use Ast::Identifier;

use overload
	'""'  => sub { $_[0]->{stream} },
	'${}' => sub { \$_[0]->{stream} };

sub new($$) {
	my ($class, $stream) = @_;

	bless { stream => $stream }, $class;
}

sub next($) {
	my ($self) = @_;

	undef while $$self =~ s/\A([\s(){}\[\]:)]|#.*)//;

	return unless $self;

	my $token;
	$token = Ast::String->parse($self) and print "String", return $token;
	$token = Ast::Identifier->parse($self) and print "Identifier", return $token;
	$token = Ast::Number->parse($self) and print "Number", return $token;
	$token = Ast::Boolean->parse($self) and print "Boolean", return $token;
	$token = Ast::Null->parse($self) and print "Null", return $token;
	$token = Ast::Function->parse($self) and print "Function", return $token;
	die "bad token start '$self'.";
}

1;
