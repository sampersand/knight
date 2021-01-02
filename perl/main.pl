use strict;
use warnings;
use lib 'lib';

use Parser;

# my $parser = Parser->new("T O 4 ; = a 3 : O + a 4");
# # my $parser = Parser->new(" 'abc' 12 ae N T F 3");

# $\="\n";
my $env = { a => 3 };

# # print $parser->next()->run($env);
# my $res = $parser->next()->run($env);

my $res = Parser->new("! 0")->next()->run($env);

print $res, "\n";
