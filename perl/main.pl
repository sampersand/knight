use strict;
use warnings;
use lib 'Kn';


use Kn::Value;
use Kn::Boolean;
use Kn::Function;

my $stream = $ARGV[0];
Kn::Function->parse(\$stream)->run();
=cut

exit;
print(Kn::Functions->add(Kn::Number->new(1), Kn::Boolean->new(1)));

print &length(Kn::String->new("123456 "));

__END__
use Kn::Parser;

# my $parser = Parser->new("T O 4 ; = a 3 : O + a 4");
# # my $parser = Parser->new(" 'abc' 12 ae N T F 3");

# $\="\n";
my $env = { a => 3 };

# # print $parser->next()->run($env);
# my $res = $parser->next()->run($env);

my $res = Kn::Parser->new("! 0")->next()->run($env);

print $res, "\n";
