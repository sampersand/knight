package Kn::Functions;
use strict;
use warnings;

use lib '.';

use String;
use Number;
use Boolean;
use Null;
use Function;
use Identifier;

sub prompt($) {
	Kn::String->new(scalar <>);
}

sub random($) {
	Kn::Number->new(int rand 0xffffffff);
}

sub eval($$) {
	die("todo");
}

sub block($$) {
	shift;

	shift;
}

sub call($$) {
	shift;

	shift->run()->run();
}

sub system($$) {
	shift;

	my $command = shift;

	Kn::String->new(`$command`);
}

sub quit($$) {
	shift;

	exit shift;
}

sub not($$) {
	shift;

	Kn::Boolean->new(!shift);
}

sub length($$) {
	shift;

	Kn::Number->new(CORE::length(shift));
}

sub output($$) {
	shift;

	my $result = shift->run();
	my $text = "$result";

	if (substr($text, -1) == '\\') {
		print substr $text, 0, -1;
	} else {
		print "$text\n";
	}

	$result;
}

sub add($$$) {
	shift; # get rid of class

	shift->add(shift);
}

sub sub($$$) {
	shift;

	shift->sub(shift);
}

sub mul($$$) {
	shift;

	shift->mul(shift);
}

sub div($$$) {
	shift;

	shift->div(shift);
}

sub mod($$$) {
	shift;

	shift->mod(shift);
}

sub pow($$$) {
	shift;

	shift->pow(shift);
}

sub eql($$$) {
	shift;

	Kn::Boolean->new(shift->eql(shift));
}

sub lth($$$) {
	shift;

	Kn::Boolean->new(shift->cmp(shift) < 0);
}

sub gth($$$) {
	shift;

	Kn::Boolean->new(shift->cmp(shift) > 0);
}

sub and($$$) {
	shift;

	shift->run() && shift->run();
}

sub or($$$) {
	shift;

	shift->run() || shift->run();
}

sub then($$$) {
	shift;

	shift->run();
	shift->run();
}

sub while($$$) {
	shift;

	my ($cond, $body) = @_;

	my $ret = Kn::Null->new();
	$ret = $body->run() while $cond;
	$ret;
}

sub assign($$$) {
	shift;

	my $var = shift;
	our %ENVIRONMENT;

	$var = $var->isa('Kn::Identifier') ? $var->{value} : "$var";

	$ENVIRONMENT{$var} = shift->run();
}

sub if($$$$) {
	shift;

	(shift() ? shift : pop)->run()
}

sub get($$$$) {
	shift;

	no warnings; # subscripting out of range is fine.

	Kn::String->new(substr(shift, shift, shift) || "");
}

sub set($$$$$) {
	shift;

	my $str = "" . shift;
	my $start = 0 + shift;
	my $len = 0 + shift;
	my $repl = "" . shift;

	Kn::String->new(substr($str, 0, $start) . $repl . substr($str, $start + $len));
}

1;
