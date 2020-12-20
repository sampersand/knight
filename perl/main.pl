use strict;
use warnings;
use lib 'lib';

use Parser;

my $parser = Parser->new("L 'a'");
# my $parser = Parser->new(" 'abc' 12 ae N T F 3");

$\="\n";
my $env = { a => 3 };

print $parser->next();
my $res = $parser->next()->run($env);

print $res;
