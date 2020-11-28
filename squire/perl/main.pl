use strict;
use warnings;
use lib 'lib';

use Lexer;
use Types::Array;
my $x = Lexer->new("abc");
print $x;

__END__
BEGIN { use File::Basename; push @INC, dirname(__FILE__) . '/types'; }

use Types::Boolean;
use Types::Null;
use Types::Number;
use Types::String;
use Types::Array;
use Lexer;


my $lexer = Lexer->new(" ;");
print $lexer->next();
# print !!$lexer;
# print $lexer->advance();

__END__
my $num = Number->new(12);
my $arr = Array->new(
	String->new('a'),
	String->new('b'),
	String->new('c'),
	Number->new(0),
	Number->new(1),
	Number->new(2)
);

$\="\n";
print $arr;
$arr->insert(Number->new(2), String->new("abc"));
$arr->set_index(Number->new(9), String->new("?"));
$arr->delete(Number->new(7));
my $x = "123 a";
print Number->parse($x);
print $x;
