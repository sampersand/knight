our %ENVIRONMENT = (qwert => 4);
use strict;
use warnings;
use lib 'Kn';

use Kn::Number;
use Kn::Value;

=begin
print("" . Kn::Boolean->new(0),
	int Kn::Boolean->new(0),
	Kn::Boolean->new(0) ? "t" : "f");

print("" . Kn::Boolean->new(1),
	int Kn::Boolean->new(1),
	Kn::Boolean->new(1) ? "t" : "f");

print("" . Kn::Null->new(),
	int Kn::Null->new(),
	Kn::Null->new() ? "t" : "f");

print("" . Kn::Number->new(0),
	int Kn::Number->new(0),
	Kn::Number->new(0) ? "t" : "f");

print("" . Kn::Number->new(1),
	int Kn::Number->new(1),
	Kn::Number->new(1) ? "t" : "f");

print("" . Kn::Number->new(2),
	int Kn::Number->new(2),
	Kn::Number->new(2) ? "t" : "f");

print("" . Kn::Number->new(3),
	int Kn::Number->new(3),
	Kn::Number->new(3) ? "t" : "f");

print("" . Kn::Number->new(-1),
	int Kn::Number->new(-1),
	Kn::Number->new(-1) ? "t" : "f");

print("" . Kn::String->new(""),
	int Kn::String->new(""),
	Kn::String->new("") ? "t" : "f");

print("" . Kn::String->new("1"),
	int Kn::String->new("1"),
	Kn::String->new("1") ? "t" : "f");

print("" . Kn::String->new("0"),
	int Kn::String->new("0"),
	Kn::String->new("0") ? "t" : "f");

print("" . Kn::String->new("0 but true"),
	int Kn::String->new("0 but true"),
	Kn::String->new("0 but true") ? "t" : "f");
=cut

# my $s = "G "" 1 2++ '<' 4 '>'";
# # my $s = "G 'abcdefghijkl' 1 2 ''";
# print(
# 	Kn::Function->parse(\$s)->run()
# );

# =cut

# __END__

my $stream = $ARGV[0];
$stream = join("", <>);

Kn::Value->parse(\$stream)->run();
