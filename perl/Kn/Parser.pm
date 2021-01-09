package Kn::Parser;
use strict;
use warnings;

use lib '.';

use Kn::String;
use Kn::Number;
use Kn::Boolean;
use Kn::Null;
use Kn::Function;
use Kn::Identifier;

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
	$token = Kn::String->parse($self) and print "String", return $token;
	$token = Kn::Identifier->parse($self) and print "Identifier", return $token;
	$token = Kn::Number->parse($self) and print "Number", return $token;
	$token = Kn::Boolean->parse($self) and print "Boolean", return $token;
	$token = Kn::Null->parse($self) and print "Null", return $token;
	$token = Kn::Function->parse($self) and print "Function", return $token;
	die "bad token start '$self'.";
}

1;
